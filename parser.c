#include "parser.h"
#include "constants.h"
#include "executor.h"
#include "utils/malloc.h"

#include <string.h>
#include <stdlib.h>
#include <syslog.h>

void _push_token(exec_node* node, char* line, int i, int start, int end) {
    char* token = umalloc(sizeof(char) * (end - start), "tokenizer allocation error.");
    strncpy(token, line + sizeof(char) * start, end - start);
    node->argv[i] = token;

    // NOTE: Dynamic check if next size would be bigger
    // TODO: Double buffer size to avoid O(n^2) complexity as much as possible
    if ((i + 1) / PARSER_TOKEN_BUFFER_SIZE > i / PARSER_TOKEN_BUFFER_SIZE) {
        int size = ((i + 1) / PARSER_TOKEN_BUFFER_SIZE + 1) * PARSER_TOKEN_BUFFER_SIZE;
        node->argv = urealloc(
                node->argv,
                sizeof(char*) * size,
                "tokenizer reallocation error."
        );

        for (int j = 0; j < PARSER_TOKEN_BUFFER_SIZE; ++j) {
            node->argv[size - 1 - j] = NULL;
        }
    }
}

exec_node* _create_node () {
    exec_node* node = umalloc(sizeof(exec_node), "parser allocation error.");
    node->argv = umalloc(sizeof(char*) * PARSER_TOKEN_BUFFER_SIZE, "parser allocation error.");
    for (int i = 0; i < PARSER_TOKEN_BUFFER_SIZE; ++i) {
        node->argv[i] = NULL;
    }
    node->node = NULL;
    node->relation = 0;
    return node;
}

// NOTE: Supports:
//       - cmd1 | cmd2
//       - cmd2 > cmd2
//       - cmd2 >> cmd2
//       - cmd1 &

// TODO: Add support for escaping spaces
// TODO: Add support for escaping \&
exec_context* parse(char* line) {
    static int lineno = 0;
    exec_context* ctx = umalloc(sizeof(exec_context), "parser allocation error.");
    ctx->node = NULL;
    ctx->flags = 0;
    ctx->lineno = ++lineno;

    exec_node* node = _create_node();
    ctx->node = node;

    unsigned int token_idx = 0;

    char lastc = '\0';
    int last_token_idx = 0;
    // NOTE: normally we'd use strtok as it's much easier,
    //       though we're unsure if we can match
    //       multi character delimeters
    for (int i = 0; i < strlen(line) + 1; ++i) {
        char c = line[i];

        if (lastc == '\0' && c != '#' && strchr(PARSER_TOKEN_DELIMETERS, c) != NULL) {
            last_token_idx = i + 1;
            continue;
        }

        if (lastc == '>') {
            if (lastc != '\0') {
                _push_token(node, line, token_idx++, last_token_idx, i - 1);
                last_token_idx = i + 1;
                lastc = '\0';
            }

            exec_node* next_node = _create_node();

            node->node = next_node;
            node->argv[token_idx] = NULL;
            node->relation = c == '>'
                    ? EXEC_RELATION_REDIRECT_APPEND
                    : EXEC_RELATION_REDIRECT_WRITE;

            token_idx = 0;
            node = next_node;
            continue;
        }

        if (c == '>') {
            lastc = c;
            continue;
        }

        if (c == '|') {
            if (lastc != '\0') {
                _push_token(node, line, token_idx++, last_token_idx, i);
                last_token_idx = i + 1;
                lastc = '\0';
            }

            exec_node* next_node = _create_node();

            node->node = next_node;
            node->argv[token_idx] = NULL;
            node->relation = EXEC_RELATION_PIPE;

            token_idx = 0;
            node = next_node;
            continue;
        }

        if (strchr(PARSER_TOKEN_DELIMETERS, c) != NULL || c == '\0') {
            if (lastc != '\0') {
                _push_token(node, line, token_idx++, last_token_idx, i);
                lastc = '\0';
                last_token_idx = i + 1;
            }

            // NOTE: When parser receives line with unescaped & character,
            //       it ignores everything after & character and executes
            //       left hand side in background.
            if (c == '&') {
                ctx->flags |= EXEC_BACKGROUND;
                node->argv[token_idx] = NULL;
                break;
            }

            if (c == '#') {
                node->argv[token_idx] = NULL;
                break;
            }

            continue;
        }

        lastc = c;
    }

    if (ctx->node->argv[0] == NULL) {
        ctx->flags |= EXEC_SKIP;
    }

    return ctx;
}