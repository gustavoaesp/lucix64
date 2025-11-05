#ifndef _LUCIX_ARCH_GENERIC_PAGING_H_
#define _LUCIX_ARCH_GENERIC_PAGING_H_

#include <stdint.h>

/*
*   This will create an empty page table that includes all the kernel mappings.
*/
void *cpu_mm_create_page_table();

/*
*   This will update pgtable to include the specified mapped page
*/
void cpu_mm_map_page(void *pgtable, uint64_t phys_addr, uint64_t vaddr, uint32_t vm_flags);

void cpu_mm_set_pgtable(void *pgtable);

#endif
