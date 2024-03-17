#ifndef _LUCIX_VFS_H_
#define _LUCIX_VFS_H_

#include <stdint.h>

enum fs_node_type
{
	FSNODE_FIFO = 1,
	FSNODE_CHARDEV = 2,
	FSNODE_DIRECTORY = 3,
	FSNODE_BLOCKDEV = 4,
	FSNODE_FILE = 5,
	FSNODE_SYMLINK = 6,
	FSNODE_SOCKET = 7
};

struct fs_node
{
	enum fs_node_type type;
};

struct file_ops
{
	int (*read)(void* dst, uint64_t bytes);
};

struct file
{
	struct fs_node* node;
	uint64_t size;
	uint64_t offset;
};

void vfs_init();

#endif
