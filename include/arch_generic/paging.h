#ifndef _LUCIX_ARCH_GENERIC_PAGING_H_
#define _LUCIX_ARCH_GENERIC_PAGING_H_

#include <stdint.h>
#include <lucix/vma.h>

/*
*   This will create an empty page table that includes all the kernel mappings.
*/
void *cpu_mm_create_page_table();

/*
*   This will update pgtable to include the specified mapped page
*/
void cpu_mm_map_page(void *pgtable, uint64_t phys_addr, uint64_t vaddr, uint32_t vm_flags);

/*
 *  This will get the phys address of a virtual address from a given vma
 * */
uint64_t cpu_mm_get_phys_address(struct procmm *, uintptr_t vaddr);

void cpu_mm_set_pgtable(void *pgtable);

#endif
