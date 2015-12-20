#include <stdio.h>
#include <stdlib.h>

#include "list.h"

node_t *append(node_t **node, void *val) {
    node_t *this = *node,
           *next = malloc(sizeof(node_t));
    next->val = NULL;
    next->next = NULL;
    this->val = val;
    this->next = next;
    *node = next;
    return this;
}

void free_list(node_t *front, void(*f)(void*)) {
    node_t *prev = NULL;
    node_t *iter = front;
    while (iter) {
        prev = iter;
        iter = iter->next;
        f(prev->val);
        free(prev);
    }
}
