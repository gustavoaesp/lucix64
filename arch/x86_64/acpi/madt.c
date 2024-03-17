#include <arch/acpi/madt.h>

#include <lucix/printk.h>

struct acpi_madt *g_acpi_madt;

void parse_madt(struct acpi_madt *madt)
{
	uint32_t curr_index = sizeof(struct acpi_madt);
	struct acpi_madt_entry *curr_ptr = (struct acpi_madt_entry*)(madt + 1);

	// pointers used
	struct madt_processor_local_apic *lapic = (struct madt_processor_local_apic*)curr_ptr;
	struct madt_io_apic *ioapic;
	struct madt_io_apic_interrupt_source_override *src_override;
	struct madt_io_apic_nmi_interrupt_source *nmi_source;
	struct madt_local_apic_nmi_interrupts *lapic_nmi;
	struct madt_local_apic_address_override *lapic_override;

	printf("MADT found\n");

	g_acpi_madt = madt;

	printf("Length:      %d\n", madt->header.length);
	printf("Local APIC:  0x%p\n", madt->local_apic_addr);
	printf("Flags:       0x%p\n", madt->flags);

	while (curr_index < madt->header.length) {
		switch(curr_ptr->entry_type) {
		case ACPI_MADT_ENTRY_TYPE0_LAPIC:
			lapic = (struct madt_processor_local_apic*)curr_ptr;
			/*printf("(0) LAPIC FOUND\n");
			printf("  cpu id:  0x%p\n", lapic->acpi_processor_id);
			printf("  apic id: 0x%p\n", lapic->apic_id);
			printf("  flags:   0x%p\n", lapic->flags);*/
			curr_index += sizeof(struct madt_processor_local_apic);
			curr_ptr = (struct acpi_madt_entry*)(++lapic);
			break;
		case ACPI_MADT_ENTRY_TYPE1_IOAPIC:
			ioapic = (struct madt_io_apic*)curr_ptr;
			printf("(1) IOAPIC FOUND\n");
			/*printf("  id:     0x%p\n", ioapic->io_apic_id);
			printf("  addr:   0x%p\n", ioapic->io_apic_address);
			printf("  gsib:   0x%p\n", ioapic->global_system_interrupt_base);*/
			curr_index += sizeof(struct madt_io_apic);
			curr_ptr = (struct acpi_madt_entry*)(++ioapic);
			break;
		case ACPI_MADT_ENTRY_TYPE2_IOAPIC_SOURCE_OVERRIDE:
			src_override = (struct madt_io_apic_interrupt_source_override*)curr_ptr;
			printf("(2) IOAPIC SOURCE OVERRIDE\n");
			printf("  bus_source: 0x%p\n", src_override->bus_source);
			printf("  irq_source: 0x%p\n", src_override->irq_source);
			printf("  gsi:        0x%p\n", src_override->global_system_interrupt);
			printf("  flags:      0x%p\n", src_override->flags);
			curr_index += sizeof(struct madt_io_apic_interrupt_source_override);
			curr_ptr = (struct acpi_madt_entry*)(++src_override);
			break;
		case ACPI_MADT_ENTRY_TYPE3_IOAPIC_NMI_SOURCE:
			nmi_source = (struct madt_io_apic_nmi_interrupt_source*)curr_ptr;
			printf("(3) NMI INTERRUPT SOURCE\n");
			curr_index += sizeof(struct madt_io_apic_nmi_interrupt_source);
			curr_ptr = (struct acpi_madt_entry*)(++nmi_source);
			break;
		case ACPI_MADT_ENTRY_TYPE4_LAPIC_NMI:
			lapic_nmi = (struct madt_local_apic_nmi_interrupts*)curr_ptr;
			printf("(4) LAPIC NMI\n");
			curr_index += sizeof(struct madt_local_apic_nmi_interrupts);
			curr_ptr = (struct acpi_madt_entry*)(++lapic_nmi);
			break;
		case ACPI_MADT_ENTRY_TYPE5_LAPIC_ADDRESS_OVERRIDE:
			lapic_override = (struct madt_local_apic_address_override*)curr_ptr;
			printf("(5) LAPIC ADDRESS OVERRIDE\n");
			curr_index += sizeof(struct madt_local_apic_address_override);
			curr_ptr = (struct acpi_madt_entry*)(++lapic_override);
			break;
		}
	}
}
