#include <exec/elf.h>

#include <lucix/task.h>
#include <lucix/mm.h>
#include <lucix/printk.h>

#include <arch/paging.h>

#include <arch_generic/paging.h>
#include <arch_generic/sched.h>

static obj_mem_cache_t *task_cache = NULL;
static obj_mem_cache_t *vm_area_cache = NULL;
static obj_mem_cache_t *procmm_cache = NULL;

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

static void copy_vma_pages(const void* from, struct vma_area_operations *to, int64_t size)
{
}

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
	t->kstack_top = t->kstack + t->kstack_size;
	t->ksp = t->kstack_top;

	t->mm = mem_cache_alloc_obj(procmm_cache);
	t->mm->pgtable = mm_create_page_table();

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

/*int __map_elf_header(struct procmm *mm, void *elf, struct elf_header_entry_64 *header_entry)
{
	uint32_t offset = 0; 
	uint32_t vm_flags = VM_USER;
	vm_flags |= (header_entry->flags & elf_segment_executable) ? VM_EXEC : 0;
	vm_flags |= (header_entry->flags & elf_segment_readable) ? VM_READ : 0;
	vm_flags |= (header_entry->flags & elf_segment_writable) ? VM_WRITE : 0;

	while (offset < header_entry->size_in_mem) {
		struct page *p = alloc_pages(PGALLOC_KERNEL, 0);
		int32_t copy_bytes = (header_entry->size_in_mem - offset < PAGE_SIZE) ?
			header_entry->size_in_mem - offset : PAGE_SIZE;

		mm_map_page(
			mm->pgtable,
			VA2PA(p->vaddr),
			header_entry->p_vaddr + offset,
			vm_flags
		);

		memcpy(p->vaddr, elf + header_entry->offset_data, copy_bytes);

		offset += copy_bytes;
	}

	return 0;
}*/

/*
void task_exec(struct task *t, void *elf, uint64_t size)
{
	struct elf_header_64 *header = elf;

	if (!is_valid_elf(elf, size)) {
		return;
	}

	if (t->mm) {
		mem_cache_free_obj(procmm_cache, t->mm);
		t->mm = mem_cache_alloc_obj(procmm_cache);
	}

	struct elf_header_entry_64 *header_table = elf + header->program_header_table_pos;
	for (int i = 0; i < header->num_program_header_entries; ++i) {
		__map_elf_header(t->mm, elf, header_table + i);
	}

	/* stack setup *
	struct page *stack_p = alloc_pages(PGALLOC_KERNEL, 0);
	if (!stack_p) {
		return;
	}
	mm_map_page(
		t->mm->pgtable,
		VA2PA(stack_p->vaddr),
		0x00007ffffffff000,
		VM_READ | VM_USER | VM_EXEC
	);

	/* Zero out the kernel stack*
	/*memset(t->kstack, 0, PAGE_SIZE << KSTACK_PGORDER);*

	t->entry = header->entry;

	cpu_task_setup(t, NULL);
	list_add(&t->list, &task_list);
}*/

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
		current_task = task_list.next;
	}

	mm_set_pgtable(current_task->mm->pgtable);
	cpu_context_switch();
	/* should never reach here */
}