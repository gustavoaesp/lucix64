#ifndef _LUCIX_FS_H_
#define _LUCIX_FS_H_

#include <lucix/compiler.h>

enum inode_type {
    INODE_FILE = 1,
    INODE_DIRECTORY,
    INODE_SYMLINK,
    INODE_BLOCKDEV,
    INODE_CHARDEV,
    INODE_PIPE
};
#endif