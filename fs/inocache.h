#ifndef _LUCIX_INOCACHE_H_
#define _LUCIX_INOCACHE_H_

#include <lucix/fs/inode.h>

struct ino_cache_entry {
    struct list_head list;

    struct inode *ino;
    struct super_block *sb;
};

extern struct list_head ino_cache_list;

/*
*   Initialize the cache
*/
int ino_cache_init();

/*
*   Try to fetch an inode based from an inode number and super block
*/
struct inode *ino_cache_get(const struct super_block *, ino_t);

/*
*   Add new super block/inode pair
*/
int ino_cache_insert(struct super_block*, struct inode*);

/*
*   Called by the reaper when memory is running low
*/
int ino_cache_reap();

#endif