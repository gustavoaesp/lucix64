#include <lucix/task.h>
#include <lucix/utils.h>

int task_fd_table_get_free_fd_slot(struct fd_table *fd_table)
{
	int ret = 0;
	struct file **new_table = NULL;
	for (int i = 0; i < fd_table->table_size; ++i) {
		if (fd_table->fd[i])
			continue;
		return i;
	}

	new_table = kmalloc(sizeof(struct file*) * fd_table->table_size + 32, 0);
	memset(new_table, 0, sizeof(struct file*)*(fd_table->table_size + 32));
	memcpy(new_table, fd_table->fd, sizeof(struct file*) * fd_table->table_size);

	kfree(fd_table->fd);
	fd_table->fd = new_table;
	ret = fd_table->table_size;
	fd_table->table_size = fd_table->table_size + 32;

	return ret;
}
