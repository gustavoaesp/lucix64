#include <lucix/vma.h>
#include <lucix/task.h>
#include <arch/paging.h>

/* Technically the limit would be 0xfff800000000000 but the other last bits are for
 * cannonical kernel addresses (the last bit in 52 is extended all the way to 64 */
#define X86_LIMIT 	0x800000000000
#define X86_LOW_LIMIT 	0x400000

int cpu_assign_vma_vaddr(struct procmm *procmm, struct vm_area *vma, size_t len)
{
	struct list_head *pos = NULL;

	list_for_each(pos, &procmm->vm_areas) {
		struct vm_area *area = (struct vm_area *)pos;
		struct vm_area *prev = (struct vm_area *)pos->prev;
		struct vm_area *next = (struct vm_area *)pos->next;

		size_t prev_empty_start = (pos->prev == &procmm->vm_areas) ? X86_LOW_LIMIT : prev->end;
		size_t prev_empty_end = area->start;

		size_t next_empty_start = area->end;
		size_t next_empty_end = (pos->next == &procmm->vm_areas) ? X86_LIMIT : next->start;


		if (prev_empty_end - prev_empty_start > len + (PAGE_SIZE * 2)) {
			vma->start = prev_empty_start + PAGE_SIZE;
			vma->end = vma->start + len;
			return 0;
		}
		if (next_empty_end - next_empty_start > len + (PAGE_SIZE * 2)) {
			vma->start = next_empty_start + PAGE_SIZE;
			vma->end = vma->start + len;
			return 0;
		}
	}

	return 1;
}
