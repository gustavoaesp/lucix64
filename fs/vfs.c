#include <fs/inocache.h>
#include <fs/namecache.h>

#include <lucix/printk.h>
#include <lucix/errno.h>
#include <lucix/slab.h>
#include <lucix/fs/super.h>
#include <lucix/vfs.h>

#include <lucix/fs/file_system.h>
#include <lucix/fs/file.h>

struct vfs_mount {
    struct list_head list;
    struct super_block *super_block;
};

static struct vfs_mount *root = NULL;

static obj_mem_cache_t *file_mem_cache;

void vfs_init()
{
    file_mem_cache = create_obj_mem_cache(sizeof(struct file), 0, 0, "vfs-file-struct");
    ino_cache_init();
    fs_name_cache_init();
}

int vfs_mount(const char *mount_point, const char *fs_type, dev_t device, uint32_t flags)
{
    return 0;
}

int vfs_mount_nodev(const char *mount_point, const char *fs_type, uint32_t flags)
{
    return 0;
}

int vfs_open(struct namei *namei, uint32_t oflags, uint32_t mode)
{
    return 0;
}