#ifndef _LUCIX_FILE_SYSTEM_H_
#define _LUCIX_FILE_SYSTEM_H_

#include <stdint.h>
#include <lucix/list.h>
#include <lucix/types.h>

enum {
    FS_NODEV = 1
};

struct file_system {
    struct list_head list_entry;

    const char *name;
    uint32_t flags;

    struct super_block *(*alloc_sb)(void);
    int (*dealloc_sb)(struct super_block*);

    int (*read_sb)(struct super_block *, dev_t device);
};

void file_system_register(struct file_system*);
void file_system_unregister(const char *name);

struct file_system *file_system_lookup(const char *name);

#endif