#pragma once

#include <stdio.h>
#include <stdarg.h>  // For va_list

// Function declarations
int safe_printf(const char *format, ...);
int safe_fprintf(FILE *stream, const char *format, ...);
void* safe_memcpy(void *dest, const void *src, size_t n);
int safe_snprintf(char *str, size_t size, const char *format, ...);

