#include "read.h"

#include <stdlib.h>
#include <syslog.h>
#include <stdio.h>

char* read_line () {
    char* line = NULL;
    size_t size = 0;

    if (getline(&line, &size, stdin) == -1) {
        // NOTE: Reached EOF, we can terminate successfully
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        }

        syslog(LOG_ERR, "read_line can't read the line.");
        exit(EXIT_FAILURE);
    }


    return line;
}
