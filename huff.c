#include "bitreader.h"
#include "bitwriter.h"
#include "node.h"
#include "pq.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Code {
    uint64_t code;
    uint8_t code_length;
} Code;

Code code_table[256];

uint32_t fill_histogram(FILE *fin, uint32_t *histogram) {
    // Clear all elements of the histogram array
    for (int i = 0; i < 256; ++i) {
        histogram[i] = 0;
    }

    uint32_t filesize = 0;
    int byte;

    // Read bytes from the input file and update histogram
    while ((byte = fgetc(fin)) != EOF) {
        ++histogram[byte];
        ++filesize;
    }

    // Increment two different bins of the histogram
    ++histogram[0x00];
    ++histogram[0xff];

    return filesize;
}

Node *create_tree(uint32_t *histogram, uint16_t *num_leaves) {
    // Create a priority queue
    PriorityQueue *pq = pq_create();

    // Count the number of non-zero histogram entries
    *num_leaves = 0;
    for (int i = 0; i < 256; ++i) {
        if (histogram[i] > 0) {
            // Create a node for each non-zero histogram entry
            Node *new_node = node_create((uint8_t) i, histogram[i]);
            enqueue(pq, new_node);
            (*num_leaves)++;
            //            node_free(&new_node);
        }
    }

    // Run Huffman Coding algorithm
    while (pq_size_is_1(pq) != true && pq_is_empty(pq) != true) {
        // Dequeue two nodes with the lowest weights
        Node *left = dequeue(pq);
        Node *right = dequeue(pq);

        // Create a new node with weight = sum of the weights of left and right nodes
        Node *new_node
            = node_create(0, left->weight + right->weight); // Symbol = 0 for internal nodes
        new_node->left = left;
        new_node->right = right;

        // Enqueue the new node
        enqueue(pq, new_node);
    }

    // Dequeue the queue's only entry and return it (the root of the Huffman tree)
    Node *huffman_tree = dequeue(pq);

    // Free the priority queue
    pq_free(&pq);

    return huffman_tree;
}

void free_tree(Node *root) {
    if (root == NULL) {
        return;
    }

    // Recursively free left and right subtrees
    free_tree(root->left);
    free_tree(root->right);

    // Free the current node
    node_free(&root);
}

void fill_code_table(Code *code_table, Node *node, uint64_t code, uint8_t code_length) {
    if (node == NULL) {
        exit(1);
    }

    if (node->left != NULL && node->right != NULL) { // Internal node
        // Recursive call to the left child (append 0 to the code)
        fill_code_table(code_table, node->left, code, code_length + 1);

        // Set bit at code_length for the right child and recurse (append 1 to the code)
        code |= ((uint64_t) 1 << code_length);
        fill_code_table(code_table, node->right, code, code_length + 1);
    } else { // Leaf node
        // Store Huffman code for the leaf node's symbol
        code_table[node->symbol].code = code;
        code_table[node->symbol].code_length = code_length;
    }
}

void huff_write_tree(BitWriter *outbuf, Node *node) {
    if (node->left == NULL && node->right == NULL) {
        // Node is a leaf
        bit_write_bit(outbuf, 1); // Write bit 1 to indicate a leaf node
        bit_write_uint8(outbuf, node->symbol); // Write the symbol of the leaf node
    } else {
        // Node is internal
        huff_write_tree(outbuf, node->left); // Recursively write left subtree
        huff_write_tree(outbuf, node->right); // Recursively write right subtree
        bit_write_bit(outbuf, 0); // Write bit 0 to indicate an internal node
    }
}

void huff_compress_file(BitWriter *outbuf, FILE *fin, uint32_t filesize, uint16_t num_leaves,
    Node *code_tree, Code *code_table) {
    // Writing file signature and metadata to the output buffer
    bit_write_uint8(outbuf, 'H');
    bit_write_uint8(outbuf, 'C');
    bit_write_uint32(outbuf, filesize);
    bit_write_uint16(outbuf, num_leaves);

    // Write the Huffman tree to the output buffer
    huff_write_tree(outbuf, code_tree); // Function to write the Huffman tree

    while (1) {
        int b = fgetc(fin);
        if (b == EOF) {
            break;
        }

        uint64_t code = code_table[b].code;
        uint8_t code_length = code_table[b].code_length;

        for (uint8_t i = 0; i < code_length; ++i) {
            bit_write_bit(outbuf, (code & 1)); // Write the bit to the output buffer
            code >>= 1;
        }
    }
}

int main(int argc, char *argv[]) {
    char *input_file = NULL;
    char *output_file = NULL;

    // Process command-line arguments
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            input_file = argv[i + 1];
            ++i;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[i + 1];
            ++i;
        }
    }

    if (input_file == NULL || output_file == NULL) {
        return 1;
    }

    FILE *input_fp = fopen(input_file, "rb");
    if (input_fp == NULL) {
        fclose(input_fp);
        return 1;
    }

    FILE *output_fp = fopen(output_file, "wb");
    if (output_fp == NULL) {
        fclose(input_fp);
        fclose(output_fp);
        return 1;
    }
    BitWriter *outbuf = bit_write_open(output_file); // Initialize BitWriter

    uint32_t histogram[256];
    uint32_t filesize = fill_histogram(input_fp, histogram);

    uint16_t num_leaves = 0;
    Node *huffman_tree = create_tree(histogram, &num_leaves);

    fill_code_table(code_table, huffman_tree, 0, 0);

    if (fseek(input_fp, 0, SEEK_SET) != 0) {
        node_free(&huffman_tree);
        bit_write_close(&outbuf);
        fclose(input_fp);
        return 1;
    }

    huff_compress_file(outbuf, input_fp, filesize, num_leaves, huffman_tree, code_table);

    // Close the files, free memory, etc.
    bit_write_close(&outbuf);
    fclose(input_fp);
    fclose(output_fp);
    // Free memory, close buffers, etc.
    node_free(&huffman_tree);

    return 0;
}
