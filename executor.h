#pragma once

#define EXEC_BACKGROUND 1 << 0
#define EXEC_SKIP       1 << 1

typedef struct {
    char** argv;
    unsigned int flags;
    unsigned int lineno;
} exec_context;

void execute(exec_context* ctx);
void free_exec_context(exec_context* ctx);