#ifndef TRACE_RUNTIME_H
#define TRACE_RUNTIME_H

#include "syscall_event.h"

typedef void (*trace_observer_fn)(const struct syscall_event *ev, void *userdata);

int trace_program(char *const argv[],
                  trace_observer_fn observer,
                  void *userdata);

#endif
