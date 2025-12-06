#include <lucix/syscall.h>
#include <lucix/task.h>
#include <lucix/user.h>
#include <lucix/vfs.h>

int64_t sys_open (int64_t path, int64_t flags, int64_t mode,
			int64_t __unused0, int64_t __unused1, int64_t __unused2)
{
	int ret = 0;
	char *path_str = (char*)path;
	char pathk[256];
	struct file *fp = NULL;
	int fd = 0;

	strncpy_from_user(pathk, path_str, 256);

	/* TODO sanitize the path */

	fp = vfs_open(pathk, flags, mode);
	if (!fp) {
		return -1;
	}

	fd = task_fd_table_get_free_fd_slot(current_task->fd_table);
	current_task->fd_table->fd[fd] = fp;

	return ret;
}
