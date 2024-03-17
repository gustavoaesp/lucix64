#ifndef _ACPI_MADT_H_
#define _ACPI_MADT_H_

#include <arch/acpi.h>

struct acpi_madt {
	struct sdt_header header;

	uint32_t local_apic_addr;
	uint32_t flags; /* 1 = dual 8259 legacy PIC's enabled */
} __attribute__((packed));

struct acpi_madt_entry {
	uint8_t entry_type;
	uint8_t record_length;
} __attribute__((packed));

/* type 0 */
struct madt_processor_local_apic {
	struct acpi_madt_entry entry;
	uint8_t acpi_processor_id;
	uint8_t apic_id;
	uint32_t flags;
} __attribute__((packed));

/* type 1 */
struct madt_io_apic {
	struct acpi_madt_entry entry;
	uint8_t io_apic_id;
	uint8_t reserved;
	uint32_t io_apic_address;
	uint32_t global_system_interrupt_base;
} __attribute__((packed));

/* type 2 */
struct madt_io_apic_interrupt_source_override {
	struct acpi_madt_entry entry;
	uint8_t bus_source;
	uint8_t irq_source;
	uint32_t global_system_interrupt;
	uint16_t flags;
} __attribute__((packed));

/* type 3 */
struct madt_io_apic_nmi_interrupt_source {
	struct acpi_madt_entry entry;
	uint8_t nmi_source;
	uint8_t reserved;
	uint16_t flags;
	uint32_t global_system_interrupt;
} __attribute__((packed));

/* type 4 */
struct madt_local_apic_nmi_interrupts {
	struct acpi_madt_entry entry;
	uint8_t acpi_processor_id;
	uint16_t flags;
	uint8_t lint_num;
} __attribute__((packed));

/* type 5 */
struct madt_local_apic_address_override {
	struct acpi_madt_entry entry;
	uint16_t reserved;
	uint64_t apic_address;
} __attribute__((packed));

// TODO type 9, x2apic
#define ACPI_MADT_ENTRY_TYPE0_LAPIC			0
#define ACPI_MADT_ENTRY_TYPE1_IOAPIC			1
#define ACPI_MADT_ENTRY_TYPE2_IOAPIC_SOURCE_OVERRIDE	2
#define ACPI_MADT_ENTRY_TYPE3_IOAPIC_NMI_SOURCE		3
#define ACPI_MADT_ENTRY_TYPE4_LAPIC_NMI			4
#define ACPI_MADT_ENTRY_TYPE5_LAPIC_ADDRESS_OVERRIDE	5

extern struct acpi_madt *g_acpi_madt;

extern void parse_madt(struct acpi_madt *madt);

#endif
