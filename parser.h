#pragma once
#include "executor.h"

#define PARSER_FLAGS_NONE                  0
#define PARSER_FLAGS_STOP_AFTER_NEXT_TOKEN 1 << 0

exec_context* parse(char* line);