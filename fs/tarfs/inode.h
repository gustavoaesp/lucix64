#ifndef _LUCIX_TARFS_INODE_H_
#define _LUCIX_TARFS_INODE_H_

#include <stdint.h>

#include <lucix/fs/inode.h>
#include <lucix/list.h>

struct tarfs_inode {
    struct inode i;

    union {
        uint8_t **blocks;
        struct list_head dir_entries;
    };
};

#endif