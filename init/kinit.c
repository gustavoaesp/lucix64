#include <lucix/vfs.h>
#include <lucix/printk.h>
#include <lucix/utils.h>
#include <lucix/string.h>
#include <lucix/task.h>
#include <lucix/init/initramfs.h>
#include <arch_generic/paging.h>

#include <fs/inocache.h>
#include <fs/namecache.h>

#include <lucix/fs/inode.h>
#include <lucix/fs/exec.h>
#include <fs/ramfs/inode.h>

static int DEBUG_print_contents(struct ramfs_inode *ino, int level) {
    if ((ino->i.mode & 0xf000) == S_IFDIR) {
        struct list_head *list;
        list_for_each(list, &ino->dir_entries.list) {
            struct ramfs_dir_entry *entry = (struct ramfs_dir_entry*)list;
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

    /*DEBUG_print_contents(mnt_root->root_ino, 0);*/

    do_execve("/bin/init", NULL, NULL);


	printf("Trying to read from 0x40000\n");
	uint32_t *ptr = (uint32_t*)0x40000;
	printf("Contents: %x\n", *ptr);

    while(1) {
        asm volatile ("hlt");
    }
}
