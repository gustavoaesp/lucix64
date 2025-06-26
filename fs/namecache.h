#ifndef _LUCIX_NAME_CACHE_H_
#define _LUCIX_NAME_CACHE_H_

#include <lucix/fs/inode.h>
#include <lucix/list.h>
#include <lucix/slab.h>

#define MAX_NAME (255)

struct qstr {
    const char *str;
    uint32_t len;
};

/*
*   Initialize the cache
*/
int fs_name_cache_init();

/*
*   Fast lookup, returns null if element not found
*/
struct inode *fs_name_cache_fast_lookup(struct inode *parent, const struct qstr *name);

/*
*   A new inode insertion
*/
int fs_name_cache_add_inode(struct inode *parent, const struct qstr *name, struct inode *);

/*
*   Called by the reaper when memory is running low
*/
int fs_name_cache_reap();

#endif