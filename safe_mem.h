#pragma once

#include <stddef.h>  // For size_t

// Function declarations
void* safe_malloc(size_t size);
void* safe_realloc(void *ptr, size_t size);
void safe_free(void *ptr);

// Memory tracking functions
int check_free(void *ptr);

