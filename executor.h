#pragma once

#define EXEC_BACKGROUND 1 << 0
#define EXEC_SKIP       1 << 1

#define EXEC_RELATION_NONE            0
#define EXEC_RELATION_PIPE            1 << 0
#define EXEC_RELATION_REDIRECT_WRITE  1 << 1
#define EXEC_RELATION_REDIRECT_APPEND 1 << 2


typedef struct s_exec_node exec_node;
struct s_exec_node {
    char** tokens;
    exec_node* node;

    // TODO [$605c9fb87a37180656eec417]: Rename exec_node->relation to flags
    //       Also rename EXEC_RELATION_* to EXEC_* and continue from previous EXEC_* bits
    //       So EXEC_PIPE is 1 << 2 and so on
    int relation;
};

typedef struct {
    exec_node* node;
    unsigned int flags;
    unsigned int lineno;
} exec_context;

void execute(exec_context* ctx);
void free_exec_context(exec_context* ctx);