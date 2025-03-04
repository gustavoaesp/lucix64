#include <stdint.h>
#include <arch/paging.h>
#include <lucix/list.h>
#include <lucix/mm.h>

struct alloc_state {
	uint64_t num_zones;
	struct memory_zone zones[MAX_MEM_ZONES];

	uint32_t biggest_mapped_idx;

	uint64_t total_system_memory;
	uint64_t total_system_memory_pages;

	uint64_t allocator_maintenance_memory;
	uint64_t total_maintenance_used;
};

static struct alloc_state state = {
};

uint32_t __get_max_order(uint32_t usable_pages)
{
	for (int i = 0; i < NUM_PG_ORDERS; ++i) {
		/* right-shift makes it zero? */
		if (!(usable_pages >> i)) {
			if (usable_pages % (1 << i)) {
				return i - 1;
			}
			return i;
		}
	}

	return MAX_PG_ORDER;
}

void __init_memmap(struct memory_zone* zone)
{
	for (int page = 0; page < zone->usable_num_pages; ++page) {
		INIT_LIST_HEAD(&zone->mem_map[page].list);
		zone->mem_map[page].flags = 0;

		if (zone->flags & MEMZONE_IDENTITY_MAPPED) {
			zone->mem_map[page].vaddr = PA2VA(
				zone->phys_base
				+ (page + (zone->abs_pfn_start - zone->abs_pfn_base)) * PAGE_SIZE
			);
		}
	}
}


uint32_t __toggle_bit_pfn_zone(uint64_t pfn, struct memory_zone *zone, uint32_t order)
{
	uint64_t total_bit_offset = pfn >> (order + 1);
	uint64_t bitmap_offset = total_bit_offset / (sizeof(uint32_t) * 8);
	uint32_t bit_offset = total_bit_offset % (sizeof(uint32_t) * 8);

	uint32_t* map = zone->free_areas[order].map;

	map[bitmap_offset] ^= BIT(bit_offset);
	return map[bitmap_offset] & BIT(bit_offset);
}


struct memory_zone* get_zone_and_pfn_from_page(struct page* p, uint64_t* pfn)
{
	uint64_t page_struct_phys = VA2PA(p);

	for (int z = 0; z < state.num_zones; ++z) {
		struct memory_zone* zone = state.zones + z;
		uint64_t zone_mem_map_phys = VA2PA(zone->mem_map);
		uint64_t mem_map_size = sizeof(mem_map_t) * zone->usable_num_pages;

		if (page_struct_phys >= zone_mem_map_phys &&
			page_struct_phys < zone_mem_map_phys + mem_map_size)
		{
			*pfn = (page_struct_phys - zone_mem_map_phys) / sizeof(mem_map_t);
			return zone;
		}
	}

	return NULL;
}

uint64_t get_phys_addr_from_page(struct page* p)
{
	uint64_t pfn;
	struct memory_zone* memzone = get_zone_and_pfn_from_page(p, &pfn);
	uint64_t base_phys = memzone->abs_pfn_start;

	base_phys += pfn;
	return base_phys * PAGE_SIZE;
};


uint32_t __toggle_bit_page(struct page* p, uint32_t order)
{
	uint64_t pfn = 0;
	struct memory_zone* zone = NULL;

	zone = get_zone_and_pfn_from_page(p, &pfn);

	return __toggle_bit_pfn_zone(pfn, zone, order);
}


void __initialize_offsets(struct memory_zone* zone)
{
	void *biggest_base = PA2VA(state.zones[state.biggest_mapped_idx].phys_base);

	zone->max_order = __get_max_order(zone->usable_num_pages);


	// Truncate leftovers
	zone->usable_num_pages >>= zone->max_order;
	zone->usable_num_pages <<= zone->max_order;

	zone->mem_map = (((void*)biggest_base) + state.total_maintenance_used);
	state.total_maintenance_used += sizeof(mem_map_t) * zone->usable_num_pages;

	uint32_t* bitmap_offset = (((void*)biggest_base) + state.total_maintenance_used);
	for (int order = 0; order <= zone->max_order; ++order) {
		uint32_t toadd = 0;

		if (order < MAX_PG_ORDER) {
			zone->free_areas[order].map = bitmap_offset;

			toadd = zone->bitmap_sizes[order] / 4;
			bitmap_offset += toadd;
			state.total_maintenance_used += toadd * 4;
		}
	}

	__init_memmap(zone);
}

