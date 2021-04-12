#include "parser.h"
#include "constants.h"
#include "executor.h"
#include "utils.h"
#include "builtins.h"

#include <string.h>
#include <stdlib.h>
#include <syslog.h>

void _push_token(exec_node* node, char* line, int i, int start, int end) {
    char* token = umalloc(sizeof(char) * (end - start + 1), "tokenizer allocation error.");
    strncpy(token, line + sizeof(char) * start, end - start);
    token[end - start] = '\0';
    node->tokens[i] = token;

    if (i == 0 && is_builtin(token)) {
        node->flags |= EXEC_BUILTIN;
    }

    // NOTE: Dynamic check if next size would be bigger
    // TODO [#18]: Double buffer size to avoid O(n^2) complexity as much as possible
    if ((i + 1) / PARSER_TOKEN_BUFFER_SIZE > i / PARSER_TOKEN_BUFFER_SIZE) {
        int size = ((i + 1) / PARSER_TOKEN_BUFFER_SIZE + 1) * PARSER_TOKEN_BUFFER_SIZE;
        node->tokens = urealloc(
                node->tokens,
                sizeof(char*) * size,
                "tokenizer reallocation error."
        );

        for (int j = 0; j < PARSER_TOKEN_BUFFER_SIZE; ++j) {
            node->tokens[size - 1 - j] = NULL;
        }
    }
}

exec_node* _create_node () {
    exec_node* node = umalloc(sizeof(exec_node), "parser allocation error.");
    node->tokens = umalloc(sizeof(char*) * PARSER_TOKEN_BUFFER_SIZE, "parser allocation error.");
    for (int i = 0; i < PARSER_TOKEN_BUFFER_SIZE; ++i) {
        node->tokens[i] = NULL;
    }
    node->node = NULL;
    node->flags = 0;
    return node;
}

// NOTE: Supports:
//       - cmd1 | cmd2
//       - cmd2 > cmd2
//       - cmd2 >> cmd2
//       - cmd1 &

// TODO [#19]: Add support for escaping spaces
// TODO [#20]: Add support for escaping \&
// TODO [#21]: Add support for echo test > test.txt &
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
    int parser_flags = PARSER_FLAGS_NONE;
    // NOTE: normally we'd use strtok as it's much easier,
    //       though we're unsure if we can match
    //       multi character delimeters
    for (int i = 0; i < strlen(line) + 1; ++i) {
        char c = line[i];

        if (lastc == '\0' && c != '#' && strchr(PARSER_TOKEN_DELIMETERS, c) != NULL) {
            last_token_idx = i + 1;
            continue;
        }

        if (c == '>') {
            if (lastc != '\0') {
                _push_token(node, line, token_idx++, last_token_idx, i - 1);
                lastc = '\0';

                if (parser_flags & PARSER_FLAGS_STOP_AFTER_NEXT_TOKEN) {
                    break;
                }
            }

            last_token_idx = i + 1;

            exec_node* next_node = _create_node();

            node->node = next_node;
            node->tokens[token_idx] = NULL;
            node->flags = line[i + 1] == '>'
                             ? EXEC_REDIRECT_APPEND
                             : EXEC_REDIRECT_WRITE;

            token_idx = 0;
            node = next_node;

            // NOTE: When parser receives line with unescaped > or >> keyword
            //       it ignores everything after the next token
            parser_flags |= PARSER_FLAGS_STOP_AFTER_NEXT_TOKEN;

            if (line[i + 1] == '>') {
                ++i;
            }

            continue;
        }

        if (c == '|') {
            if (lastc != '\0') {
                _push_token(node, line, token_idx++, last_token_idx, i);
                lastc = '\0';

                if (parser_flags & PARSER_FLAGS_STOP_AFTER_NEXT_TOKEN) {
                    break;
                }
            }

            last_token_idx = i + 1;

            exec_node* next_node = _create_node();

            node->node = next_node;
            node->tokens[token_idx] = NULL;
            node->flags = EXEC_PIPE;

            token_idx = 0;
            node = next_node;
            continue;
        }

        if (strchr(PARSER_TOKEN_DELIMETERS, c) != NULL || c == '\0') {
            if (lastc != '\0') {
                _push_token(node, line, token_idx++, last_token_idx, i);
                lastc = '\0';
                last_token_idx = i + 1;

                if (parser_flags & PARSER_FLAGS_STOP_AFTER_NEXT_TOKEN) {
                    break;
                }
            }

            // NOTE: When parser receives line with unescaped & character,
            //       it ignores everything after & character and executes
            //       left hand side in background.
            if (c == '&') {
                ctx->flags |= EXEC_BACKGROUND;
                node->tokens[token_idx] = NULL;
                break;
            }

            if (c == '#') {
                node->tokens[token_idx] = NULL;
                break;
            }

            continue;
        }

        lastc = c;
    }

    if (ctx->node->tokens[0] == NULL) {
        ctx->flags |= EXEC_SKIP;
    }

    return ctx;
}