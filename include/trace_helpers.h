#ifndef TRACE_HELPERS_H
#define TRACE_HELPERS_H

#include <stddef.h>
#include <sys/types.h>

int read_child_string(pid_t pid, unsigned long addr, char *buf, size_t bufsz);

#endif
