#ifndef _LUCIX_RAMFS_INODE_H_
#define _LUCIX_RAMFS_INODE_H_

#include <lucix/fs/inode.h>

#define RAMFS_MAX_FILENAME (256)

struct ramfs_inode {
    struct inode i;

    union {
        /* a list of pointers to blocks of PAGE_SIZE size for a regular file
        */
        uint8_t **blocks;
        /*
        *   This is the head of all directories available in this inode
        *   (has to be a DIR)
        */
        struct { struct list_head list; }  dir_entries;
    };
};

struct ramfs_dir_entry {
    struct list_head list_entry;
    char name[RAMFS_MAX_FILENAME];
    struct ramfs_inode* i;
};

#endif