#include "builtins.h"
#include "history.h"
#include "constants.h"

#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define BUILTINS_LENGTH sizeof(builtin_names) / sizeof(char*)

int builtin_exit (char** tokens) {
    exit(tokens[1] == NULL ? 0 : atoi(tokens[1]));
}

int builtin_cd (char** tokens) {
    if (tokens[1] != NULL) {
        // TODO [$6073a857b98763060c4bc349]: Handle errors
        chdir(tokens[1]);
        return 1;
    }

    return 0;
}

int builtin_history (char** tokens) {
    if (cmd_history == NULL) {
        return 0;
    }

    fprintf(stdout, "\nHistory:\n");

    int i = 0;
    history_node* node = cmd_history->node;
    while (node && node->next) {
        fprintf(stdout, "%d: %s\n", ++i, node->line);
        node = node->next;
    }


    if (tokens[1] != NULL && strcmp(tokens[1], "--with-prompt") == 0) {
        fprintf(stdout, "%s", PROMPT);
        fflush(stdout);
    }

    return 1;
}

int builtin_pid (char** tokens) {
    fprintf(stdout, "%d\n", getpid());
    return 1;
}

char* builtin_names[] = {
        "cd",
        "pid",
        "exit",
        "history",
};

int (*builtin_functions[]) (char**) = {
        &builtin_cd,
        &builtin_pid,
        &builtin_exit,
        &builtin_history,
};

int is_builtin (char* token) {
    for (int i = 0; i < BUILTINS_LENGTH; ++i) {
        if (strcmp(token, builtin_names[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

int exec_builtin (char** tokens) {
    if (tokens == NULL || tokens[0] == NULL) {
        // NOTE: Maybe set `command not found` error?
        return 0;
    }

    for (int i = 0; i < BUILTINS_LENGTH; ++i) {
        if (strcmp(tokens[0], builtin_names[i]) == 0) {
            return (*builtin_functions[i])(tokens);
        }
    }

    // TODO [$6073a857b98763060c4bc34a]: Set `command not found` error
    return 0;
}