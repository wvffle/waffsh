#include "read.h"
#include "parser.h"
#include "constants.h"
#include "executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

void loop () {
    do {
        fprintf(stdout, "%s", PROMPT);

        char* line = read_line();
        exec_context* ctx = parse(line);
        execute(ctx);

        free(line);
        free_exec_context(ctx);
    } while (1);
}

int main(int argc, char **argv) {
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(argv[0], LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    loop();

    closelog();
    return EXIT_SUCCESS;
}
