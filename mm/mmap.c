#include <lucix/fs/file.h>
#include <lucix/fs/mapping.h>
#include <lucix/vma.h>
#include <lucix/task.h>
#include <lucix/printk.h>
#include <arch/paging.h>
#include <arch_generic/paging.h>
#include <lucix/mm.h>

static int is_valid_vaddr_for_vma(struct procmm *procmm, uintptr_t addr, uint64_t length)
{
	struct list_head *pos = NULL;
	if (list_empty(&procmm->vm_areas)) {
		return 1;
	}

	list_for_each(pos, &procmm->vm_areas) {
		struct vm_area *area = (struct vm_area*)pos;
		/* this is inside the area we are trying to get */
		if (area->start >= addr && area->end <= addr + length) {
			return 0;
		}
		/* overlaps */
		if (addr >= area->start && addr < area->end)
			return 0;
		if (addr + length > area->start && addr + length < area->end)
			return 0;
	}

	return 1;
}

int vma_populate(struct vm_area *vma)
{
	for (int i = 0; i < vma->length / PAGE_SIZE; ++i) {
		uint64_t vaddr = vma->start + i*PAGE_SIZE;
		uint64_t phys = 0;
		struct page *page = NULL;
		if (vma->flags & VM_FLAG_ANON) {
			page = alloc_pages(PGALLOC_KERNEL, 0);
			phys = get_phys_addr_from_page(page);
		} else {
			struct inode * ino = vma->file->inode;
			page = ino->f_map->pages[i + vma->fpg_off];
			if (!page) {
				page = alloc_pages(PGALLOC_KERNEL, 0);
				page->flags = PAGE_USAGE_CACHE;
				page->page_cache_attr.owner = ino->f_map;
				ino->f_map->ops->readpage(vma->file, ino->f_map, page, vma->fpg_off * PAGE_SIZE);
				ino->f_map->pages[i + vma->fpg_off] = page;
			}
		}
		page_ref(page);
		cpu_mm_map_page(
			vma->procmm->pgtable,
			phys,
			PAGE_FRAME(vaddr),
			vma->prot
		);
	}

	return 0;
}

uintptr_t do_mmap(
	struct procmm *procmm,
	struct file *file,
	uintptr_t addr,
	uint64_t length,
	uint32_t prot,
	uint32_t flags,
	uint32_t type,
	uint32_t pgoff)
{
	struct vm_area *vm_area = NULL;
	size_t num_pages = length / PAGE_SIZE;

	num_pages += (length % PAGE_SIZE) ? 1 : 0;

	vm_area = mem_cache_alloc_obj(vm_area_cache);

	if (addr && is_valid_vaddr_for_vma(procmm, addr, num_pages * PAGE_SIZE)) {
		vm_area->start = addr;
		vm_area->end = addr + (num_pages * PAGE_SIZE);
	} else {
		/* TODO find suitable area for a vm_area */
		cpu_assign_vma_vaddr(procmm, vm_area, num_pages * PAGE_SIZE);
	}

	vm_area->procmm = procmm;
	vm_area->prot = prot;
	vm_area->flags = flags;
	vm_area->length = length;
	vm_area->file = file;
	vm_area->type = type;

	if (vm_area->prot & VM_WRITE) {
		if (vm_area->flags & VM_FLAG_PRIVATE) {
			vm_area->prot &= ~(VM_WRITE);
			vm_area->prot |= VM_MAYWRITE;
		}
	}

	/* only used when the vm_area is a file mmap */
	vm_area->fpg_off = pgoff;

	if (file) {
		file_ref(file);
		if (file->ops && file->ops->mmap) {
			file->ops->mmap(file, vm_area);
		}
	}

	insert_vma(procmm, vm_area);

	if (flags & VM_FLAG_POPULATE) {
		vma_populate(vm_area);
	}
	return (uintptr_t)vm_area->start;
}
