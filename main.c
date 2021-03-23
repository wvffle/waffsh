#include "read.h"
#include "parser.h"
#include "constants.h"
#include "executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void loop (int fd) {
    do {
        if (fd == STDIN_FILENO) {
            fprintf(stdout, "%s", PROMPT);
            fflush(stdout);
        }

        char* line = read_line(fd);
        if (line == NULL) {
            // NOTE: Reached EOF, we can terminate successfully
            break;
        }

        exec_context* ctx = parse(line);
        // execute(ctx);

        free(line);
        free_exec_context(ctx);
    } while (1);
}

int main(int argc, char **argv) {
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(argv[0], LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    int fd = STDIN_FILENO;
    if (argc > 1) {
        char* path = realpath(argv[1], NULL);
        fd = open(path, O_RDONLY);
        if (fd == -1) {
            syslog(LOG_ERR, "open error: %s", strerror(errno));
            fprintf(stderr, "open error: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        free(path);
    }

    loop(fd);

    if (fd != STDIN_FILENO) {
        close(fd);
    }

    closelog();
    return EXIT_SUCCESS;
}

