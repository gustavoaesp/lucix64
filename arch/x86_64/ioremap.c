#include <arch/ioremap.h>
#include <arch/paging.h>
#include <lucix/vm.h>

void *ioremap(uint64_t phys_addr, uint64_t size, uint32_t flags)
{
	struct vm_zone *zone = get_vm_zone(size, flags);
	zone->phys_addr = phys_addr;

	return map_vm_zone(zone, flags);
}
