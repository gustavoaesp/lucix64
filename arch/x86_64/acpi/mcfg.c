#include <arch/acpi/mcfg.h>
#include <lucix/printk.h>
#include <lucix/device.h>

void parse_mcfg(struct acpi_mcfg *mcfg)
{
	uint64_t curr_index = sizeof(struct acpi_mcfg);
	struct acpi_mcfg_entry *curr_ptr = (struct acpi_mcfg_entry*)(mcfg + 1);

	if ((mcfg->header.length - sizeof(struct acpi_mcfg)) % sizeof(struct acpi_mcfg_entry)) {
		return;
	}

	while(curr_index < mcfg->header.length) {
		struct device_table_element* device = mem_cache_alloc_obj(g_device_mem_cache);

		device->device_type = DEVICE_TYPE_PCI_CFG;
		device->device.pci_cfg.address_base = curr_ptr->base_address;
		device->device.pci_cfg.bus_start = curr_ptr->start_pci_bus_number;
		device->device.pci_cfg.bus_end = curr_ptr->end_pci_bus_number;
		device->device.pci_cfg.segment = curr_ptr->pci_segment_group;
		add_device_to_table(device);

		curr_ptr++;
		curr_index += sizeof(struct acpi_mcfg_entry);
	}
}
