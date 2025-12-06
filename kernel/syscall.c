#include <lucix/syscall.h>

syscall_fp_t syscall_table[SYSCALLS_NR] = {
	sys_read,
	sys_write,
	sys_open,
};
