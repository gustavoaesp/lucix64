#ifndef _LUCIX_FS_FILE_H_
#define _LUCIX_FS_FILE_H_

#include <stdint.h>
#include <lucix/kernel.h>

struct file;
struct user_buffer;
struct file_ops
{
	int (*open)(struct inode *inode, struct file *);
	int64_t (*read)(struct file *, void *dst, uint64_t bytes, size_t *pos);
	int64_t (*write)(struct file *, const void *src, uint64_t bytes, size_t *pos);
	int (*release)(struct inode *, struct file*);
};

struct file
{
	struct inode* inode;
	struct file_ops *ops;

	uint32_t flags;

	uint64_t offset;

	void *private_data;
};

enum file_whence {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END,
};

enum file_flags {
    FILE_READABLE,
    FILE_WRITABLE,
    FILE_APPEND,
    FILE_NONBLOCK,
    FILE_NOCTTY,
};

#endif
