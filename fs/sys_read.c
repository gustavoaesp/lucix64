#include <lucix/syscall.h>
#include <lucix/task.h>
#include <lucix/user.h>
#include <lucix/vfs.h>

int64_t sys_read(int64_t fd, int64_t dst, int64_t count,
		int64_t __unused0, int64_t __unused1, int64_t __unused2)
{
	int64_t total = 0;
	struct file *file = NULL;

	return total;
}
