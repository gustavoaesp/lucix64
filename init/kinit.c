#include <lucix/cpu.h>
#include <lucix/vfs.h>
#include <lucix/printk.h>
#include <lucix/utils.h>
#include <lucix/string.h>
#include <lucix/task.h>
#include <lucix/cdev.h>
#include <lucix/init/initramfs.h>
#include <arch_generic/paging.h>

#include <fs/inocache.h>
#include <fs/namecache.h>

#include <lucix/fs/inode.h>
#include <lucix/fs/exec.h>
#include <uapi/lucix/fcntl.h>
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
	struct cpu *cpu = cpu_get_cpu();
	struct list_head* pos;
	struct file *fstdout = NULL;
	int ret = 0;
	printf("kinit_task\n");
	vfs_root("ramfs", 0, 0);

	unpack_initramfs();

	/*
	*	For now we create the device for the console in /dev
	* */
	vfs_mknod("/dev/console", S_IFCHR | 0666, MKDEV(5, 1));
	fstdout = vfs_open("/dev/console", O_WRONLY, 0666);

	cpu->current->task->fd_table->fd[1] = fstdout;

	do_execve("/bin/init", NULL, NULL);

	/* Should not reach here (exec will jmp into userspace's entry point */
	while(1) {
		asm volatile ("hlt");
	}
}