void __init_maintenance_zone(struct memory_zone* zone)
{
	uint64_t length = zone->abs_num_pages * PAGE_SIZE;
	uint64_t maintenance_memory_in_pages;
	uint64_t extra_page = (state.allocator_maintenance_memory % PAGE_SIZE != 0);

	length -= state.allocator_maintenance_memory;
	zone->abs_pfn_start = (state.allocator_maintenance_memory) / PAGE_SIZE + extra_page;

	length = PAGE_FRAME(length) - extra_page * PAGE_SIZE;

	maintenance_memory_in_pages = state.allocator_maintenance_memory / PAGE_SIZE + extra_page;

	zone->usable_num_pages = zone->abs_num_pages - maintenance_memory_in_pages;

	__initialize_offsets(zone);
}

void __init_free_lists(struct memory_zone* zone)
{
	uint64_t num_big_pages = zone->usable_num_pages >> zone->max_order;

	for (int o = 0; o <= zone->max_order; ++o) {
		INIT_LIST_HEAD(&zone->free_areas[o].list);
	}

	for (int p = 0; p < num_big_pages; ++p) {
		uint64_t pfn_offset = p << zone->max_order;
		list_add_tail(&zone->mem_map[pfn_offset].list, &zone->free_areas[zone->max_order].list);
	}
}

void mem_init(const struct cpu_memory_info* mem_info)
{
	const struct mem_zone_info* biggest_mapped = NULL;
	state.total_system_memory = 0;
	state.num_zones = mem_info->num_mem_zones;
	state.allocator_maintenance_memory = 0;
	state.total_maintenance_used = 0;

	/* First find the biggest zone that is below the identity mapped region */
	for (int z = 0; z < mem_info->num_mem_zones; ++z) {
		const struct mem_zone_info* zone_info = &mem_info->zones[z];
		struct memory_zone* zone = &state.zones[z];
		state.total_system_memory += zone_info->length;

		zone->phys_base = zone_info->phys_base;
		zone->abs_num_pages = zone_info->length / PAGE_SIZE;

		/* figure the bitmap length for each page order */
		for (int i = 0; i < MAX_PG_ORDER; ++i) {
			zone->bitmap_sizes[i] = zone->abs_num_pages / (8 * (2 << i));
			if (zone->bitmap_sizes[i] % sizeof(uint32_t)) {
				zone->bitmap_sizes[i] += sizeof(uint32_t) - (zone->bitmap_sizes[i] % sizeof(uint32_t));
			}
			if (!zone->bitmap_sizes[i]) {
				zone->bitmap_sizes[i] = sizeof(uint32_t);
			}
			state.allocator_maintenance_memory += zone->bitmap_sizes[i];
		}

		/* set the flag if the zone is going to be identity mapped */
		if (zone_info->phys_base + zone_info->length < 0x100000000) {
			zone->flags = MEMZONE_IDENTITY_MAPPED;
			if (!biggest_mapped || zone_info->length > biggest_mapped->length) {
				biggest_mapped = zone_info;
				state.biggest_mapped_idx = z;
			}
		}
		state.zones[z].abs_pfn_start = state.zones[z].abs_pfn_base = state.zones[z].phys_base / PAGE_SIZE;
	}

	state.total_system_memory_pages = state.total_system_memory / PAGE_SIZE;
	state.allocator_maintenance_memory += state.total_system_memory_pages * sizeof(mem_map_t);

	if (biggest_mapped->length < state.allocator_maintenance_memory) {
		asm ("hlt");
	}

	for (int z = 0; z < state.num_zones; ++z) {
		if (z == state.biggest_mapped_idx) {
			continue;
		}

		state.zones[z].usable_num_pages = state.zones[z].abs_num_pages;

		__initialize_offsets(state.zones + z);
	}

	/* special case for the biggest mapped zone */
	__init_maintenance_zone(state.zones + state.biggest_mapped_idx);

	/* finally init the free lists */
	for (int z = 0; z < state.num_zones; ++z) {
		state.zones[z].available_pages = state.zones[z].usable_num_pages;
		__init_free_lists(state.zones + z);
	}
}

