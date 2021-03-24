#pragma once
#include <unistd.h>

pid_t ufork ();
void upipe (int* fd);
void await_pid (pid_t pid);

void* umalloc (int size, char* error);
void* urealloc (void* src, int size, char* error);

int uread (int fd, char* buf, int size);
