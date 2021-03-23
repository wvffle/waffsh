#include "read.h"
#include "parser.h"
#include "constants.h"
#include "executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

void loop (FILE* fp) {
    do {
        if (fp == stdin) {
            fprintf(stdout, "%s", PROMPT);
        }

        char* line = read_line(fp);
        if (line == NULL) {
            // NOTE: Reached EOF, we can terminate successfully
            break;
        }

        exec_context* ctx = parse(line);
        execute(ctx);

        free(line);
        free_exec_context(ctx);
    } while (1);
}

int main(int argc, char **argv) {
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(argv[0], LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    FILE* fp = stdin;

    if (argc > 1) {
        char* path = realpath(argv[1], NULL);
        fp = fopen(path, "r");
        free(path);
    }

    loop(fp);

    if (fp != stdin) {
        fclose(fp);
    }

    closelog();
    return EXIT_SUCCESS;
}
