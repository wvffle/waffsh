#include "history.h"
#include "utils.h"
#include "read.h"
#include "constants.h"

#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

history_node* _create_history_node (char* line) {
    history_node* node = umalloc(sizeof(history_node), "history allocation error.");
    node->next = NULL;
    node->prev = NULL;
    node->line = NULL;

    if (line != NULL) {
        node->line = umalloc(sizeof(char) * strlen(line), "history allocation error.");
        strcpy(node->line, line);
    }

    return node;
}

char* _get_history_file () {
    struct passwd* pw = getpwuid(getuid());
    char* history_file = umalloc(
            sizeof(char) * (strlen(pw->pw_dir) + strlen(HISTORY_FILE) + 2),
            "history fifo allocation error."
    );

    memset(history_file, '\0', strlen(pw->pw_dir) + strlen(HISTORY_FILE));
    strcat(history_file, pw->pw_dir);
    strcat(history_file, "/");
    strcat(history_file, HISTORY_FILE);
    return history_file;
}

history* get_history () {
    char* history_file = _get_history_file();
    int fd = uopen3(history_file, O_RDONLY | O_CREAT, UTILS_FILE_PERMS);
    free(history_file);

    history* history = umalloc(sizeof(history), "history allocation error.");
    history_node* node = _create_history_node(NULL);
    history_node* tmp = node;


    char* line;
    while ((line = read_line(fd)) != NULL) {
        history_node* next = _create_history_node(line);
        node->next = next;
        next->prev = node;
        node = next;
    }

    // NOTE: The temporary node is here to track
    //       the beginning of the history.
    history->node = tmp->next;
    if (tmp->next != NULL) {
        tmp->next->prev = NULL;
        tmp->next = NULL;
    }

    free(tmp);
    close(fd);

    return history;
}

void push_history (history* history, char* line) {
    history_node* start = history->node;
    history_node* end = start;

    int size = 0;
    while (end && end->next) {
        size++;
        end = end->next;
    }

    while (size-- >= HISTORY_LINES) {
        history_node* curr = history->node;
        history->node = curr->next;
        history->node->prev = NULL;
        curr->next = NULL;
        free(curr->line);
        free(curr);
    }

    if (end != NULL) {
        end->next = _create_history_node(line);
        end->next->prev = end;
    } else {
        history->node = _create_history_node(line);
    }
}

void save_history(history* history) {
    char* history_file = _get_history_file();
    int fd = uopen3(history_file, O_WRONLY | O_CREAT | O_TRUNC, UTILS_FILE_PERMS);
    free(history_file);

    history_node* curr = history->node;
    while (curr) {
        write(fd, curr->line, strlen(curr->line));
        write(fd, "\n", strlen("\n"));
        curr = curr->next;
    }

    close(fd);
    // TODO [#14]: Free history
}
