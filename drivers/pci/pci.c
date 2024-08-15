#include <lucix/device.h>
#include <lucix/printk.h>

#include <arch/paging.h>

#include <lucix/pci.h>

static void probe_cfg_space(struct pci_cfg_space* cfg)
{
	void* pci_space = PA2VA(cfg->address_base);
	uint32_t bus_start = cfg->bus_start;

	/*for (int bus = bus_start; bus <= cfg->bus_end; ++bus) {
		for (int device = 0; device < 32; ++device) {
			for (int func = 0; func < 8; ++func) {
				uint64_t address = (uint64_t)pci_space
					+ ((bus - bus_start) << 20)
					| (device << 15) | (func << 12);
				struct pci_mmio_header* header =
					(struct pci_mmio_header*)address;
				if (header->vendor == 0xffff)
					continue;
				/*printf("Pci Dev: %.2x:%.2x.%.1x\n", bus, device, func);
				printf(
					"* Vendor %.4x | Device %.4x\n",
					header->vendor, header->device
				);
				printf(
					"* Class: %.2x | Subclass: %.2x\n",
					header->class_code, header->subclass
				);
				printf("* ProgIF: %.2x\n", header->prog_if);
			}
		}
	}*/
}

void pci_init()
{
	struct list_head* list;
	__list_for_each(list, &g_device_table) {
		struct device_table_element* device = (struct device_table_element*)list;
		if (device->device_type == DEVICE_TYPE_PCI_CFG) {
			printf("Found a cfg space, probing\n");
			probe_cfg_space(&device->device.pci_cfg);
		}
	}
}
