#include <lucix/vma.h>
#include <lucix/task.h>
#include <lucix/fs/file.h>

int try_join(struct procmm *procmm)
{
	struct list_head *pos = NULL;
	struct list_head *n = NULL;

	list_for_each_safe(pos, n, &procmm->vm_areas) {
		if(pos->next == &procmm->vm_areas) break;

		struct vm_area *current = (struct vm_area *)pos;
		struct vm_area *next = (struct vm_area *)pos->next;

		if (current->end == next->start) {
			if (current->file == next->file
				&& current->flags == next->flags
				&& current->prot == next->prot
				&& current->vm_ops == next->vm_ops
			) {
				current->end = next->end;
				if (next->file) file_unref(next->file);
				list_del(&next->list);
				mem_cache_free_obj(vm_area_cache, next);
			}
		}
	}
	return 0;
}

int insert_vma(struct procmm *procmm, struct vm_area *vma)
{
	struct list_head *pos = NULL;
	int insertion_happened = 0;

	if (list_empty(&procmm->vm_areas)) {
		list_add(&vma->list, &procmm->vm_areas);
		return 0;
	}

	list_for_each(pos, &procmm->vm_areas) {
		struct vm_area *area = (struct vm_area *)pos;
		if (vma->start < area->start) {
			list_add_tail(&vma->list, &area->list);
			insertion_happened = 1;
			break;
		}
	}

	if (!insertion_happened) {
		list_add_tail(&vma->list, &procmm->vm_areas);
	}

	try_join(procmm);

	return 0;
}
