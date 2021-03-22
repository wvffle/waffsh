#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "read.h"
#include "constants.h"



void loop () {
    do {
        fprintf(stdout, "%s", PROMPT);

        char* line = read_line();

        free(line);
    } while (1);
}

int main(int argc, char **argv) {
    setlogmask(LOG_UPTO(LOG_NOTICE));
    openlog(argv[0], LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    loop();

    closelog();
    return EXIT_SUCCESS;
}
