#ifndef LIST_H
#define LIST_H

struct node {
    void *val;
    struct node *next;
};
typedef struct node node_t;

node_t *append(node_t**, void*);
void free_list(node_t*, void(*)(void*));

#endif
