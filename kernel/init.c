#include <arch/ioremap.h>
#include <arch/paging.h>
#include <arch/setup.h>

#include <lucix/console.h>
#include <lucix/device.h>
#include <lucix/init.h>
#include <lucix/initcall.h>
#include <lucix/mm.h>
#include <lucix/printk.h>
#include <lucix/slab.h>
#include <lucix/task.h>

#include <arch/acpi.h>

struct initramfs initramfs_info;

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
		printf("ACPI NOT FOUND\n");
	}


	init_apic();

	initramfs_info.addr = startup_data->ramdisk->addr;
	initramfs_info.size = startup_data->ramdisk->size;

	printf("module_addr: %p\n", startup_data->ramdisk->addr);
	printf("module_size: %d\n", startup_data->ramdisk->size);

	pci_init();

	//do_initcalls();
	//load_elf(startup_data->ramdisk->addr, startup_data->ramdisk->size);
	sched_init();

	struct task *t = create_task();
	task_exec(t, initramfs_info.addr, initramfs_info.size);

	while(1) {
		asm volatile ("hlt");
	}
}
