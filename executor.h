#pragma once

#define EXEC_NONE            0
#define EXEC_BACKGROUND      1 << 0
#define EXEC_SKIP            1 << 1
#define EXEC_PIPE            1 << 2
#define EXEC_REDIRECT_WRITE  1 << 3
#define EXEC_REDIRECT_APPEND 1 << 4
#define EXEC_BUILTIN         1 << 5

typedef struct s_exec_node exec_node;
struct s_exec_node {
    char** tokens;
    exec_node* node;
    int flags;
};

typedef struct {
    exec_node* node;
    unsigned int flags;
    unsigned int lineno;
} exec_context;

void execute(exec_context* ctx);
void free_exec_context(exec_context* ctx);