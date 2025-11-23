#ifndef _LUCIX_FS_FILE_H_
#define _LUCIX_FS_FILE_H_

#include <stdint.h>
#include <lucix/kernel.h>
#include <lucix/vma.h>
#include <lucix/fs/inode.h>

struct file;
struct user_buffer;
struct file_ops
{
	int (*open)(struct inode *inode, struct file *);
	int64_t (*read)(struct file *, void *dst, uint64_t bytes, size_t *pos);
	int64_t (*write)(struct file *, const void *src, uint64_t bytes, size_t *pos);
	int (*mmap)(struct file *, struct vm_area*);
	int (*lseek)(struct file *, size_t, uint32_t);
	int (*release)(struct inode *, struct file*);
};

struct file
{
	struct inode* inode;
	struct file_ops *ops;

	uint32_t flags;

	uint64_t offset;

	uint32_t refcnt;

	void *private_data;
};

enum file_whence {
    SEEK_SET = 0,
    SEEK_CUR = 1,
    SEEK_END = 2,
};

enum file_flags {
    FILE_READABLE,
    FILE_WRITABLE,
    FILE_APPEND,
    FILE_NONBLOCK,
    FILE_NOCTTY,
};

static inline void file_ref(struct file *file)
{
	uint64_t irq_state = cpu_irq_save();
	file->refcnt++;
	cpu_irq_restore(irq_state);
}

static inline void file_unref(struct file *file)
{
	uint64_t irq_state = cpu_irq_save();
	file->refcnt--;
	cpu_irq_restore(irq_state);
}

#endif
