#ifndef _ARCH_PAGING_H_
#define _ARCH_PAGING_H_
#include <lucix/utils.h>
#include <lucix/vm.h>

#define	PML4_PRESENT_FLAG	BIT(0)
#define PML4_RW_FLAG		BIT(1)
#define PML4_USER		BIT(2)
#define PML4_WRITE_THROUGH	BIT(3)
#define PML4_CACHE_DISABLE	BIT(4)
#define PML4_XD			BIT(63)

#define	PDPT_PRESENT_FLAG	BIT(0)
#define PDPT_RW_FLAG		BIT(1)
#define PDPT_USER		BIT(2)
#define PDPT_WRITE_THROUGH	BIT(3)
#define PDPT_CACHE_DISABLE	BIT(4)
#define PDPT_PAGE_SIZE		BIT(7)
#define PDPT_XD			BIT(63)

#define	PDET_PRESENT_FLAG	BIT(0)
#define PDET_RW_FLAG		BIT(1)
#define PDET_USER		BIT(2)
#define PDET_WRITE_THROUGH	BIT(3)
#define PDET_CACHE_DISABLE	BIT(4)
#define PDET_PAGE_SIZE		BIT(7)
#define PDET_XD			BIT(63)

#define	PTE_PRESENT_FLAG	BIT(0)
#define PTE_RW_FLAG		BIT(1)
#define PTE_USER		BIT(2)
#define PTE_WRITE_THROUGH	BIT(3)
#define PTE_CACHE_DISABLE	BIT(4)
#define PTE_XD			BIT(63)

#define	PML4_FROM_VA(va)	((((uint64_t)va) & 0x0000ff8000000000) >> 39)
#define	PDPT_FROM_VA(va)	((((uint64_t)va) & 0x0000007fc0000000) >> 30)
#define	PDET_FROM_VA(va)	((((uint64_t)va) & 0x000000003fe00000) >> 21)
#define	PTE_FROM_VA(va)		((((uint64_t)va) & 0x00000000001ff000) >> 12)


#define	PAGE_FRAME(pa)		(((uint64_t)pa) & 0x0000fffffffff000)
#define	PAGE_FRAME_BIG(pa)	(((uint64_t)pa) & 0x0000ffffffe00000)

#define VA2PA(va)		(((uint64_t)va) ^ 0xffff800000000000)
#define PA2VA(pa)		((void*)(pa ^ 0xffff800000000000))

#define	PAGE_SIZE		(0x1000)
#define	PAGE_SIZE_BIG		(0x200000)

extern uint64_t g_kernel_phys_base;
extern uint64_t g_kernel_reserved_size;

void paging_init();

void *map_vm_zone(struct vm_zone *zone, uint32_t flags);

#endif
