#ifndef _LUCIX_FS_LOOKUP_H_
#define _LUCIX_FS_LOOKUP_H_

#include <stdint.h>
#include <lucix/fs/inode.h>

enum lookup_flags {
    LOOKUP_PARENT = 1,
};

int vfs_lookup(const char *path, uint32_t flags, struct inode **);

#endif
