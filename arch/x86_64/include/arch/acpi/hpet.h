#ifndef _ACPI_HPET_H_
#define _ACPI_HPET_H_

#include <arch/acpi.h>
#include <lucix/compiler.h>

struct hpet_address
{
	uint8_t address_space_id;    // 0 - system memory, 1 - system I/O
	uint8_t register_bit_width;
	uint8_t register_bit_offset;
	uint8_t reserved;
	uint64_t address;
} __packed;

struct acpi_hpet
{
	struct sdt_header header;
	uint8_t hardware_rev_id;
	uint8_t comparator_count:5;
	uint8_t counter_size:1;
	uint8_t reserved:1;
	uint8_t legacy_replacement:1;
	uint16_t pci_vendor_id;
	struct hpet_address address;
	uint8_t hpet_number;
	uint16_t minimum_tick;
	uint8_t page_protection;
} __packed;

void parse_hpet(struct acpi_hpet* hpet);

#endif
