#include <arch/paging.h>
#include <arch_generic/paging.h>

#include <lucix/mm.h>
#include <lucix/page_fault.h>
#include <lucix/task.h>
#include <lucix/vma.h>
#include <lucix/printk.h>

static int anonymous_page_handler(struct vm_fault *vm_fault)
{
	struct page *new_page = NULL;

	/* Let's check if the page is present but protection violation
	 * was the fault cause */
	if (vm_fault->flags & PGFAULT_PROTECTION) {
		/* fault was caused by a write even though we have write allowed */
		if (vm_fault->flags & PGFAULT_WRITE) {
			uint64_t page_phys = cpu_mm_get_phys_address(
				vm_fault->vma->procmm,
				vm_fault->addr
			);
			struct page *page = NULL;
			/* Tried to write to a RO page */
			if (!(vm_fault->vma->prot & VM_MAYWRITE) && !(vm_fault->vma->prot & VM_WRITE)) {
				return 1;
			}
			if (page_phys & 1) {
				/* WHAT THE FUCK */
				printf("WHAT THE FUCK\n");
				for(;;) asm volatile ("hlt");
			}
			page = get_page_from_paddr(page_phys);
			if (!page) {
				/* WHAT THE FUCK */
				printf("WHAT THE FUCK\n");
				for (;;) asm volatile ("hlt");
			}
			/* this implies a copy on write */
			if (vm_fault->vma->prot & VM_MAYWRITE) {
				struct page *new_page;/* = alloc_pages(PGALLOC_KERNEL, 0);*/
				if (page->refcnt > 1) {
					new_page = alloc_pages(PGALLOC_KERNEL, 0);
					memcpy(new_page->vaddr, page->vaddr, PAGE_SIZE);
					page_ref(new_page);
					page_unref(page);
					vm_fault->new_page = new_page;
				} else {
					/* we are the only ref to this page, keep it and just remap it */
					vm_fault->new_page = page;
				}
				return 0;
			}
		} else { /* read was tried */
			/* tried to execute non-executable memory */
			if ((vm_fault->flags & PGFAULT_INSTRUCTION) && !(vm_fault->vma->prot & VM_EXEC)) {
				return 1;
			}
			return 1;
		}
	} else { /* page not present */
		struct page *new_page = alloc_pages(PGALLOC_KERNEL, 0);

		vm_fault->new_page = new_page;
		page_ref(new_page);
	}

	return 0;
}

int kernel_page_fault(uintptr_t addr, uint32_t flags, uintptr_t pc)
{
	struct vm_area *vma = NULL;
	struct vm_fault vm_fault = {
		.addr = addr,
		.flags = flags,
	};

	vma = find_vma_for_address(addr, current_task->mm);

	if (!vma) {
		/* TODO implement a segfault handler */
		printf("SEGFAULT:\nrip: %p\naddr: %p\nflags: %p\n", pc, addr, flags);
		for(;;) asm volatile ("hlt");
	}

	vm_fault.vma = vma;
	vm_fault.pgoffset = ((int64_t)addr - vma->start) / PAGE_SIZE;

	if (vma->vm_ops && vma->vm_ops->fault) {
		if(vma->vm_ops->fault(vma, &vm_fault)) {
			printf("SIGBUS\n");
			for(;;) asm volatile ("hlt");
		}
	} else {
		if (anonymous_page_handler(&vm_fault)) {
			printf("SEGFAULT\n");
			for(;;) asm volatile ("hlt");
		}
	}

	if (vm_fault.new_page) {
		uint32_t prot = vma->prot;

		if ((vm_fault.flags & PGFAULT_PROTECTION)
			&& (vm_fault.flags & PGFAULT_WRITE)
			&& (vm_fault.vma->prot & VM_MAYWRITE))
		{
			prot |= VM_WRITE;
		}

		cpu_mm_map_page(
			vma->procmm->pgtable,
			get_phys_addr_from_page(vm_fault.new_page),
			PAGE_FRAME(addr),
			prot
		);
	} else if (vm_fault.pfn) {
		cpu_mm_map_page(
			vma->procmm->pgtable,
			(vm_fault.pfn * PAGE_SIZE),
			PAGE_FRAME(addr),
			vma->prot
		);
	}

	return 0;
}
