#include "parser.h"
#include "constants.h"
#include <string.h>
#include <stdlib.h>
#include <syslog.h>

char** parse(char* line) {
    unsigned int size = PARSER_TOKEN_BUFFER_SIZE;
    char** tokens = malloc(sizeof(char*) * size);
    if (!tokens) {
        syslog(LOG_ERR, "parser allocation error.");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    const char* delims = PARSER_TOKEN_DELIMETERS;

    // NOTE: tokenizer modified from previous university project:
    //       https://github.com/wvffle/waffy/blob/studies/src/utils.c#L59
    for (char* token = strtok(line, delims); token != NULL; token = strtok(NULL, delims)) {
        tokens[i] = token;

        if (++i >= size) {
            size += PARSER_TOKEN_BUFFER_SIZE;
            tokens = realloc(tokens, sizeof(char*) * size);

            if (!tokens) {
                syslog(LOG_ERR, "parser reallocation error.");
                exit(EXIT_FAILURE);
            }
        }
    }

    // NOTE: Need a way to check the end of array
    //       as we do not know the size
    tokens[i] = NULL;

    return tokens;
}
