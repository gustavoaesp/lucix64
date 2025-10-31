#ifndef _LUCIX_SUPER_H_
#define _LUCIX_SUPER_H_

#include <lucix/list.h>
#include <lucix/types.h>

struct super_block;

struct super_block_ops {
    struct inode *(*inode_mem_alloc)(struct super_block*);
    int (*inode_mem_free)(struct super_block*, struct inode*);

    int (*inode_read)(struct super_block*, struct inode*);
    int (*inode_write)(struct super_block*, struct inode*);

    int (*inode_delete)(struct super_block*, struct inode*);

    struct inode *(*get_root_inode)(struct super_block *);

    int (*sb_write)(struct super_block*);
    int (*sb_put)(struct super_block*);
};

struct super_block {
    struct list_head list_entry;

    dev_t device;

    int count;

    struct file_system* fs;

    struct list_head inodes;
    struct list_head inodes_dirty;

    struct super_block_ops* ops;
};

#endif