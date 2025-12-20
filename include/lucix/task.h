#ifndef _LUCIX_TASK_H_
#define _LUCIX_TASK_H_

#include <stdint.h>
#include <lucix/slab.h>

enum task_state
{
	TASK_RUNNABLE = 0,
	TASK_BLOCKED = 1,
};

enum task_creation_flags {
	TASK_KERNEL = 1,
	TASK_USER = 2
};

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
	struct list_head children;
	struct task *parent;

	/* memory map (address space, can be shared)*/
	struct procmm* mm;

	/* File descriptor table */
	struct fd_table *fd_table;

	/* Kernel Stack */
	void *kstack;
	/* Kernel stack pointer
	 * (keep it here, offset 64 or 32 depending on the arch,
	 * do not change any previous field)
	 * */
	uint64_t ksp;
	int64_t kstack_size;

	uint32_t pid;
	uint32_t secondary_id;

	void *cpu_state;

	uint32_t state;
	uint32_t needs_sched;
};

int task_fd_table_get_free_fd_slot(struct fd_table *);

//extern struct task *current_task;
extern obj_mem_cache_t *task_cache;
extern obj_mem_cache_t *procmm_cache;

/*
 *	Create a struct task with kernel stack, procmm, etc
 * */
struct task *create_task(struct task *t, uint32_t flags);
void setup_task_entry(struct task*, void (*)(void*), void *args);

#endif
