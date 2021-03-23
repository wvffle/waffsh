#include "read.h"
#include "constants.h"
#include "utils/malloc.h"

#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

char read_char (int fd) {
    int size = READER_FILE_BUFFER_SIZE;

    // NOTE: We're using static here, so we're
    //       limited to only one fd
    static char buffer[READER_FILE_BUFFER_SIZE];
    static size_t i = 0;

    if (i == 0 || i > size) {
        int res = read(fd, buffer, size);
        if (res == -1) {
            syslog(LOG_ERR, "reader error: %s", strerror(errno));
            fprintf(stderr, "reader error: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (res == 0) {
            return EOF;
        }
    }

    return *(buffer + i++);
}

char* read_line (int fd) {
    ssize_t size = READER_LINE_BUFFER_SIZE;
    char* line = umalloc(sizeof(char) * size, "reader allocation error.");
    memset(line, '\0', size);

    int i = 0;
    while (1) {
        char c = read_char(fd);

        if (c == EOF || c == '\0') {
            return line;
        }

        if (c == '\n') {
            break;
        }

        line[i] = c;
        if (++i >= size) {
            size *= 2;
            line = urealloc(line, sizeof(char) * size, "reader reallocation error.");
            memset(line + i, '\0', size / 2);
        }
    }

    return line;
}
