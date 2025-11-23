#include <arch/paging.h>
#include <arch_generic/sched.h>
#include <lucix/fs/exec.h>
#include <lucix/binfmt.h>
#include <lucix/printk.h>
#include <lucix/task.h>
#include <lucix/vfs.h>
#include <lucix/vma.h>
#include <lucix/mmap.h>
#include <uapi/lucix/fcntl.h>

int do_execve(const char *path, const char **argv, const char **envp)
{
	struct file *file = NULL;
	struct list_head *pos = NULL;

	if (list_empty(&binfmt_list)) {
		printf("ERROR: binfmt list is empty!\n");
		return -1;
	}

	file = vfs_open(path, O_RDONLY, 0);

	list_for_each(pos, &binfmt_list) {
		struct binfmt *fmt = (struct binfmt*)pos;
		void *new_cpu_state = NULL;
		uint64_t irq_state = cpu_irq_save();
		if (fmt->load_binary(file, path, NULL, NULL, &new_cpu_state) == 0) {
			/*printf("cpu_state: %p\n", new_cpu_state);*/
			cpu_irq_restore(irq_state);
			cpu_context_switch(new_cpu_state);
			return 0;
		}
		cpu_irq_restore(irq_state);
	}

	return -1;
}
