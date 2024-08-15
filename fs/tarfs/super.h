#ifndef _LUCIX_TARFS_SUPER_H_
#define _LUCIX_TARFS_SUPER_H_

#include <lucix/fs/super.h>

struct tarfs_super_block {
    struct super_block super;

    struct inode *root;
};

#endif