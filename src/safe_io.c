#include "safe_io.h"
#include "safe_mem.h"  // Include memory management for check_free
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

void* safe_memcpy(void *dest, const void *src, size_t n) {
    if (n <= 0) {
        fprintf(stderr, "[safe_memcpy] Copy size must be greater than zero.\n");
        return NULL;
    }
    if (src == NULL) {
        fprintf(stderr, "[safe_memcpy] Source pointer is NULL.\n");
        return NULL;
    }
    if (dest == NULL) {
        fprintf(stderr, "[safe_memcpy] Destination pointer is NULL.\n");
        return NULL;
    }

    return memcpy(dest, src, n);
}



// Safe printf with pointer check
int safe_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Pointer scan: check for potential UAFs
    const char *p = format;
    while (*p) {
        if (*p == '%') {
            p++;
            while (*p && strchr("0123456789.-+ #", *p)) p++;  // Skip flags/width/precision
            if (*p == 'l' || *p == 'h') {
                p++;
                if (*p == 'l') p++;  // Support "ll"
            }

            switch (*p) {
                case 's': {
                    char *arg = va_arg(args, char *);
                    if (check_free(arg) == 1) {
                        fprintf(stderr, "[safe_printf] Argument for %%s is freed or invalid.\n");
                        va_end(args);
                        return -1;
                    }
                    break;
                }
                case 'p': {
                    void *arg = va_arg(args, void *);
                    if (check_free(arg) == 1) {
                        fprintf(stderr, "[safe_printf] Argument for %%p is freed.\n");
                        va_end(args);
                        return -1;
                    }
                    break;
                }
                default:
                    va_arg(args, void *);  // Consume non-pointer args
                    break;
            }
        }
        p++;
    }

    va_end(args);

    // Re-iterate and actually print now
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);

    return result;
}

// Safe fprintf with pointer check
int safe_fprintf(FILE *stream, const char *format, ...) {
    if (!stream) {
        fprintf(stderr, "[safe_fprintf] NULL file stream.\n");
        return -1;
    }

    if (check_free((void *)format) == 1) {
        fprintf(stderr, "[safe_fprintf] Format string is freed or invalid.\n");
        return -1;
    }

    va_list args;
    va_start(args, format);

    // Pointer scan: check for potential UAFs
    const char *p = format;
    while (*p) {
        if (*p == '%') {
            p++;
            while (*p && strchr("0123456789.-+ #", *p)) p++;  // Skip flags/width/precision
            if (*p == 'l' || *p == 'h') {
                p++;
                if (*p == 'l') p++;  // Support "ll"
            }

            switch (*p) {
                case 's': {
                    char *arg = va_arg(args, char *);
                    if (check_free(arg) == 1) {
                        fprintf(stderr, "[safe_fprintf] %%s arg is freed or invalid.\n");
                        va_end(args);
                        return -1;
                    }
                    break;
                }
                case 'p': {
                    void *arg = va_arg(args, void *);
                    if (check_free(arg) == 1) {
                        fprintf(stderr, "[safe_fprintf] %%p arg is freed.\n");
                        va_end(args);
                        return -1;
                    }
                    break;
                }
                default:
                    va_arg(args, void *);  // Consume non-pointer args
                    break;
            }
        }
        p++;
    }

    va_end(args);

    // Re-iterate and actually print now
    va_start(args, format);
    int result = vfprintf(stream, format, args);
    va_end(args);

    return result;
}

// safe_snprintf with guard against both buffer overflows and use-after-free errors in format arguments.
// it will fail fast (exit) upon UAF, so we don't run with garbage values (potential security risk)
int safe_snprintf(char *str, size_t size, const char *format, ...) {
    if (check_free(str) == 1) {
        fprintf(stderr, "[safe_snprintf] Destination buffer is freed or invalid.\n");
        exit(EXIT_FAILURE);  // Exit immediately upon detecting UAF
    }

    if (check_free((void *)format) == 1) {
        fprintf(stderr, "[safe_snprintf] Format string is freed or invalid.\n");
        exit(EXIT_FAILURE);  // Exit immediately upon detecting UAF
    }

    if (size == 0) {
        fprintf(stderr, "[safe_snprintf] Warning: size is 0 — nothing will be written.\n");
        return 0;
    }

    va_list args;
    va_start(args, format);

    // Scan format for UAF-prone arguments
    const char *p = format;
    va_list args_copy;
    va_copy(args_copy, args);

    while (*p) {
        if (*p == '%') {
            p++;
            while (*p && strchr("0123456789.-+ #", *p)) p++;  // skip flags
            if (*p == 'l' || *p == 'h') {
                p++;
                if (*p == 'l') p++;
            }

            switch (*p) {
                case 's': {
                    char *arg = va_arg(args_copy, char *);
                    if (check_free(arg) == 1) {
                        fprintf(stderr, "[safe_snprintf] %%s argument is freed or invalid.\n");
                        exit(EXIT_FAILURE);  // Exit on UAF
                    }
                    break;
                }
                case 'p': {
                    void *arg = va_arg(args_copy, void *);
                    if (check_free(arg) == 1) {
                        fprintf(stderr, "[safe_snprintf] %%p argument is freed.\n");
                        exit(EXIT_FAILURE);  // Exit on UAF
                    }
                    break;
                }
                default:
                    va_arg(args_copy, void *);  // Consume non-pointer arguments
                    break;
            }
        }
        p++;
    }
    va_end(args_copy);

    // Do the actual snprintf
    int written = vsnprintf(str, size, format, args);
    va_end(args);

    // Optional: warn if output was truncated
    if ((size > 0) && (written >= (int)size)) {
        fprintf(stderr, "[safe_snprintf] Warning: output truncated (written: %d, buffer: %zu)\n", written, size);
    }

    return written;
}
