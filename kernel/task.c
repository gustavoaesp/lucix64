#include <lucix/mm.h>
#include <lucix/task.h>
#include <lucix/utils.h>
#include <arch_generic/paging.h>

#include <arch/paging.h>

struct task *create_task(struct task *t, uint32_t flags)
{
	if (!t) {
		t = mem_cache_alloc_obj(task_cache);
	}
	if (!t) {
		return t;
	}

	INIT_LIST_HEAD(&t->qlist);
	INIT_LIST_HEAD(&t->list);
	INIT_LIST_HEAD(&t->children);

	t->cpu_state = NULL;
	struct page *p = alloc_pages(PGALLOC_KERNEL, 1);
	t->kstack = p->vaddr;
	t->kstack_size = PAGE_SIZE << 1;
	t->ksp = (uint64_t)t->kstack + t->kstack_size;

	t->needs_sched = 0;
	if (flags == TASK_USER) {
		t->mm = mem_cache_alloc_obj(procmm_cache);
		t->mm->pgtable = cpu_mm_create_page_table();
		INIT_LIST_HEAD(&t->mm->vm_areas);
	}
	t->fd_table = kmalloc(sizeof(struct fd_table), 0);
	t->fd_table->fd = kmalloc(sizeof(struct file*) * 32, 0);
	t->fd_table->table_size = 32;
	memset(t->fd_table->fd, 0, sizeof(struct file *) * 32);

	return t;
}

void task_add_child(struct task *t, struct task *child)
{
	list_add(&child->list, &t->children);
	child->parent = t;
}
