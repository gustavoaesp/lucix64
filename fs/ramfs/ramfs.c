#include <fs/ramfs/inode.h>
#include <fs/ramfs/super.h>

#include <lucix/errno.h>
#include <lucix/fs/file_system.h>
#include <lucix/initcall.h>
#include <lucix/slab.h>
#include <lucix/string.h>

#include <lucix/printk.h>

static obj_mem_cache_t* ramfs_inode_cache = NULL;
uint32_t k_ramfs_block_size = 0x1000;

static int ramfs_create_root_ino(struct ramfs_super_block *sb)
{
    struct ramfs_inode *inode = mem_cache_alloc_obj(ramfs_inode_cache);
    inode->i.mode = S_IFDIR;
    inode->i.sb = sb;
    inode->i.block_size = k_ramfs_block_size;
    inode->i.id = 2;
    INIT_LIST_HEAD(&inode->dir_entries.list);
}

static int ramfs_ino_lookup(struct inode* dir, const char* name, uint32_t namelen, struct inode **out)
{
    struct ramfs_inode *ramfs_inode = dir;
    struct ramfs_dir_entry *dir_entry;
    list_for_each_entry(dir_entry, &ramfs_inode->dir_entries, list_entry) {
        if (!strcmp(dir_entry->name, name)) {
            *out = dir_entry->i;
            return 0;
        }
    }

    return -ENOENT; /* TODO fill all errnos*/
}

static int ramfs_ino_truncate(struct inode *i, uint64_t size)
{
    struct ramfs_inode *ramfs_inode = i;

    return 0;
}

static struct inode *ramfs_sb_inode_memalloc(struct super_block *base_sb)
{
    return mem_cache_alloc_obj(ramfs_inode_cache);
}

static int ramfs_sb_inode_mem_free(struct super_block *base_sb, struct inode *inode)
{
    mem_cache_free_obj(ramfs_inode_cache, inode);
    return 0;
}

static int *ramfs_sb_inode_read(struct super_block *base_sb, struct inode *i)
{
    struct ramfs_inode *inode = i;
    struct ramfs_super_block *sb = base_sb;

    if (!sb->root) {
        ramfs_create_root_ino(sb);
        if (i->id != 2) {
            return -ENOENT; /* TODO include all errno's as macros */
        }
    }
}

static struct super_block *ramfs_alloc_sb ()
{
    struct ramfs_super_block *sb = kmalloc(sizeof(struct ramfs_super_block), 0);
    sb->ino_counter = 3;
    return sb;
}

static void ramfs_dealloc_sb(struct super_block* sb)
{
}

static void ramfs_read_sb(struct super_block *sb)
{
}

static struct file_system ramfs_filesystem = {
    .alloc_sb = ramfs_alloc_sb,
    .name = "ramfs",
    .flags = FS_NODEV,
    .dealloc_sb = ramfs_dealloc_sb
};

static int ramfs_init()
{
    ramfs_inode_cache = create_obj_mem_cache(
        sizeof(struct ramfs_inode),
        0, 0,
        "ramfs-inode-cache"
    );

    if (!ramfs_inode_cache) {
        printf("Can't start the ramfs inode cache\n");
        return -1;
    }

    file_system_register(&ramfs_filesystem);

    return 0;
}

static int ramfs_unload()
{
}

initcall_fs(ramfs_init)