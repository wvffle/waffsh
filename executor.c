#include "executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

void execute (char** argv) {
    pid_t pid = fork();
    if (pid == -1) {
        syslog(LOG_ERR, "error when forking child process.");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        if(execvp(argv[0], argv) == -1) {
            syslog(LOG_ERR, "error when executing command %s: %s", argv[0], strerror(errno));
            fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
            exit(EXIT_FAILURE);
        }

        // NOTE: execvp replaced the forked child process
        //       so there is no need to exit
    }

    int stat_loc;
    do {
        if (waitpid(pid, &stat_loc, WUNTRACED) == -1) {
            switch (errno) {
                case ECHILD:
                    syslog(LOG_ERR, "no member process that is a child of the calling process.");
                    break;
                case EINTR:
                    syslog(LOG_ERR, "the function was interrupted by a signal.");
                    break;
            }

            // NOTE: Should we exit with failure here?
        }
    } while (!WIFEXITED(stat_loc) && !WIFSIGNALED(stat_loc));
}