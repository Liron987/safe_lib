#include "safe_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    void *ptr;
    int freed;
    struct Node *next;
} Node;

Node *alloc_list = NULL;

// Safe malloc
void* safe_malloc(size_t size) {
    void *p = malloc(size);
    if (!p) return NULL;

    Node *n = malloc(sizeof(Node));
    n->ptr = p;
    n->freed = 0;
    n->next = alloc_list;
    alloc_list = n;

    return p;
}

// Safe realloc
void* safe_realloc(void *ptr, size_t size) {
    if (check_free(ptr) != 0) {
        fprintf(stderr, "[safe_realloc] Pointer is freed or invalid.\n");
        return NULL;
    }

    void *new_ptr = realloc(ptr, size);
    if (!new_ptr) return NULL;

    Node *n = alloc_list;
    while (n) {
        if (n->ptr == ptr) {
            n->ptr = new_ptr;
            return new_ptr;
        }
        n = n->next;
    }

    return new_ptr;
}

// Safe free
void safe_free(void *p) {
    Node *n = alloc_list;
    while (n) {
        if (n->ptr == p) {
            if (n->freed) {
                fprintf(stderr, "[safe_free] Double free detected!\n");
                return;
            }
            free(p);
            n->freed = 1;
            return;
        }
        n = n->next;
    }
    fprintf(stderr, "[safe_free] Invalid or untracked free!\n");
}

// Check if memory has been freed
int check_free(void *ptr) {
    Node *n = alloc_list;
    while (n) {
        if (n->ptr == ptr) {
            return n->freed ? 1 : 0;  // 1 if freed, 0 if still valid
        }
        n = n->next;
    }
    return -1;  // Not tracked
}
