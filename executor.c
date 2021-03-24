#include "executor.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

void _execute_node (exec_node* node, exec_context* ctx) {
    if(execvp(node->tokens[0], node->tokens) == -1) {
        syslog(LOG_ERR, "error when executing command %s: %s on line %d", node->tokens[0], strerror(errno), ctx->lineno);
        fprintf(stderr, "line %d: %s: %s\n", ctx->lineno, node->tokens[0], strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void execute (exec_context* ctx) {
    if (ctx->flags & EXEC_SKIP) {
        return;
    }

    int length = 0;
    exec_node* node = ctx->node;
    for (exec_node* c = node;c;c = c->node) {
        ++length;
    }

    int fds[2 * length];
    int pids[length];
    for (int i = 0; i < length; ++i) {
        upipe(fds + i * 2);
    }

    pid_t pid = ufork("error forking child process.");
    if (pid == 0) {
        for (int i = 0; i < length; ++i) {
            pid_t pid = ufork("error forking child process.");
            if (pid == 0) {
                if (node->relation != EXEC_RELATION_NONE) {
                    syslog(LOG_NOTICE, "%s: %d %d", node->tokens[0], i * 2 + 1, 1);
                    dup2(fds[i * 2 + 1], 1);
                }

                if ((i != 0 && node->relation == EXEC_RELATION_PIPE) || i == length - 1) {
                    syslog(LOG_NOTICE, "%s: %d %d", node->tokens[0], i * 2 - 2, 0);
                    dup2(fds[i * 2 - 2], 0);
                }

                for (int i = 0; i < length * 2; ++i) {
                    close(*(fds + i));
                }

                _execute_node(node, ctx);
            }

            pids[i] = pid;
            node = node->node;
        }
    }

    for (int i = 0; i < length * 2; ++i) {
        close(*(fds + i));
    }

    for (int i = 0; i < length; ++i) {
        await_pid(pids[i]);
    }

    // NOTE: Don't wait for pid to finish
    //       when EXEC_BACKGROUND is set
    if (ctx->flags & EXEC_BACKGROUND) {
        return;
    }

    await_pid(pid);
}

void free_exec_context (exec_context* ctx) {
    exec_node* node = ctx->node;
    while (node) {
        exec_node* next = node->node;
        if (node->tokens != NULL) {
            for (int i = 0; node->tokens[i] != NULL; ++i) {
                free(node->tokens[i]);
            }

            free(node->tokens);
        }

        free(node);
        node = next;
    }

    free(ctx);
}
