#include <stddef.h>
#include <arch/init_mem.h>
#include <arch/debug_eax.h>
#include <lucix/mm.h>

static struct cpu_memory_info x86_64_cpu_info = {
	.num_mem_zones = 0
};

struct cpu_memory_info* initialize_memory(struct limine_memmap_response* memmap)
{
	for (uint64_t e = 0; e < memmap->entry_count; ++e)
	{
		struct limine_memmap_entry* entry = memmap->entries[e];
		if (entry->type) {
			continue;
		}

		struct mem_zone_info* zone = &x86_64_cpu_info.zones[x86_64_cpu_info.num_mem_zones++];
		zone->phys_base = entry->base;
		zone->length = entry->length;
	}
	
	return &x86_64_cpu_info;
}
