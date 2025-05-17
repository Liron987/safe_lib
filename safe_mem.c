#include "safe_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node 
{
    void *ptr;
    size_t size;
    int freed;
    struct Node *next;
} Node;

Node *alloc_list = NULL;

// Safe allocation of memory - zero initialized
void* safe_alloc(size_t size) 
{
    void *p = calloc(1, size);
    if (!p) return NULL;

    Node *n = malloc(sizeof(Node));
    if (!n) {
        free(p);  // clean up previously allocated memory
        return NULL;
    }
    
    n->ptr = p;
    n->size = size;
    n->freed = 0;
    n->next = alloc_list;
    alloc_list = n;

    return p;
}

// Check if memory has been freed
// Return values:
//  0 = valid and not freed
//  1 = was freed
// -1 = not tracked
int check_free(void *ptr) {
    Node *n = alloc_list;
    while (n) {
        if (n->ptr == ptr) {
            return n->freed ? 1 : 0;
        }
        n = n->next;
    }
    return -1;
}

// Safe realloc
void* safe_realloc(void *ptr, size_t size) 
{
    // NULL realloc = malloc
    if (!ptr) return safe_alloc(size);
    
    int status = check_free(ptr);
    if (status != 0) {  // either freed or untracked
        fprintf(stderr, "[safe_realloc] Pointer is %s.\n",
                status == 1 ? "already freed" : "not tracked");
        return NULL;
    }

    void *new_ptr = realloc(ptr, size);
    if (!new_ptr) return NULL;

    // Update the node with the new pointer and size
    Node *n = alloc_list;
    while (n) {
        if (n->ptr == ptr) {
            n->ptr = new_ptr;
            n->size = size;
            return new_ptr;
        }
        n = n->next;
    }

    // Shouldn't reach here if check_free worked correctly
    fprintf(stderr, "[safe_realloc] Internal error: pointer not found in list.\n");
    return NULL;
}

// Safe free
void safe_free(void *p) {
    Node *curr = alloc_list;
    Node *prev = NULL;

    while (curr) {
        if (curr->ptr == p) {
            if (curr->freed) {
                fprintf(stderr, "[safe_free] Double free detected!\n");
                return;
            }

            // Optional: Zero memory before freeing (security sensitive)
            memset(curr->ptr, 0, curr->size);

            free(curr->ptr);  // Free user data
            curr->freed = 1;   // Mark as freed
            curr->size = 0;    // Clear size (optional, not strictly necessary)

            // Unlink and free node from list
            if (prev) {
                prev->next = curr->next;
            } else {
                alloc_list = curr->next;  // Remove the head node if freeing the first item
            }

            free(curr);  // Free the node itself
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    fprintf(stderr, "[safe_free] Invalid or untracked free!\n");
}


// Returns the size of the allocated block, or:
// -1 if not tracked
// -2 if the block has already been freed
ssize_t safe_sizeof(void *ptr) {
    Node *n = alloc_list;
    while (n) {
        if (n->ptr == ptr) {
            if (n->freed) return -2;
            return (ssize_t)n->size;
        }
        n = n->next;
    }
    return -1;
}

// Safe strlen
static size_t safe_strlen(const char *str)
{
    if (!str) {
        safe_fprintf(stderr, "String is NULL in safe_strlen\n");
        return 0;
    }
    return strlen(str);
}
