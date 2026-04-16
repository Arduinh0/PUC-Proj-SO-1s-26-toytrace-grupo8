#ifndef SYSCALL_NAMES_H
#define SYSCALL_NAMES_H

const char *syscall_name(long syscall_no);
long syscall_number_by_name(const char *name);

#endif
