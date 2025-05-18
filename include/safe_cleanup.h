#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    SAFE_RESOURCE_NONE = 0,
    SAFE_RESOURCE_FILE,
    SAFE_RESOURCE_MEM,
    SAFE_RESOURCE_CUSTOM
} SafeResourceType;


typedef struct {
    SafeResourceType type;
    void *handle;
    void (*custom_cleanup)(void *);
} SafeResource;


// Cleans up an array of resources
void safe_cleanup_all(SafeResource *resources, int count);

// Short macros for clean usage
#define R_FILE(f)   (SafeResource){ SAFE_RESOURCE_FILE, (void *)(f), NULL }
#define R_MEM(p)    (SafeResource){ SAFE_RESOURCE_MEM,  (void *)(p), NULL }
#define R_CUST(h,f) (SafeResource){ SAFE_RESOURCE_CUSTOM, (void *)(h), (f) }

#define CLEAN(...) \
    safe_cleanup_all((SafeResource[]){__VA_ARGS__, { SAFE_RESOURCE_NONE, NULL, NULL }}, \
                     sizeof((SafeResource[]){__VA_ARGS__, {0}})/sizeof(SafeResource) - 1)

