#include "executor.h"
#include "utils.h"
#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>

void _close_all(int length, int* fds, int ffd) {
    for (int i = 0; i < length * 2; ++i) {
        close(*(fds + i));
    }

    if (ffd > 0) {
        close(ffd);
    }
}

void _execute_node (exec_node* node, exec_context* ctx) {
    if (node->flags & EXEC_BUILTIN) {
        // NOTE: This is for more difficult builtins that take stdin and write to stdout
        //       cd and exit won't work there
        if (!exec_builtin(node->tokens)) {
            // TODO [#25]: Handle error
        }
    }

    if(execvp(node->tokens[0], node->tokens) == -1) {
        syslog(LOG_ERR, "error when executing command [%-16s] %s on line %d", node->tokens[0], strerror(errno), ctx->lineno);
        fprintf(stderr, "line %d: %s: %s\n", ctx->lineno, node->tokens[0], strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void execute (exec_context* ctx) {
    if (ctx->flags & EXEC_SKIP) {
        return;
    }

    if (ctx->node->flags & EXEC_BUILTIN) {
        // NOTE: This takes place in main thread, handles cd and exit
        if (!exec_builtin(ctx->node->tokens)) {
            // TODO [#26]: Handle error
        }
        return;
    }

    int ffd = -2;
    int length = 1;
    exec_node* node = ctx->node;
    for (exec_node* c = ctx->node; c->node; c = c->node) {
        if (c->flags & (EXEC_REDIRECT_WRITE | EXEC_REDIRECT_APPEND)) {
            syslog(LOG_NOTICE, "Opening file: %s", c->node->tokens[0]);
            ffd = uopen(
                    // TODO [#6]: Get realpath
                    c->node->tokens[0],
                    c->flags & EXEC_REDIRECT_APPEND
                        ? O_WRONLY | O_CREAT | O_APPEND
                        : O_WRONLY | O_CREAT | O_TRUNC
            );

            if (ffd == -1) {
                perror(strerror(errno));
            }

            // NOTE: We can expect only
            //       one following token
            break;
        }

        ++length;
    }

    int fds[2 * length];
    memset(fds, -1, 2 * length);

    int pids[length];
    memset(pids, -1, length);

    for (int i = 0; i < length; ++i) {
        upipe(fds + i * 2);
    }

    pid_t pid = ufork("error forking child process.");
    if (pid == 0) {
        for (int i = 0; i < length; ++i) {
            pid_t pid = ufork("error forking child process.");
            if (pid == 0) {
                if (node->flags && (node->flags & (EXEC_REDIRECT_APPEND | EXEC_REDIRECT_WRITE)) == 0) {
                    syslog(LOG_NOTICE, "[%-16s] %d %d", node->tokens[0], i * 2 + 1, 1);
                    dup2(fds[i * 2 + 1], 1);
                }

                if ((i != 0 && node->flags == EXEC_PIPE) || i == length - 1) {
                    syslog(LOG_NOTICE, "[%-16s] %d %d", node->tokens[0], i * 2 - 2, 0);
                    dup2(fds[i * 2 - 2], 0);
                }

                if (node->flags & (EXEC_REDIRECT_APPEND | EXEC_REDIRECT_WRITE)) {
                    syslog(LOG_NOTICE, "[%-16s] redirect to %s (%d)", node->tokens[0], node->node->tokens[0], ffd);
                    dup2(ffd, 1);
                }

                _close_all(length, fds, ffd);
                _execute_node(node, ctx);
            }

            syslog(LOG_NOTICE, "[%-16s] pid: %d", node->tokens[0], pid);
            pids[i] = pid;
            node = node->node;
        }

        _close_all(length, fds, ffd);
        for (int i = 0; i < length; ++i) {
            syslog(LOG_NOTICE, "[%-16s] await pid: %d", "", pids[i]);
            await_pid(pids[i]);
            syslog(LOG_NOTICE, "[%-16s] await pid: %d done", "", pids[i]);
        }

        exit(EXIT_SUCCESS);
    }

    _close_all(length, fds, ffd);

    // NOTE: Don't wait for pid to finish
    //       when EXEC_BACKGROUND is set
    if (ctx->flags & EXEC_BACKGROUND) {
        return;
    }

    syslog(LOG_NOTICE, "await main pid: %d", pid);
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
