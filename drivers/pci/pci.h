#ifndef _DRIVERS_PCI_H_
#define _DRIVERS_PCI_H_
#include <lucix/compiler.h>

struct pci_dev
{
	uint32_t segment, bus, device;
};

struct pci_mmio_header
{
	uint16_t vendor, device;
	uint16_t command, status;
	uint8_t revision_id, prog_if, subclass, class_code;
	uint8_t cache_line_size, latency_timer, header_type, bist;
} __packed;

#endif
