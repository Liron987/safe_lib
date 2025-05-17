#pragma once

#include <stddef.h>  // For size_t
#include <sys/types.h>  // for ssize_t

// Safe memory allocation functions
void* safe_malloc(size_t size);
void* safe_realloc(void *ptr, size_t size);
void  safe_free(void *ptr);

// Memory tracking utilities
int    check_free(void *ptr);      // 0 = not freed, 1 = freed, -1 = not tracked
ssize_t safe_sizeof(void *ptr);    // >=0 = size, -1 = not tracked, -2 = already freed
static size_t safe_strlen(const char *str);     // NULL checked strlen
