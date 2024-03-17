#include <arch/ioremap.h>
#include <arch/paging.h>
#include <arch/setup.h>

#include <lucix/console.h>
#include <lucix/device.h>
#include <lucix/init.h>
#include <lucix/mm.h>
#include <lucix/printk.h>
#include <lucix/slab.h>

#include <arch/acpi.h>

static void hcf(void)
{
	for(;;) {
		asm ("hlt");
	}
}

void start_kernel(struct lucix_startup_data* startup_data)
{
	mem_init(startup_data->mem_info);
	obj_mem_cache_init();
	init_device_table();

	setup_arch();

	console_init(startup_data->framebuffer);
	if(acpi_init()) {
		printf("ACPI NOT FOUND :(\n");
	}


	init_apic();
	/*
	printf("Lucix 0.01\n");

	asm volatile ("int $0x0");

	printf("After int\n");*/
	printf("module_addr: %p\n", startup_data->ramdisk->addr);
	printf("module_size: %d\n", startup_data->ramdisk->size);
	pci_init();

	//load_elf(startup_data->ramdisk->addr, startup_data->ramdisk->size);

	while(1);
}
