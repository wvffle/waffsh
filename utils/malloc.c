#include "malloc.h"

#include <syslog.h>
#include <stdlib.h>
#include <malloc.h>

void* umalloc (int size, char* error) {
    void* res = malloc(size);

    if (!res) {
        syslog(LOG_ERR, "%s", error);
        exit(EXIT_FAILURE);
    }

    return res;
}

void* urealloc (void* src, int size, char* error) {
    void* res = realloc(src, size);

    if (!res) {
        syslog(LOG_ERR, "%s", error);
        exit(EXIT_FAILURE);
    }

    return res;
}