#ifndef _LUCIX_DEVICE_H_
#define _LUCIX_DEVICE_H_
#include <stdint.h>
#include <lucix/slab.h>

enum device_type {
	DEVICE_TYPE_PCI_CFG	= 0,
	DEVICE_TYPE_PCI		= 1,
	DEVICE_TYPE_PCI_EXPRESS = 2,
	DEVICE_TYPE_GPIO	= 3,
	DEVICE_TYPE_APIC	= 4
};

struct pci_device
{
	uint32_t bus, slot, function;
};

struct pci_express_device
{
	uint32_t bus, slot, function;
	uint64_t	bar_phys;
};

struct pci_cfg_space {
	uint64_t address_base;
	uint32_t segment;
	uint32_t bus_start;
	uint32_t bus_end;
};

struct device_table_element
{
	struct list_head	list;
	uint32_t		device_type;
	char			driver[16];
	union {
		struct pci_device		pci;
		struct pci_express_device	pci_express;
		struct pci_cfg_space		pci_cfg;
	} device;
};

extern struct list_head g_device_table;
extern obj_mem_cache_t*	g_device_mem_cache;

void init_device_table();

void add_device_to_table(struct device_table_element* device);

#endif
