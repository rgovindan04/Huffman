#include "pq.h"

#include "node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct ListElement ListElement;

struct ListElement {
    Node *tree;
    ListElement *next;
};

struct PriorityQueue {
    ListElement *list;
};

PriorityQueue *pq_create(void) {
    PriorityQueue *newPQ = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (newPQ == NULL) {
        return NULL; // Allocation error
    }

    // Initialize list to NULL
    newPQ->list = NULL;

    return newPQ;
}

void pq_free(PriorityQueue **q) {
    if (*q != NULL) {
        free(*q);
        *q = NULL;
    }
}

bool pq_is_empty(PriorityQueue *q) {
    return (q == NULL || q->list == NULL);
}

bool pq_size_is_1(PriorityQueue *q) {
    if (q == NULL || q->list == NULL) {
        return false; // Empty queue or NULL pointer
    }

    // Check if the queue contains a single element
    return (q->list != NULL && q->list->next == NULL);
}

bool pq_less_than(ListElement *e1, ListElement *e2) {
    if (e1->tree->weight < e2->tree->weight) {
        return true;
    } else if (e1->tree->weight == e2->tree->weight) {
        return (e1->tree->symbol < e2->tree->symbol);
    }

    return false;
}

void enqueue(PriorityQueue *q, Node *tree) {
    ListElement *new_element = (ListElement *) malloc(sizeof(ListElement));
    if (new_element == NULL) {
        exit(1);
    }

    new_element->tree = tree;
    new_element->next = NULL;

    if (q->list == NULL || pq_less_than(new_element, q->list)) {
        // New element becomes the new first element of the queue
        new_element->next = q->list;
        q->list = new_element;
    } else {
        ListElement *current = q->list;
        while (current->next != NULL && !pq_less_than(new_element, current->next)) {
            current = current->next;
        }
        new_element->next = current->next;
        current->next = new_element;
    }
}

Node *dequeue(PriorityQueue *q) {
    if (q == NULL || q->list == NULL) {
        // Queue is empty, report fatal error
        exit(EXIT_FAILURE);
    }

    ListElement *temp = q->list;
    Node *dequeued_node = temp->tree;
    q->list = temp->next;
    free(temp);

    return dequeued_node;
}

void pq_print(PriorityQueue *q) {
    assert(q != NULL);
    ListElement *e = q->list;
    int position = 1;
    while (e != NULL) {
        if (position++ == 1) {
            printf("=============================================\n");
        } else {
            printf("---------------------------------------------\n");
        }
        node_print_tree(e->tree);
        e = e->next;
    }
    printf("=============================================\n");
}
