#include "node.h"

#include <stdio.h>
#include <stdlib.h>

Node *node_create(uint8_t symbol, uint32_t weight) {
    // Allocate memory for a new Node
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode == NULL) {
        return NULL; // Allocation error
    }

    // Set the symbol and weight fields of the Node
    newNode->symbol = symbol;
    newNode->weight = weight;

    return newNode;
}

void node_free(Node **pnode) {
    if (*pnode != NULL) {
        // Free the left child of *pnode
        node_free(&((*pnode)->left));
        // Free the right child of *pnode
        node_free(&((*pnode)->right));
        // Free *pnode
        free(*pnode);
        // Set *pnode to NULL
        *pnode = NULL;
    }
}

void node_print_node(Node *tree, char ch, int indentation) {
    if (tree == NULL) {
        return;
    }
    node_print_node(tree->right, '/', indentation + 3);
    printf("%*cweight = %d", indentation + 1, ch, tree->weight);

    if (tree->left == NULL && tree->right == NULL) {
        if (' ' <= tree->symbol && tree->symbol <= '~') {
            printf(", symbol = '%c'", tree->symbol);
        } else {
            printf(", symbol = 0x%02x", tree->symbol);
        }
    }

    printf("\n");
    node_print_node(tree->left, '\\', indentation + 3);
}

void node_print_tree(Node *tree) {
    node_print_node(tree, '<', 2);
}
