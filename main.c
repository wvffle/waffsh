#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "read.h"
#include "parser.h"
#include "constants.h"
#include "executor.h"



void loop () {
    do {
        fprintf(stdout, "%s", PROMPT);

        char* line = read_line();
        char** argv = parse(line);

        execute(argv);

        free(line);
        free(argv);
    } while (1);
}

int main(int argc, char **argv) {
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(argv[0], LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    loop();

    closelog();
    return EXIT_SUCCESS;
}
