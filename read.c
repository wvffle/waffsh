#include "read.h"

#include <stdlib.h>
#include <syslog.h>
#include <stdio.h>

char* read_line (FILE* fp) {
    char* line = NULL;
    size_t size = 0;

    if (getline(&line, &size, fp) == -1) {
        if (feof(fp)) {
            return NULL;
        }

        syslog(LOG_ERR, "read_line can't read the line.");
        exit(EXIT_FAILURE);
    }


    return line;
}
