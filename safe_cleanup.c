#include "safe_cleanup.h"

void safe_cleanup_all(SafeResource *resources, int count) {
    for (int i = 0; i < count; ++i) {
        SafeResource *res = &resources[i];
        if (!res->handle) continue;

        switch (res->type) {
            case SAFE_RESOURCE_FILE:
                fclose((FILE *)res->handle);
                break;

            case SAFE_RESOURCE_MEM:
                free(res->handle);
                break;

            case SAFE_RESOURCE_CUSTOM:
                if (res->custom_cleanup)
                    res->custom_cleanup(res->handle);
                break;

            default:
                break;
        }

        res->handle = NULL;
    }
}

/*
 * Example usage:
 * ***********************
#include "safe_cleanup.h"

int main(void) {
    FILE *f = fopen("config.txt", "r");
    char *tmp = malloc(128);

    if (!f || !tmp)
        CLEAN(R_FILE(f), R_MEM(tmp));

    // use f and tmp...

    CLEAN(R_FILE(f), R_MEM(tmp));
    return 0;
}
*/