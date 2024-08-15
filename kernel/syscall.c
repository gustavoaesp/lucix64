#include <lucix/syscall.h>
#include <lucix/types.h>

enum syscall_id
{
    SYS_OPEN    = 0,
    SYS_CLOSE,
    SYS_READ,
    SYS_WRITE,
    SYS_MMAP,
    SYS_FORK,
    SYS_EXEC
};

static int32_t sys_open(const userptr_t path, uint32_t oflag, uint32_t mode)
{
    return 0;
}

int32_t syscall(
	uint64_t syscall_id,
	uint64_t syscall_param0,
	uint64_t syscall_param1,
	uint64_t syscall_param2,
	uint64_t syscall_param3,
	uint64_t syscall_param4,
	uint64_t syscall_param
)
{
    return 0;
}