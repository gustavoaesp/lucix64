#ifndef _LUCIX_TASK_H_
#define _LUCIX_TASK_H_

#include <stdint.h>
#include <lucix/slab.h>

enum mm_flags {
	VM_USER	=	0x1,
	VM_EXEC	=	0x2,
	VM_READ =	0x4,
	VM_WRITE=	0x8
};

struct vm_area_operations
{
	int (*open)(struct vm_area*);
	void (*close)(struct vm_area*);
};

struct vm_area
{
	struct list_head list;
	struct procmm* mm;
	// [start, end)
	uint64_t start; // inclusive
	uint64_t end; // exclusive
	struct vm_area_operations vm_ops;
	uint32_t flags;
};

struct procmm
{
	struct list_head vm_areas;

	// code segment
	uint64_t code_start;
	uint64_t code_end;

	// data segment
	uint64_t data_start;
	uint64_t data_end;

	// brk segment
	uint64_t brk_start;
	uint64_t brk_end;

	// stack
	uint64_t stack_start;
	uint64_t stack_end;

	// pgtable (void* to uint64_t*)
	void* pgtable;
};

struct task
{
	struct list_head list;

	struct procmm* mm;

	uint32_t pid;
	uint32_t secondary_id;

	uint64_t kstack_size;
	void *kstack;

	void* cpu_state;
};

void sched_init();

#endif
