#include "read.h"
#include "constants.h"

#include <stdlib.h>
#include <syslog.h>
#include <stdio.h>

char* read_line () {
    unsigned int size = READLINE_BUFFER_SIZE;
    char* buf = malloc(sizeof(char) * size);
    if (buf == NULL) {
        syslog(LOG_ERR, "read_line allocation error.");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while (1) {
        // WARN: Thread unsafe
        char c = getchar_unlocked();

        if (c == EOF || c == '\n') {
            buf[i] = '\n';
            return buf;
        }

        buf[i] = c;
        if (++i >= size) {
            size += READLINE_BUFFER_SIZE;
            buf = realloc(buf, size);

            if (buf == NULL) {
                syslog(LOG_ERR, "read_line reallocation error.");
                exit(EXIT_FAILURE);
            }
        }
    }
}
