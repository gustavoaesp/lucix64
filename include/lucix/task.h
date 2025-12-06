#ifndef _LUCIX_TASK_H_
#define _LUCIX_TASK_H_

#include <stdint.h>
#include <lucix/slab.h>

struct fd_table
{
	struct file **fd;
	int table_size;
};

struct procmm
{
	/* this is a linked list */
	struct list_head vm_areas;

	void* pgtable;
};

struct task
{
	struct list_head list;

	struct procmm* mm;

	struct fd_table *fd_table;

	uint32_t pid;
	uint32_t secondary_id;

	int64_t kstack_size;
	/*uint64_t kstack_top;*/
	uint64_t ksp;
	void *kstack;

	uint64_t entry;

	void *cpu_state;
};

/*struct task *create_task();*/

/*
void task_exec(struct task *, void *elf, uint64_t size);
*/

int task_fd_table_get_free_fd_slot(struct fd_table *);

extern struct task *current_task;
#endif
