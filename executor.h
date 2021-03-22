#pragma once

#define EXEC_BACKGROUND 1 << 0

typedef struct {
    char** argv;
    unsigned int flags;
} exec_context;

void execute(exec_context* ctx);
void free_exec_context(exec_context* ctx);