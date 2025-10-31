#include <fs/inocache.h>
#include <fs/namecache.h>
#include <fs/lookup.h>

#include <lucix/printk.h>
#include <lucix/errno.h>
#include <lucix/slab.h>
#include <lucix/string.h>
#include <lucix/fs/super.h>
#include <lucix/vfs.h>

#include <lucix/fs/file_system.h>
#include <lucix/fs/file.h>

struct vfs_mount *mnt_root = NULL;

static obj_mem_cache_t *file_mem_cache;

static int get_last_slash(const char *path)
{
    int last_slash = -1;
    for (int i = 0; path[i]; ++i) {
        if (path[i] == '/' && path[i + 1]) {
            last_slash = i;
        }
    }

    return last_slash;
}

void vfs_init()
{
    file_mem_cache = create_obj_mem_cache(sizeof(struct file), 0, 0, "vfs-file-struct");
    ino_cache_init();
    fs_name_cache_init();
}

int vfs_root(const char *fs_type, dev_t device, uint32_t flags)
{
    struct file_system *fs = NULL;
    struct super_block *sb = NULL;
    struct inode *root_ino = NULL;
    if (mnt_root) {
        return -EINVAL;
    }

    mnt_root = kmalloc(sizeof(struct vfs_mount), 0);
    fs = file_system_lookup(fs_type);
    if (!fs) {
        return -EINVAL;
    }
    sb = fs->alloc_sb();
    fs->read_sb(sb, device);

    root_ino = sb->ops->get_root_inode(sb);

    if (!root_ino) {
        return -EINVAL;
    }

    INIT_LIST_HEAD(&mnt_root->list);
    mnt_root->mnt_ino = root_ino;
    mnt_root->root_ino = root_ino;
    mnt_root->super_block = sb;

    ino_ref(root_ino);

    return 0;
}

int vfs_mount(const char *mount_point, const char *fs_type, dev_t device, uint32_t flags)
{
    struct vfs_mount *new_mnt = NULL;
    struct super_block *sb = NULL;
    struct file_system *fs = NULL;
    fs = file_system_lookup(fs_type);
    if (!fs) {
        return -ENOENT;
    }
    sb = fs->alloc_sb();
    fs->read_sb(sb, device);

    new_mnt = kmalloc(sizeof(struct vfs_mount), 0);

    fs = file_system_lookup(fs_type);
    return 0;
}

int vfs_mount_nodev(const char *mount_point, const char *fs_type, uint32_t flags)
{
    return 0;
}

struct file *vfs_open(const char *path, uint32_t oflags, uint32_t mode)
{
    return NULL;
}

int vfs_mkdir(const char *path, mode_t mode)
{
    struct inode *parent_ino = NULL;
    char *directory_name = NULL;
    int ret = 0;

    ret = vfs_lookup(path, LOOKUP_PARENT, &parent_ino);
    if (ret) {
        return ret;
    }

    directory_name = strrchr(path, '/');
    if (directory_name) {
        directory_name++;
    }

    ret = parent_ino->ops->mkdir(parent_ino, directory_name, strlen(directory_name), mode);

    ino_deref(parent_ino);

    return ret;
}