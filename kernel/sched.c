#include <exec/elf.h>

#include <lucix/task.h>
#include <lucix/mm.h>
#include <lucix/printk.h>
#include <lucix/vma.h>

#include <arch/paging.h>

#include <arch_generic/paging.h>
#include <arch_generic/sched.h>

static obj_mem_cache_t *task_cache = NULL;
static obj_mem_cache_t *procmm_cache = NULL;

obj_mem_cache_t *vm_area_cache = NULL;

static uint32_t pid_count = 1;

static struct list_head task_list;

struct task *current_task = NULL;

void sched_init()
{
	task_cache = create_obj_mem_cache(sizeof(struct task), 0, 0, "task-cache");
	vm_area_cache = create_obj_mem_cache(sizeof(struct vm_area), 0, 0, "vma-cache");
	procmm_cache = create_obj_mem_cache(sizeof(struct procmm), 0, 0, "procmm-cache");

	INIT_LIST_HEAD(&task_list);
}

/*static void copy_vma_pages(const void* from, struct vma_area_operations *to, int64_t size)
{
}*/

static struct task *create_task()
{
	struct task *t = mem_cache_alloc_obj(task_cache);
	if (!t) {
		return t;
	}

	t->cpu_state = NULL;
	struct page *p = alloc_pages(PGALLOC_KERNEL, 1);
	t->kstack = p->vaddr;
	t->kstack_size = PAGE_SIZE << 1;
	/*t->kstack_top = t->kstack + t->kstack_size;*/
	t->ksp = (uint64_t)t->kstack + t->kstack_size;

	t->mm = mem_cache_alloc_obj(procmm_cache);
	t->mm->pgtable = cpu_mm_create_page_table();
	t->fd_table = kmalloc(sizeof(struct fd_table), 0);
	t->fd_table->fd = kmalloc(sizeof(struct file*) * 32, 0);
	t->fd_table->table_size = 32;
	memset(t->fd_table->fd, 0, sizeof(struct file *) * 32);
	INIT_LIST_HEAD(&t->mm->vm_areas);

	return t;
}

void sched_mk_kernel_task(void (*__entry)(void*), void *args)
{
	struct task *t = create_task();

	cpu_ktask_setup(t, __entry, args);
	/*if (!current_task) {
		printf("current_task was %p, setting to %p\n", current_task, t);
		current_task = t;
	}*/
	/*list_add(&t->list, &task_list);*/
	list_add_tail(&t->list, &task_list);
}

void sched_irq()
{
	if (!procmm_cache || !task_cache || !vm_area_cache) {
		return;
	}

	if (list_empty(&task_list)) {
		return;
	}

	if (current_task) {
		current_task = (struct task*)current_task->list.next;
	} else {
		current_task = (struct task*)task_list.next;
	}

	/* we reached all tasks, restart from the beginning */
	if (((uint64_t)current_task) == ((uint64_t)&task_list)) {
		current_task = (struct task*)task_list.next;
	}

	cpu_mm_set_pgtable(current_task->mm->pgtable);
	cpu_context_switch(NULL);
	/* should never reach here */
}
