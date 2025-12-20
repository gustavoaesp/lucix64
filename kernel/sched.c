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
		cpu->current->task->needs_sched = 1;
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
	/* TODO maybe have a special cache for this */
	runqueue_entry_t *entry = kmalloc(sizeof(runqueue_entry_t), 0);

	entry->task = task;
	irq_state = cpu_irq_save();
	{
		list_add_tail(&entry->list, &cpu->runqueue.tasks);
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
	cpu->current = (runqueue_entry_t*)cpu->runqueue.tasks.next;
	cpu_mm_set_pgtable(cpu->current->task->mm->pgtable);
	cpu_context_switch(NULL);
}

static runqueue_entry_t *sched_pick_next_task()
{
	struct cpu *cpu = cpu_get_cpu();
	runqueue_entry_t *current_entry = cpu->current;
	runqueue_entry_t *next = NULL;
	uint64_t irq_state = cpu_irq_save();

	if (!current_entry && list_empty(&cpu->runqueue.tasks)) {
		cpu_irq_restore(irq_state);
		return NULL;
	}

	if (!current_entry) {
		next = (runqueue_entry_t*)cpu->runqueue.tasks.next;
	} else {
		next = (runqueue_entry_t*)current_entry->list.next;
	}
	if (&next->list == &cpu->runqueue.tasks) {
		next = (runqueue_entry_t*)cpu->runqueue.tasks.next;
	}

	cpu_irq_restore(irq_state);
	return next;
}

void schedule()
{
	struct cpu *cpu = cpu_get_cpu();
	runqueue_entry_t *next = sched_pick_next_task();
	struct task *next_task = NULL;
	struct task *prev_task = NULL;

	next_task = (next) ? next->task : cpu->idle;
	prev_task = (cpu->current) ? cpu->current->task : cpu->idle;

	if (next_task == prev_task) {
		return;
	}

	if (next_task->mm) {
		cpu_mm_set_pgtable(next->task->mm->pgtable);
	}

	cpu->current = next;
	cpu_schedule(prev_task, next_task, cpu);
	cpu_switch_to(prev_task, next_task);
}
