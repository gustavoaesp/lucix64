#ifndef _LUCIX_FS_FILE_H_
#define _LUCIX_FS_FILE_H_

#include <stdint.h>

struct file_ops
{
	int (*open)(struct inode *inode, struct file *);
	int (*release)(struct file *);
	int64_t (*read)(struct file *, void* dst, uint64_t bytes);
	int (*pread) (struct file *, struct user_buffer, int64_t sz, int64_t off);
    int (*readdir) (struct file *, struct file_readdir_handler *);
    int (*read_dent) (struct file *, struct user_buffer, int64_t dent_size);
    int64_t (*lseek) (struct file *, int64_t offset, int whence);
	int64_t (*write)(struct file *, const void *src, uint64_t bytes);
	int (*ioctl) (struct file *, int cmd, struct user_buffer arg);
    int (*poll) (struct file *, struct poll_table *, int events);
};

struct file
{
	struct inode* inode;
	struct file_ops *ops;

	uint32_t flags;

	uint64_t size;
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