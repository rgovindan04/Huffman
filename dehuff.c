#include "bitreader.h"
#include "bitwriter.h"
#include "node.h"
#include "pq.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void stack_push(Node *stack[], int *top, Node *node) {
    if (*top < 64) {
        stack[++(*top)] = node;
    } else {
        exit(1);
    }
}

Node *stack_pop(Node *stack[], int *top) {
    if (*top >= 0) {
        return stack[(*top)--];
    } else {
        exit(1);
    }
}

void dehuff_decompress_file(FILE *fout, BitReader *inbuf) {
    uint8_t type1 = bit_read_uint8(inbuf);
    uint8_t type2 = bit_read_uint8(inbuf);
    uint32_t filesize = bit_read_uint32(inbuf);
    uint16_t num_leaves = bit_read_uint16(inbuf);

    if (type1 != 'H' || type2 != 'C') {
        exit(1);
    }

    uint16_t num_nodes = 2 * num_leaves - 1;
    Node *stack[64];
    int top = -1;

    for (int i = 0; i < num_nodes; ++i) {
        uint8_t bit = bit_read_bit(inbuf);
        Node *node;

        if (bit == 1) {
            uint8_t symbol = bit_read_uint8(inbuf);
            node = node_create(symbol, 0);
        } else {
            node = node_create(0, 0);
            node->right = stack_pop(stack, &top);
            node->left = stack_pop(stack, &top);
        }

        stack_push(stack, &top, node);
    }

    Node *code_tree = stack_pop(stack, &top);

    for (uint32_t i = 0; i < filesize; ++i) {
        Node *node = code_tree;

        while (1) {
            uint8_t bit = bit_read_bit(inbuf);

            if (bit == 0) {
                node = node->left;
            } else {
                node = node->right;
            }

            if (node->left == NULL && node->right == NULL) {
                fwrite(&(node->symbol), sizeof(uint8_t), 1, fout);
                break;
            }
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
        return 1;
    }

    FILE *output_fp = fopen(output_file, "wb");
    if (output_fp == NULL) {
        fclose(input_fp);
        return 1;
    }

    BitReader *inbuf = bit_read_open(input_file);
    if (inbuf == NULL) {
        fclose(input_fp);
        fclose(output_fp);
        return 1;
    }

    dehuff_decompress_file(output_fp, inbuf);

    bit_read_close(&inbuf);
    fclose(input_fp);
    fclose(output_fp);

    return 0;
}
