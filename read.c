#include "read.h"
#include "constants.h"

#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

char* read_line (int fd) {
    ssize_t size = READER_LINE_BUFFER_SIZE;
    char* line = malloc(sizeof(char) * size);
    memset(line, '\0', size);
    if (line == NULL) {
        syslog(LOG_ERR, "reader allocation error.");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    char c[1];
    do {
        int res = read(fd, c, 1);
        if (res == -1) {
            syslog(LOG_ERR, "reader error: %s", strerror(errno));
            fprintf(stderr, "reader error: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (res == 0) {
            free(line);
            return NULL;
        }

        if (c[0] == '\n') {
            break;
        }

        line[i] = c[0];
        if (++i >= size) {
            size += READER_LINE_BUFFER_SIZE;
            line = realloc(line, sizeof(char) * size);
            memset(line + i, '\0', READER_LINE_BUFFER_SIZE);
            if (line == NULL) {
                syslog(LOG_ERR, "reader reallocation error.");
                exit(EXIT_FAILURE);
            }
        }
    } while (1);

    return line;
}
