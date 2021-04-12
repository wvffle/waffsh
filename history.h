#pragma once
#include <stddef.h>

typedef struct s_history_node history_node;
struct s_history_node {
    char* line;
    history_node* next;
    history_node* prev;
};

typedef struct {
    history_node* node;
} history;

history* get_history ();
void push_history (history* history, char* line);
void save_history(history* history);

extern history* cmd_history;