struct page* __alloc_from_zone(struct memory_zone* zone, uint32_t order)
{
	struct list_head *free;
	struct page *page;

	free = &zone->free_areas[order].list;

	if (order == zone->max_order) {
		if (list_empty(free)) {
			return NULL;
		}

		page = (struct page*)free->next;
		list_del_init(&page->list);
		return page;
	}

	if (!list_empty(free)) {
		page = (struct page*)free->next;
		list_del_init(&page->list);
		goto page_found;
	}

	/* get page from superior order */
	page = __alloc_from_zone(zone, order + 1);
	if (!page)
		return NULL;

	/* since this is a page at the beggining of 2^(order + 1) pages,
	 * add its buddy (next block of 2^order pages) to the free areas */
	list_add_tail(&zone->free_areas[order].list, &page[1 << order].list);

page_found:
	__toggle_bit_page(page, order);
	return page;
}

struct page *alloc_pages(uint32_t flags, uint32_t order)
{
	struct page *p = NULL;
	uint64_t required_pages = (1 << order);

	for (int z = 0; z < state.num_zones; ++z) {
		struct memory_zone* zone = state.zones + z;
		if (!(flags & PGALLOC_KERNEL && zone->flags & MEMZONE_IDENTITY_MAPPED)) {
			continue;
		}

		if (zone->max_order < order || zone->available_pages < required_pages) {
			continue;
		}

		p = __alloc_from_zone(zone, order);
		if (p) {
			zone->available_pages -= (1 << order);
			break;
		}
	}

	if (p->vaddr) {
		memset(p->vaddr, 0, PAGE_SIZE * (1 << order));
	}

	return p;
}

void free_pages(struct page* p, uint32_t order)
{
	uint64_t pfn;
	struct memory_zone* zone = NULL;
	uint64_t buddy_pfn;
	struct list_head *free = NULL;
	struct page* buddy = NULL;

	zone = get_zone_and_pfn_from_page(p, &pfn);
	if (!zone) return;
	if (order > zone->max_order) {
		return;
	}

	free = &zone->free_areas[order].list;

	/* SHOULD be aligned to (1 << order) pages*/
	if (pfn % (1 << order)) {
		return;
	}

	zone->available_pages += (1 << order);

	if (__toggle_bit_pfn_zone(pfn, zone, order)) {
		list_add(free, &p->list);
		return;
	}

	/* in case we are the 'odd' bydyy in the block of order + 1*/
	buddy_pfn = pfn >> (order + 1);
	buddy = zone->mem_map + buddy_pfn;
	list_del_init(&buddy->list);
	list_del_init(&buddy[1 << order].list);

	free_pages(buddy, order + 1);
}

struct page* get_page_from_vaddr(void* vaddr)
{
	uint64_t paddr = VA2PA(vaddr);

	for (int z = 0; z < state.num_zones; ++z) {
		struct memory_zone* zone = state.zones + z;
		uint64_t pgindex = 0;

		if (!(paddr >= zone->phys_base && paddr < zone->phys_base + zone->usable_num_pages * PAGE_SIZE)) {
			continue;
		}

		pgindex = paddr / PAGE_SIZE - zone->abs_pfn_start;
		return zone->mem_map + pgindex;
	}

	return NULL;
}
