#ifndef _LUCIX_SYSCALL_H_
#define _LUCIX_SYSCALL_H_

#include <stdint.h>

#define SYSCALLS_NR	(4)

typedef int64_t (*syscall_fp_t)(
	int64_t syscall_param0,
	int64_t syscall_param1,
	int64_t syscall_param2,
	int64_t syscall_param3,
	int64_t syscall_param4,
	int64_t syscall_param5
);

extern int64_t sys_read(int64_t fd, int64_t dst, int64_t count, int64_t, int64_t, int64_t);
extern int64_t sys_write(int64_t fd, int64_t src, int64_t count, int64_t, int64_t, int64_t);
extern int64_t sys_open(int64_t path, int64_t flags, int64_t mode, int64_t, int64_t, int64_t);
extern int64_t sys_close(int64_t fd, int64_t,int64_t,int64_t,int64_t,int64_t);

/* TODO fill */
extern syscall_fp_t syscall_table[SYSCALLS_NR];

#endif
