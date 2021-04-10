#include "read.h"
#include "parser.h"
#include "constants.h"
#include "executor.h"
#include "utils.h"
#include "history.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <signal.h>

history* cmd_history = NULL;
void exit_hook () {
    if (cmd_history != NULL) {
        save_history(cmd_history);
    }
}

void sigquit_handler(int signum) {
    if (cmd_history == NULL) {
        return;
    }

    fprintf(stdout, "\nHistory:\n");

    int i = 0;
    history_node* node = cmd_history->node;
    while (node && node->next) {
        fprintf(stdout, "%d: %s\n", ++i, node->line);
        node = node->next;
    }

    fprintf(stdout, "%s", PROMPT);
}

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

        // NOTE: Only save history if we read from stdin
        //       and line does not start with ' ' or '#'
        if (fd == STDIN_FILENO && (line[0] != ' ' || (ctx->flags & EXEC_SKIP) == 0)) {
            push_history(cmd_history, line);
        }

        execute(ctx);

        free(line);
        free_exec_context(ctx);
    } while (1);
}

int main(int argc, char **argv) {
    // TODO [#16]: Handle atexit errors
    atexit(exit_hook);

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
    } else {
        cmd_history = get_history();

        // TODO [#27]: Handle signal errors
        signal(SIGQUIT, sigquit_handler);
    }

    loop(fd);

    if (fd != STDIN_FILENO) {
        close(fd);
    }

    closelog();
    return EXIT_SUCCESS;
}

