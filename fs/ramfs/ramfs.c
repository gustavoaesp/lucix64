#include <arch/paging.h>

#include <fs/ramfs/inode.h>
#include <fs/ramfs/super.h>
#include <fs/inocache.h>

#include <lucix/errno.h>
#include <lucix/fs/file_system.h>
#include <lucix/initcall.h>
#include <lucix/slab.h>
#include <lucix/string.h>
#include <lucix/utils.h>

#include <lucix/printk.h>

static obj_mem_cache_t* ramfs_inode_cache = NULL;
uint32_t k_ramfs_block_size = 0x1000;

extern struct inode_ops ramfs_ino_ops;

static int ramfs_ino_lookup(struct inode* dir, const char* name, uint32_t namelen, struct inode **out)
{
    struct ramfs_inode *ramfs_inode = dir;
    struct ramfs_dir_entry *dir_entry;
    list_for_each_entry(dir_entry, &ramfs_inode->dir_entries, list_entry) {
        if (!memcmp(dir_entry->name, name, namelen)) {
            *out = dir_entry->i;
            ino_ref(*out);
            return 0;
        }
    }

    return -ENOENT; /* TODO fill all errnos*/
}

static int ramfs_ino_create_generic(struct inode *dir, const char *name, mode_t mode, struct inode **res)
{
    struct inode *new_ino = NULL;
    struct ramfs_super_block *sb = (struct ramfs_super_block*)dir->sb;
    struct ramfs_inode *ramfs_dir = (struct ramfs_inode*)dir;
    struct ramfs_dir_entry *new_entry = NULL;
    struct list_head *pos = NULL;

    list_for_each(pos, &ramfs_dir->dir_entries.list) {
        struct ramfs_dir_entry *entry = (struct ramfs_dir_entry *)pos;
        if (!strcmp(name, entry->name)) {
            *res = NULL;
            return -EEXIST;
        }
    }

    new_ino = mem_cache_alloc_obj(ramfs_inode_cache);
    new_entry = kmalloc(sizeof(struct ramfs_dir_entry), 0);
    memset(new_entry, 0, sizeof(struct ramfs_dir_entry));
    strcpy(new_entry->name, name);
    new_entry->i = new_ino;
    new_ino->mode = mode;
    new_ino->id = sb->ino_counter++;
    new_ino->blocks = 0;
    new_ino->block_size = PAGE_SIZE;
    new_ino->sb = dir->sb;
    new_ino->ops = &ramfs_ino_ops;

    list_add(&new_entry->list_entry, &ramfs_dir->dir_entries.list);

    ((struct ramfs_inode*)new_ino)->blocks = NULL;

    *res = new_ino;
    ino_ref(*res);
    ino_cache_insert(dir->sb, *res);

    return 0;
}

static int ramfs_ino_create(struct inode *dir, const char *name, mode_t mode, struct inode **res)
{
    int ret = ramfs_ino_create_generic(dir, name, (mode & 0x7) | S_IFREG, res);
    ino_ref(*res);
    return ret;
}

static int ramfs_ino_mkdir(struct inode *dir, const char *name, mode_t mode)
{
    struct inode *res = NULL;
    int ret = ramfs_ino_create_generic(dir, name, (mode & 0x7) | S_IFDIR, &res);
    struct ramfs_inode *new_dir = NULL;
    struct ramfs_dir_entry *entry = NULL;
    if (ret) {
        return ret;
    }
    new_dir = (struct ramfs_inode*)res;
    INIT_LIST_HEAD(&new_dir->dir_entries.list);

    /* set '..' to the parent */
    entry = kmalloc(sizeof(struct ramfs_dir_entry), 0);
    entry->i = dir;
    strcpy(entry->name, "..");
    list_add(&entry->list_entry, &new_dir->dir_entries.list);

    /* set '.' to self */
    entry = kmalloc(sizeof(struct ramfs_dir_entry), 0);
    entry->i = res;
    strcpy(entry->name, ".");
    list_add(&entry->list_entry, &new_dir->dir_entries.list);

    return ret;
}

