#include <lucix/slab.h>
#include <lucix/vm.h>
#include <arch/paging.h>

static struct vm_zone *vm_zone_list = NULL;

struct vm_zone *get_vm_zone(uint64_t size, uint32_t flags)
{
	struct vm_zone **p, *tmp, *zone;
	uint64_t addr = VMAP_ZONE_START;

	zone = kmalloc(sizeof(struct vm_zone), 0);
	if(!zone)
		return NULL;

	if (size + PAGE_SIZE > VMAP_ZONE_SIZE) {
		kfree(zone);
		return NULL;
	}

	size += PAGE_SIZE;
	for (p = &vm_zone_list; (tmp = *p); p = &tmp->next) {
		if ((uint64_t)tmp->start_addr < addr)
			continue;
		if ((size + addr) < addr)
			goto out;
		if (size + addr <= (uint64_t)tmp->start_addr)
			goto found;
		addr = tmp->size + (uint64_t)tmp->start_addr;
		if (addr > VMAP_ZONE_START + VMAP_ZONE_SIZE - size)
			goto out;
	}

found:
	zone->next = *p;
	*p = zone;

	zone->flags = flags;
	zone->start_addr = addr;
	zone->pages = NULL;
	zone->num_pages = 0;
	zone->phys_addr = 0;
	zone->size = size;

	return zone;
out:
	kfree(zone);
	return NULL;
}

struct vm_zone *remove_vm_zone(void *addr)
{
	struct vm_zone **p, *tmp;

	for (p = &vm_zone_list; (tmp = *p); p = &tmp->next) {
		if (tmp->start_addr == (uint64_t)addr)
			goto found;
	}

	return NULL;
found:
	*p = tmp->next;
	return tmp;
}
