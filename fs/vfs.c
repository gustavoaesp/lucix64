#include <lucix/printk.h>
#include <lucix/errno.h>
#include <lucix/slab.h>
#include <lucix/fs/super.h>
#include <lucix/vfs.h>

#include <lucix/fs/file_system.h>
#include <lucix/fs/file.h>

struct vfs_dentry {

};

struct vfs_mount {
    struct list_head list;
    char *path;
    struct super_block *super_block;
};

static struct vfs_mount *root = NULL;

static obj_mem_cache_t *file_mem_cache;

void vfs_init()
{
    file_mem_cache = create_obj_mem_cache(sizeof(struct file), 0, 0, "vfs-file-struct");
}

int vfs_mount(const char *mount_point, const char *fs_type, dev_t device, uint32_t flags)
{
    /* TODO */
    return 0;
}

int vfs_mount_nodev(const char *mount_point, const char *fs_type, uint32_t flags)
{
    struct file_system *fs = file_system_lookup(fs_type);
    if (!fs) {
        return -EINVAL;
    }
    root = kmalloc(sizeof(struct vfs_mount), 0);
    INIT_LIST_HEAD(&root->list);
    root->path = "/";
    root->super_block = fs->alloc_sb();
    if (!root->super_block) {
        printf("Panic: no superblock for root filesystem %s", fs_type);
        return -1;
    }

    fs->read_sb(root->super_block);

    return 0;
}

int vfs_open(struct namei *namei, uint32_t oflags, uint32_t mode)
{
    return 0;
}