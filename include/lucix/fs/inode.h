#ifndef _LUCIX_INODE_H_
#define _LUCIX_INODE_H_

#include <stdint.h>
#include <lucix/block.h>
#include <lucix/list.h>
#include <lucix/types.h>
#include <arch_generic/cpu.h>

typedef uint32_t ino_t;
typedef uint32_t mode_t;
typedef uint64_t time_t;

#define S_IFIFO     (0x1000)
#define S_IFCHR     (0x2000)
#define S_IFDIR     (0x4000)
#define S_IFBLK     (0x6000)
#define S_IFREG     (0x8000)
#define S_IFLNK     (0xA000)
#define S_IFSOCK    (0xC000)

#define INO_TYPE(ino) ((ino)->mode & 0xF000)

struct inode_ops;

struct inode {
	struct list_head list_entry;
	ino_t id;
	/* Permissions and type of inode are stored here */
	mode_t mode;
	size_t size;

	/* used when the inode represents a device file */
	dev_t dev;

	uint32_t uid, gid;

	time_t atime, ctime, mtime;

	/** How many blocks does this inode use */
	uint32_t blocks, block_size;

	uint32_t refcnt;

	struct page_mapping *f_map;
	struct inode_ops *ops;
	struct file_ops *default_fops;
	struct super_block* sb;
};

struct inode_ops {
	/*
	*   Only applies if this inode is a directory, it will lookup the name provided in the entries,
	*   and return the inode in result, 
	*/
	int (*lookup)(struct inode* dir, const char* name, uint32_t namelen, struct inode**);

	/*int (*truncate)(struct inode*, uint64_t size);*/

	/*sector_t (*blockmap)(struct inode*, sector_t);
	sector_t (*blockmap_alloc)(struct inode*, sector_t);*/

	int (*create) (struct inode *dir, const char *name, mode_t mode, struct inode **result);
	int (*mkdir) (struct inode *, const char *name, size_t len, mode_t mode);
	/*int (*link) (struct inode *dir, struct inode *old, const char *name, size_t len);*/
	int (*mknod) (struct inode *dir, const char *name, size_t len, mode_t mode, dev_t dev);

	/*int (*unlink) (struct inode *dir, struct inode *entity, const char *name, size_t len);
	int (*rmdir) (struct inode *dir, struct inode *entity, const char *name, size_t len);

	int (*rename) (struct inode *old_dir, const char *name, size_t len,
		   struct inode *new_dir, const char *new_name, size_t new_len);

	int (*symlink) (struct inode *dir, const char *symlink, size_t len, const char *symlink_target);
	int (*readlink) (struct inode *dir, char *, size_t buf_len);
	int (*follow_link) (struct inode *dir, struct inode *symlink, struct inode **result);*/
};

static inline void ino_ref(struct inode *ino)
{
	uint64_t irq_state = cpu_irq_save();
	ino->refcnt++;
	cpu_irq_restore(irq_state);
}

static inline void ino_deref(struct inode *ino)
{
	uint64_t irq_state = cpu_irq_save();
	ino->refcnt--;
	cpu_irq_restore(irq_state);
}

#endif
