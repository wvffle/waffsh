#include "builtins.h"

#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>

#define BUILTINS_LENGTH sizeof(builtin_names) / sizeof(char*)

int builtin_exit (char** tokens) {
    exit(tokens[1] == NULL ? 0 : atoi(tokens[1]));
}

int builtin_cd (char** tokens) {
    if (tokens[1] != NULL) {
        // TODO [#23]: Handle errors
        chdir(tokens[1]);
        return 1;
    }

    return 0;
}

char* builtin_names[] = {
        "cd",
        "exit"
};

int (*builtin_functions[]) (char**) = {
        &builtin_cd,
        &builtin_exit
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

    // TODO [#24]: Set `command not found` error
    return 0;
}