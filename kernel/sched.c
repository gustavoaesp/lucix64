#include <exec/elf.h>

#include <lucix/cpu.h>
#include <lucix/task.h>
#include <lucix/mm.h>
#include <lucix/printk.h>
#include <lucix/vma.h>
#include <lucix/cpu.h>
#include <lucix/sched.h>

#include <arch/paging.h>

#include <arch_generic/paging.h>
#include <arch_generic/sched.h>

obj_mem_cache_t *task_cache = NULL;
obj_mem_cache_t *procmm_cache = NULL;
obj_mem_cache_t *vm_area_cache = NULL;

struct task task_init = {};
static uint32_t pid_count = 1;

void sched_init()
{
	task_cache = create_obj_mem_cache(sizeof(struct task), 0, 0, "task-cache");
	vm_area_cache = create_obj_mem_cache(sizeof(struct vm_area), 0, 0, "vma-cache");
	procmm_cache = create_obj_mem_cache(sizeof(struct procmm), 0, 0, "procmm-cache");

}

struct task *mk_kernel_task(void (*__entry)(void*), void *args)
{
	struct task *t = create_task(NULL, TASK_USER);

	cpu_ktask_setup(t, __entry, args);
	t->ksp -= sizeof(uint64_t);
	*((uint64_t*)t->ksp) = (uintptr_t)__entry;
	/* ugly hacky crap that needs to be abstracted to x86_64 arch folder */
	t->ksp -= sizeof(uint64_t) * 6;

	return t;
}

void sched_per_cpu_init()
{
	struct cpu *cpu = cpu_get_cpu();
	struct task *t = mk_kernel_task(cpu_idle, NULL);
	cpu->idle = t;
	t->ksp = (uintptr_t)cpu_idle;
	cpu->current = NULL;

	INIT_LIST_HEAD(&cpu->runqueue.tasks);
}



void sched_irq()
{
	struct cpu *cpu = cpu_get_cpu();
	/* This means the scheduler is not initialized */
	if (!procmm_cache || !task_cache || !vm_area_cache) {
		return;
	}

	if (cpu->current) {
		cpu->current->needs_sched = 1;
	} else if (list_empty(&cpu->runqueue.tasks)) {
		return;
	}
}

int sched_add_task(struct task *task)
{
	/* TODO at some point a task would be assigned to different
	 * cpu's in SMP, add logic for this in here */
	struct cpu *cpu = cpu_get_cpu();
	uint64_t irq_state;

	irq_state = cpu_irq_save();
	{
		list_add_tail(&task->qlist, &cpu->runqueue.tasks);
	}
	cpu_irq_restore(irq_state);
	return 0;
}

void sched_start()
{
	struct cpu *cpu = cpu_get_cpu();
	if(list_empty(&cpu->runqueue.tasks)) {
		/* WTF */
	}
	cpu->current = container_of(cpu->runqueue.tasks.next, struct task, qlist);
	cpu_mm_set_pgtable(cpu->current->mm->pgtable);
	cpu_context_switch(NULL);
}

static struct task *sched_pick_next_task()
{
	struct cpu *cpu = cpu_get_cpu();
	struct task *next = NULL;

	if (!cpu->current && list_empty(&cpu->runqueue.tasks)) {
		return NULL;
	}

	if (!cpu->current || cpu->current->qlist.next == &cpu->runqueue.tasks) {
		next = container_of(cpu->runqueue.tasks.next, struct task, qlist);
	} else {
		next = container_of(cpu->current->qlist.next, struct task, qlist);
	}

	return next;
}

void schedule()
{
	struct cpu *cpu = cpu_get_cpu();
	struct task *next = sched_pick_next_task();
	struct task *prev = (cpu->current) ? cpu->current : cpu->idle;

	if (cpu->current) {
		cpu->current->needs_sched = 0;
	}

	if (!next) {
		next = cpu->idle;
	}

	if (next == prev) {
		return;
	}

	if (next->mm) {
		cpu_mm_set_pgtable(next->mm->pgtable);
	}

	cpu->current = next;
	cpu_schedule(prev, next, cpu);
	cpu_switch_to(prev, next);
}
