#include "tarfs.h"
#include "super.h"
#include "inode.h"

#include <arch/paging.h>

#include <lucix/errno.h>
#include <lucix/init.h>
#include <lucix/initcall.h>
#include <lucix/slab.h>
#include <lucix/string.h>

#include <lucix/fs/file_system.h>
#include <lucix/fs/super.h>

static obj_mem_cache_t* tarfs_inode_cache = NULL;

static struct super_block *tarfs_alloc_sb()
{
    return kmalloc(sizeof(struct tarfs_super_block), 0);
}

static int tarfs_dealloc_sb(struct super_block *sb)
{
    kfree(sb);
    return 0;
}

static int __build_as_ramfs(struct tarfs_super_block *tarfs_sb)
{
    int segments = initramfs_info.size / 512 + (initramfs_info.size % 512) ? 1 : 0;
    void *ptr = initramfs_info.addr;

    struct tar_header *info = ptr;
    if (memcmp(info->magic, TMAGIC, strlen(TMAGIC))) {
        return -EINVAL;
    }

    return 0;
}

static int tarfs_read_sb(struct super_block *sb)
{
    struct tarfs_super_block *tarfs_super = sb;
    struct tarfs_inode *tarfs_ino = NULL;
    tarfs_super->root = mem_cache_alloc_obj(tarfs_inode_cache);
    tarfs_super->root->mode = S_IFDIR;
    tarfs_super->root->block_size = PAGE_SIZE;
    tarfs_super->root->blocks = 0;
    tarfs_ino = tarfs_super->root;

    INIT_LIST_HEAD(&tarfs_ino->dir_entries);
}

static struct file_system tarfs_file_system = {
    .name = "tarfs",
    .flags = FS_NODEV,
    .alloc_sb = tarfs_alloc_sb,
    .dealloc_sb = tarfs_dealloc_sb,
    .read_sb = tarfs_read_sb
};

static int tarfs_load()
{
    tarfs_inode_cache = create_obj_mem_cache(sizeof(struct tarfs_inode), 0, 0, "tarfs-inode-cache");
    file_system_register(&tarfs_file_system);
    return 0;
}

static void tarfs_unload()
{
}

initcall_fs(tarfs_load);