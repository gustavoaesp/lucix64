#ifndef _VM_H_
#define _VM_H_

#define	VMAP_ZONE_START	(0xffffc00000000000)
#define	VMAP_ZONE_SIZE	(0x0000010000000000)

struct vm_zone {
	struct vm_zone	*next;
	struct pages	**pages;
	uint64_t	start_addr;
	uint64_t	size;
	uint64_t	flags;
	uint64_t	num_pages;
	uint64_t	phys_addr;
};

struct vm_zone *get_vm_zone(uint64_t size, uint32_t flags);

struct vm_zone *remove_vm_zone(void* addr);

#endif
