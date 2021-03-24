#include "utils.h"

#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

void* umalloc (int size, char* error) {
    void* res = malloc(size);

    if (!res) {
        syslog(LOG_ERR, "%s", error);
        exit(EXIT_FAILURE);
    }

    return res;
}

void* urealloc (void* src, int size, char* error) {
    void* res = realloc(src, size);

    if (!res) {
        syslog(LOG_ERR, "%s", error);
        exit(EXIT_FAILURE);
    }

    return res;
}

pid_t ufork (char* error) {
    pid_t pid = fork();
    if (pid == -1) {
        syslog(LOG_ERR, "%s", error);
        exit(EXIT_FAILURE);
    }

    return pid;
}

void upipe (int* fd) {
    if (pipe(fd) == -1) {
        switch (errno) {
            case EMFILE:
                syslog(LOG_ERR, "Too many file descriptors are in use by the process. ");
                break;
            case ENFILE:
                syslog(LOG_ERR, "The system limit on the total number of open files has been reached.");
                break;
        }
        exit(EXIT_FAILURE);
    }
}

void await_pid (pid_t pid) {
    if (pid == -1) {
        return;
    }

    int stat_loc = -2;
    do {
        if (waitpid(pid, &stat_loc, WUNTRACED) == -1) {
            switch (errno) {
                case ECHILD:
                    syslog(LOG_ERR, "await_pid(%d): No member process that is a child of the calling process.", pid);
                    break;
                case EINTR:
                    syslog(LOG_ERR, "await_pid(%d): The function was interrupted by a signal.", pid);
                    break;
            }
        }
    } while (!WIFEXITED(stat_loc) && !WIFSIGNALED(stat_loc));
}

int uread (int fd, char* buf, int size) {
    int res = read(fd, buf, size);
    if (res == -1) {
        syslog(LOG_ERR, "reader error: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return res;
}
