#include <arch/paging.h>
#include <lucix/mm.h>
#include <lucix/vm.h>
#include <lucix/task.h>

uint64_t g_kernel_phys_base = 0;
uint64_t g_kernel_reserved_size = 0;

static uint64_t*	kernel_pgtable;

static void set_cr3(uint64_t pml4_phys)
{
	asm volatile ("mov %rdi, %cr3");
}

static uint64_t get_pg_flags_pdpt(uint64_t vmflags)
{
	uint64_t res = 0;

	res |= (vmflags & VM_USER) ? PDPT_USER : 0;
	res |= (vmflags & VM_WRITE)? PDPT_RW_FLAG : 0;
	res |= (!(vmflags & VM_EXEC)) ? : PDET_XD;

	return res;
}

static uint64_t get_pg_flags_pdet(uint64_t vmflags)
{
	uint64_t res = 0;

	res |= (vmflags & VM_USER) ? PDET_USER : 0;
	res |= (vmflags & VM_WRITE)? PDET_RW_FLAG : 0;
	res |= (!(vmflags & VM_EXEC)) ? : PDET_XD;

	return res;
}

static uint64_t get_pg_flags_pte(uint64_t vmflags)
{
	uint64_t res = 0;

	res |= (vmflags & VM_USER) ? PTE_USER : 0;
	res |= (vmflags & VM_WRITE)? PTE_RW_FLAG : 0;
	res |= (!(vmflags & VM_EXEC)) ? : PTE_XD;

	return res;
}

static void map_contiguous(void* pgtable, uint64_t phys_addr, void* vaddr, uint64_t size, uint32_t _2mb, uint64_t vmflags)
{
	uint64_t *pml4 = pgtable;
	uint64_t pgsize = (_2mb) ? 0x200000 : PAGE_SIZE;
	uint64_t num_pages = size / pgsize + ((size % pgsize) ? 1 : 0);
	uint64_t page_count = 0;
	uint64_t pte_flags = get_pg_flags_pte(vmflags);
	uint64_t pdet_flags = get_pg_flags_pdet(vmflags);

	uint64_t pml4_idx = PML4_FROM_VA(vaddr);

	int pdpti = PDPT_FROM_VA(vaddr);
	int pdeti = PDET_FROM_VA(vaddr);
	int ptei = PTE_FROM_VA(vaddr);

	for (int pml4i = pml4_idx; pml4i <= (PML4_FROM_VA(size)) + pml4_idx; ++pml4i) {
		uint64_t* pdpt = NULL;
		if (!(pml4[pml4i] & PML4_PRESENT_FLAG)) {
			pdpt = alloc_pages(PGALLOC_KERNEL, 0)->vaddr;
			pml4[pml4i] = PAGE_FRAME(VA2PA(pdpt))
				| PML4_PRESENT_FLAG | PML4_RW_FLAG | PML4_USER;
		} else {
			pdpt = PA2VA(PAGE_FRAME(pml4[pml4i]));
		}
		for (; pdpti < 512 && page_count < num_pages; pdpti++) {
			uint64_t *pdet = NULL;
			if (!(pdpt[pdpti] & PDPT_PRESENT_FLAG)) {
				pdet = alloc_pages(PGALLOC_KERNEL, 0)->vaddr;
				pdpt[pdpti] = PAGE_FRAME(VA2PA(pdet))
					| PDPT_PRESENT_FLAG | PDPT_RW_FLAG | PDPT_USER;
			} else {
				pdet = PA2VA(PAGE_FRAME(pdpt[pdpti]));
			}
			for (;pdeti < 512 && page_count < num_pages; pdeti++) {
				uint64_t* pte = NULL;
				if (_2mb) {
					pdet[pdeti] = PAGE_FRAME(phys_addr + page_count*pgsize)
						| PDET_PRESENT_FLAG | PDET_RW_FLAG | PDET_PAGE_SIZE | PDET_USER ;
					page_count++;
					continue;
				}
				if (!(pdet[pdeti] & PDET_PRESENT_FLAG)) {
					pte = alloc_pages(PGALLOC_KERNEL, 0)->vaddr;
					pdet[pdeti] = PAGE_FRAME(VA2PA(pte))
						| PDET_PRESENT_FLAG | PDET_RW_FLAG | PDET_USER;
				} else {
					// in this case we should modify the page flags if it already exists
					// in the page table.
					pte = PA2VA(PAGE_FRAME(pdet[pdeti]));
					//pte = PAGE_FRAME(pte) | pte_flags;
				}

				for (;ptei < 512 && page_count < num_pages; ptei++, page_count++) {
					pte[ptei] = PAGE_FRAME(phys_addr + page_count * pgsize)\
						| PTE_PRESENT_FLAG | PTE_RW_FLAG;
					if (vmflags & VM_USER) {
						pte[ptei] |= PTE_USER;
					}
				}
				ptei = 0;
			}
			pdeti = 0;
		}
		pdpti = 0;
	}
}

void* x86_64_vm_map_pages(void* pgtable, struct page **pages, uint64_t vaddr_start, uint64_t num_pages, uint64_t vmflags)
{
	for (int p = 0; p < num_pages; ++p) {
		uint64_t phys_addr = get_phys_addr_from_page(pages[p]);
	}
}

void paging_init()
{
	kernel_pgtable = alloc_pages(PGALLOC_KERNEL, 0)->vaddr;
	// MAP THE FIRST 4GB of physical memory
	map_contiguous(kernel_pgtable, 0, (void*)BASE_PHYS_IDENTITY, 0x10000000000, 1, VM_WRITE | VM_READ | VM_EXEC);
	// MAP THE KERNEL EXECUTABLE
	map_contiguous(kernel_pgtable, g_kernel_phys_base, (void*)BASE_KERNEL_VA, g_kernel_reserved_size, 0, VM_WRITE | VM_READ | VM_EXEC);

	set_cr3(VA2PA(kernel_pgtable));
}

void *map_vm_zone(struct vm_zone *zone, uint32_t flags)
{
	map_contiguous(kernel_pgtable, zone->phys_addr, (void*)zone->start_addr, zone->size, 0, VM_READ | VM_WRITE);

	set_cr3(VA2PA(kernel_pgtable));
	return (void*)zone->start_addr;
}

void *mm_create_page_table()
{
	void *pgtable = alloc_pages(PGALLOC_KERNEL, 0)->vaddr;
	memcpy(pgtable, kernel_pgtable, PAGE_SIZE);

	return pgtable;
}

void mm_map_page(void *pgtable, uint64_t phys_addr, uint64_t vaddr, uint32_t vm_flags)
{
	map_contiguous(pgtable, phys_addr, vaddr, PAGE_SIZE, 0, vm_flags);
}

void mm_set_pgtable(void *pgtable)
{
	set_cr3(VA2PA((uint64_t)pgtable));
}