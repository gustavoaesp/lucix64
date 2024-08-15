#ifndef _LUCIX_RAMFS_SUPER_H_
#define _LUCIX_RAMFS_SUPER_H_

#include <lucix/fs/super.h>
#include <lucix/fs/inode.h>

struct ramfs_super_block {
    struct super_block sb;

    struct inode* root;
    uint32_t ino_counter;
};

#endif