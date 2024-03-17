#ifndef _ACPI_MCFG_H_
#define _ACPI_MCFG_H_

#include <arch/acpi.h>
#include <lucix/compiler.h>

struct acpi_mcfg_entry
{
	uint64_t base_address;
	uint16_t pci_segment_group;
	uint8_t start_pci_bus_number;
	uint8_t end_pci_bus_number;
	uint32_t reserved;
} __packed;

struct acpi_mcfg
{
	struct sdt_header header;
	uint64_t reserved;
} __packed;

void parse_mcfg(struct acpi_mcfg *mcfg);

#endif