struct inode_ops ramfs_ino_ops = {
    .mkdir = ramfs_ino_mkdir,
    .lookup = ramfs_ino_lookup,
    .create = ramfs_ino_create,
};

static struct inode *ramfs_sb_inode_mem_alloc(struct super_block *base_sb)
{
    return mem_cache_alloc_obj(ramfs_inode_cache);
}

static int ramfs_sb_inode_mem_free(struct super_block *base_sb, struct inode *inode)
{
    mem_cache_free_obj(ramfs_inode_cache, inode);
    return 0;
}

/*
*   The way ramfs works will keep inodes in the caches so 'reading' an inode will not be necessary
*/
static int ramfs_sb_inode_read(struct super_block *base_sb, struct inode *i)
{
    return -EINVAL;
}

/*
*   Does nothing
*/
static int *ramfs_sb_inode_write(struct super_block *base_sb, struct inode *i)
{
    return 0;
}

static int ramfs_sb_inode_delete(struct super_block *base_sb, struct inode *i)
{
    return 0;
}

static struct inode *ramfs_sb_get_root_inode(struct super_block *base_sb)
{
    struct ramfs_super_block *super = (struct ramfs_super_block*)base_sb;
    return super->root;
}

static int ramfs_sb_write(struct super_block *sb)
{
    return 0;
}

static int ramfs_sb_put(struct super_block *sb)
{
    return 0;
}

static struct super_block *ramfs_alloc_sb ()
{
    struct ramfs_super_block *sb = kmalloc(sizeof(struct ramfs_super_block), 0);
    return sb;
}

static int ramfs_dealloc_sb(struct super_block* sb)
{
    kfree(sb);
    return 0;
}

struct super_block_ops ramfs_sb_ops = {
    .inode_mem_alloc = ramfs_sb_inode_mem_alloc,
    .inode_mem_free = ramfs_sb_inode_mem_free,
    .inode_read = ramfs_sb_inode_read,
    .inode_write = ramfs_sb_inode_write,
    .inode_delete = ramfs_sb_inode_delete,
    .get_root_inode = ramfs_sb_get_root_inode,
    .sb_write = ramfs_sb_write,
    .sb_put = ramfs_sb_put
};

static int ramfs_read_sb(struct super_block *sb, dev_t device)
{
    struct ramfs_super_block *super = sb;
    struct ramfs_inode *root_ino = NULL;
    struct ramfs_dir_entry *dir_entry = NULL;

    super->ino_counter = 2;
    super->root = mem_cache_alloc_obj(ramfs_inode_cache);
    super->root->id = super->ino_counter++;
    super->root->mode = S_IFDIR | 0666;
    super->root->sb = sb;
    ino_ref(super->root);
    ino_cache_insert(sb, super->root);

    root_ino = (struct ramfs_inode*)super->root;
    INIT_LIST_HEAD(&root_ino->dir_entries.list);

    sb->ops = &ramfs_sb_ops;
    super->root->ops = &ramfs_ino_ops;

    /* set '..' to root */
    dir_entry = kmalloc(sizeof(struct ramfs_dir_entry), 0);
    dir_entry->i = root_ino;
    strcpy(dir_entry->name, "..");
    list_add(&dir_entry->list_entry, &root_ino->dir_entries.list);

    /* set '.' to root as well */
    dir_entry = kmalloc(sizeof(struct ramfs_dir_entry), 0);
    dir_entry->i = root_ino;
    strcpy(dir_entry->name, ".");
    list_add(&dir_entry->list_entry, &root_ino->dir_entries.list);

    return 0;
}

static struct file_system ramfs_filesystem = {
    .alloc_sb = ramfs_alloc_sb,
    .name = "ramfs",
    .flags = FS_NODEV,
    .dealloc_sb = ramfs_dealloc_sb,
    .read_sb = ramfs_read_sb
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