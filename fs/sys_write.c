#include <lucix/errno.h>
#include <lucix/syscall.h>
#include <lucix/task.h>
#include <lucix/user.h>
#include <lucix/vfs.h>
#include <lucix/printk.h>

int64_t sys_write(int64_t fd, int64_t src, int64_t count,
		int64_t __unused0, int64_t __unused1, int64_t __unused2)
{
	int64_t total = 0;
	struct file *file = NULL;

	if (fd >= current_task->fd_table->table_size)
		return -EBADF;

	file = current_task->fd_table->fd[fd];
	if (!file) {
		return -EBADF;
	}

	if (!(file->ops && file->ops->write)) {
		return -EINVAL;
	}

	for (int i = 0; i < count; i += 256) {
		int64_t remaining = count - i;
		uint8_t buff[256] = {};
		int64_t bytes_to_copy = (remaining > 256) ? 256 : remaining;

		copy_from_user(
			buff,
			(void*)(src + i),
			bytes_to_copy
		);

		total += file->ops->write(file, buff, bytes_to_copy, &file->offset);
	}

	return total;
}
