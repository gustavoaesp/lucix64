#ifndef _LUCIX_VFS_H_
#define _LUCIX_VFS_H_

#include <stdint.h>
#include <lucix/fs/inode.h>
#include <lucix/fs/file.h>

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

struct vfs_mount {
	struct list_head list;
	struct super_block *super_block;
	/*
	*   This points to the inode it mounted on in a parent filesystem
	*   If this mount is the root mount it will point to the root ino
	*   itself. It will refcount the inode.
	*/
	struct inode *mnt_ino;
	/*
	*	Root inode of this mount. It is refcounted
	*/
	struct inode *root_ino;
};

extern struct vfs_mount *mnt_root;

void vfs_init();

/*
*	Set root(can only be called at startup, if there is a root fs already mounted
*	it will throw error)
*/
int vfs_root(const char *fs_type, dev_t device, uint32_t flags);

int vfs_mount(const char *mount_point, const char *fs_type, dev_t device, uint32_t flags);

/*
*	Open a file
*/
struct file *vfs_open(const char*, uint32_t oflags, uint32_t mode);

/*
*	Create a file
*/
int vfs_create(const char*, mode_t);
int vfs_mkdir(const char*, mode_t);
int vfs_mknod(const char*, mode_t, dev_t);
int vfs_read(struct file *, void __user *, int64_t count);
int vfs_write(struct file *, const void __user *, int64_t count);
int vfs_close(struct file *);
int vfs_lseek(struct file *, size_t, uint32_t whence);

/*
 *    Generic ops
 */
int64_t generic_file_write(struct file *, const void *src, size_t count, size_t *pos);
int64_t generic_file_read(struct file*, void *dst, size_t count, size_t *pos);

#endif
