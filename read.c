#include "read.h"
#include "constants.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int* iterators;
char** buffers;

//void init_reader () {
//    for (int i = 0; i < N; ++i) {
//        iterators[i] = -1;
//        buffers[i] = NULL;
//    }
//}

char read_char (int fd) {
    int size = fd == 0 ? 1 : READER_FILE_BUFFER_SIZE;

    static int max_fd = 0;
    if (max_fd == 0) {
        max_fd = fd + 1;
        buffers = umalloc(sizeof(char*) * max_fd, "reader allocation error.");
        iterators = umalloc(sizeof(int) * max_fd, "reader allocation error.");
        for (int i = 0; i < max_fd; ++i) {
            buffers[i] = NULL;
            iterators[i] = -1;
        }
    }

    if (fd + 1 > max_fd) {
        buffers = urealloc(buffers, fd + 1, "reader reallocation error.");
        iterators = urealloc(iterators, fd + 1, "reader reallocation error.");

        for (int i = 0; i < fd + 1; ++i) {
            buffers[i] = NULL;
            iterators[i] = -1;
        }

        max_fd = fd + 1;
    }

    if (iterators[fd] != -1 && buffers[fd] == NULL) {
        return EOF;
    }

    if (iterators[fd] == -1) {
        iterators[fd] = 0;

        // NOTE: When reading from STDIN we need to
        //       limit ourselves to one character only.
        //       Otherwise we'd read much past '\n'.
        //       Another solution is to clear the buffer
        //       after '\n' occurs though there is
        //       no need to suffer memory for performance
        //       in interactive mode.
        if (fd == 0) {
            buffers[fd] = malloc(sizeof(char) * 2);
            memset(buffers[fd], '\0', 2);
        } else {
            buffers[fd] = malloc(sizeof(char) * READER_FILE_BUFFER_SIZE);
            memset(buffers[fd], '\0', READER_FILE_BUFFER_SIZE);
        }
    }

    char* buffer = buffers[fd];
    size_t i = iterators[fd]++;

    if (i == 0 || i >= size) {
        if (uread(fd, buffer, size) == 0) {
            free(buffers[fd]);
            return EOF;
        }

        i = 0;
    }

    char c = *(buffer + i);
    if (c == '\0' && (*(buffer + i - 2) == '\0') && i > 2) {
        free(buffers[fd]);
        return EOF;
    }

    // NOTE: Replaced by VIM-like normal mode
    // // NOTE: Checking for arrowup/arrowdown skips the escape codes
    // if (fd == 0 && c == '\033') {
    //     char nextc = read_char(fd);
    //     if (nextc == 'A' || nextc == 'B') {
    //         // Need a way to return the arrowup/arrowdown keys
    //         // Maybe some unused escape code?
    //     }
    //
    //     return read_char(fd);
    // }

    return c;
}

char* read_line (int fd) {
    ssize_t size = READER_LINE_BUFFER_SIZE;
    char* line = umalloc(sizeof(char) * size, "reader allocation error.");
    memset(line, '\0', size);

    int i = 0;
    while (1) {
        char c = read_char(fd);

        if (c == EOF) {
            free(line);
            return NULL;
        }

        if (c == '\n' || c == '\0') {
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
