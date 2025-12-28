#ifndef _LUCIX_MM_H_
#define _LUCIX_MM_H_
#include <lucix/list.h>
#include <lucix/utils.h>
#include <arch_generic/cpu.h>
#include <stdint.h>

#define	BASE_PHYS_IDENTITY	(0xffff800000000000)
#define	BASE_VM_REGION		(0xffffc00000000000)
#define BASE_KERNEL_VA		(0xffffffff80000000)

#define	MAX_PG_ORDER	(10)
#define	NUM_PG_ORDERS	(MAX_PG_ORDER + 1)

#define	MAX_MEM_ZONES	(64)

/* First 8 bits of the flags uint32_t define the usage */
enum PAGE_USAGE {
	PAGE_USAGE_PRIVATE = 0,
	PAGE_USAGE_CACHE = 1,
	PAGE_USAGE_ANON = 2
};

/* The remaining 24 bits are for flags */
#define PAGE_CACHE_FLAG_DIRTY   (0x100)
#define PAGE_CACHE_COW		(0x200)

struct mem_zone_info {
	uint64_t phys_base;
	uint64_t length;
};

struct cpu_memory_info {
	uint32_t num_mem_zones;
	struct mem_zone_info zones[MAX_MEM_ZONES];
};

struct page_cache_attr {
	struct page_mapping *owner;
};

/*
 *
 *
 *
 * */
struct page {
	struct list_head list;
	uint32_t usage;
	uint32_t flags;
	uint32_t refcnt;
	union {
		struct page_cache_attr page_cache_attr;
	};
};

typedef struct page mem_map_t;
/*
 *
 *
 *
 *
 * */
struct free_area {
	struct list_head list;
	uint32_t* map;
};

enum memzone_flags {
	MEMZONE_NO_FLAGS = 0,
	MEMZONE_IDENTITY_MAPPED = BIT(0),
};

struct memory_zone {
	struct free_area	free_areas[NUM_PG_ORDERS];
	uint64_t		phys_base;
	uint64_t		abs_pfn_base;
	uint64_t		abs_pfn_start;
	uint64_t		abs_num_pages;
	uint64_t		usable_num_pages;

	uint64_t		available_pages;

	uint64_t		max_order;

	uint64_t		bitmap_sizes[MAX_PG_ORDER];
	mem_map_t*		mem_map;
	uint32_t		flags;
};


void mem_init(const struct cpu_memory_info* info);

enum page_alloc_flags {
	PGALLOC_NOFLAGS = 0,
	PGALLOC_KERNEL = BIT(0),
};

struct page* get_page_from_vaddr(void*);
struct page* get_page_from_paddr(uint64_t);
uint64_t get_phys_addr_from_page(struct page*);

struct page* alloc_pages(uint32_t flags, uint32_t order);
void free_pages(struct page* , uint32_t order);

static inline void page_ref(struct page *p)
{
	uint64_t cpu_irq = cpu_irq_save();
	p->refcnt++;
	cpu_irq_restore(cpu_irq);
}

static inline void page_unref(struct page *p)
{
	uint64_t cpu_irq = cpu_irq_save();
	p->refcnt--;
	if (!p->refcnt)
		free_pages(p, 0);
	cpu_irq_restore(cpu_irq);
}

static inline void page_mk_dirty(struct page *p)
{
	uint64_t cpu_irq = cpu_irq_save();
	p->flags |= PAGE_CACHE_FLAG_DIRTY;
	cpu_irq_restore(cpu_irq);
}

static inline void page_mk_clean(struct page *p)
{
	uint64_t cpu_irq = cpu_irq_save();
	p->flags &= ~(PAGE_CACHE_FLAG_DIRTY);
	cpu_irq_restore(cpu_irq);
}

static inline void page_set_usage(struct page *p, uint32_t usage)
{
	p->usage = usage;
}

#endif
