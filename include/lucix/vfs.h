#ifndef _LUCIX_VFS_H_
#define _LUCIX_VFS_H_

#include <stdint.h>
#include <lucix/fs/inode.h>

/*enum fs_node_type
{
	FSNODE_FIFO = 1,
	FSNODE_CHARDEV = 2,
	FSNODE_DIRECTORY = 3,
	FSNODE_BLOCKDEV = 4,
	FSNODE_FILE = 5,
	FSNODE_SYMLINK = 6,
	FSNODE_SOCKET = 7
};*/

struct namei
{
	const char *path;
	uint32_t flags;
};

void vfs_init();

int vfs_root(const char *fs_type, dev_t device, uint32_t flags);

int vfs_mount(const char *mount_point, const char *fs_type, dev_t device, uint32_t flags);

int vfs_open(struct namei *, uint32_t oflags, uint32_t mode);
int vfs_read(struct file *, void *dst, int64_t count);
int vfs_write(struct file *, const void *biff, int64_t count);
int vfs_close(struct file *);

#endif