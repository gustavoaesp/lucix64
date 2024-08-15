#ifndef _LUCIX_SYSCALL_H_
#define _LUCIX_SYSCALL_H_

#include <stdint.h>

int32_t syscall(
	uint64_t syscall_id,
	uint64_t syscall_param0,
	uint64_t syscall_param1,
	uint64_t syscall_param2,
	uint64_t syscall_param3,
	uint64_t syscall_param4,
	uint64_t syscall_param
);

#endif
