#include <lucix/vfs.h>
#include <lucix/printk.h>

#include <fs/inocache.h>
#include <fs/namecache.h>

#include <lucix/fs/inode.h>
#include <fs/ramfs/inode.h>

static int DEBUG_print_contents(struct ramfs_inode *ino, int level) {
    if ((ino->i.mode & 0xf000) == S_IFDIR) {
        struct list_head *list;
        list_for_each(list, &ino->dir_entries.list) {
            struct ramfs_dir_entry *entry = list;
            for (int i = 0; i < level; ++i) printf(" ");
            printf("%s\n", entry->name);
            if (!strcmp(entry->name, ".") || !strcmp(entry->name, ".."))
                continue;
            DEBUG_print_contents(entry->i, level + 1);
        }
    }

    return 0;
}

void kinit_task(void *__unused)
{
    struct list_head* pos;
    int ret = 0;
    printf("kinit_task\n");
    vfs_root("ramfs", 0, 0);

    unpack_initramfs();

    DEBUG_print_contents(mnt_root->root_ino, 0);

    while(1) {
        asm volatile ("hlt");
    }
}