#include "executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

void execute (exec_context* ctx) {
    if (ctx->flags & EXEC_SKIP) {
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        syslog(LOG_ERR, "error when forking child process.");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // TODO: Rework
//        if(execvp(ctx->argv[0], ctx->argv) == -1) {
//            syslog(LOG_ERR, "error when executing command %s: %s on line %d", ctx->argv[0], strerror(errno), ctx->lineno);
//            fprintf(stderr, "line %d: %s: %s\n", ctx->lineno, ctx->argv[0], strerror(errno));
//            exit(EXIT_FAILURE);
//        }

        // NOTE: execvp replaced the forked child process
        //       so there is no need to exit
    }

    // NOTE: Don't wait for pid to finish
    //       when EXEC_BACKGROUND is set
    if (ctx->flags & EXEC_BACKGROUND) {
        return;
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

void free_exec_context (exec_context* ctx) {
    exec_node* node = ctx->node;
    while (node) {
        exec_node* next = node->node;
        if (node->argv != NULL) {
            for (int i = 0; node->argv[i] != NULL; ++i) {
                free(node->argv[i]);
            }

            free(node->argv);
        }

        free(node);
        node = next;
    }

    free(ctx);
}
