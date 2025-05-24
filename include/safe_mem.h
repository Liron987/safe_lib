#pragma once

#include <stddef.h>  // For size_t
#include <sys/types.h>  // for ssize_t

// Safe memory allocation functions
void* safe_alloc(size_t size);
void* safe_realloc(void *ptr, size_t size);
void  safe_free(void *ptr);

// Memory tracking utilities
int    check_free(void *ptr);      // 0 = not freed, 1 = freed, -1 = not tracked
size_t safe_strlen(const char *str);     // NULL checked strlen
ssize_t safe_sizeof_impl(void *ptr, size_t hint);
#define safe_sizeof(x) safe_sizeof_impl((void *)(x), sizeof(x))

// Safe access
void *get_safe(void *arr, size_t index, ssize_t total_bytes, size_t elem_size);
void *set_safe(void *arr, size_t index, void *value, ssize_t total_bytes, size_t elem_size);

// Macros for user-friendly syntax
#define get(arr, index) \
    (*(typeof((arr)[0]) *)get_safe((arr), (index), safe_sizeof(arr), sizeof((arr)[0])))

#define set(arr, index, val) do { \
    typeof((arr)[0]) _val = (val); \
    set_safe((arr), (index), &_val, safe_sizeof(arr), sizeof((arr)[0])); \
} while (0)
