#ifndef _USR_SYSCALL_H_
#define _USR_SYSCALL_H_

#include <stdint-gcc.h>
//#include <stddef-gcc.h>

extern int64_t write(int fd, void *src, size_t count);

#endif
