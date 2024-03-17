#ifndef _LUCIX_SYSCALL_H_
#define _LUCIX_SYSCALL_H_

struct syscall
{
	uint64_t syscall_n;
	uint64_t params[4];
};

#endif
