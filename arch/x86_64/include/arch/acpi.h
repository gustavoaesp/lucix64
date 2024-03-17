#ifndef _ACPI_H_
#define _ACPI_H_

#include <lucix/compiler.h>

struct rspd_descriptor {
	char signature[8];
	uint8_t checksum;
	char oem_id[6];
	uint8_t revision;
	uint32_t rsdt_address;
} __attribute__((packed));

struct rspd_descriptor2 {
	struct rspd_descriptor first_part;
	uint32_t length;
	uint64_t xsdt_address;
	uint8_t extended_checksum;
	uint8_t reserverd;
} __attribute__((packed));

struct sdt_header {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oem_id[6];
	char oem_table_id[8];
	uint32_t oem_revision;
	uint32_t creator_id;
	uint32_t creator_revision;
} __attribute__((packed));

struct acpi_xsdt {
	struct sdt_header header;

	uint64_t pointers[1];
} __attribute__((packed));

int acpi_init();

extern struct rspd_descriptor2* g_acpi_rsdp_desc;
extern struct acpi_xsdt* g_acpi_xsdt;

#endif
