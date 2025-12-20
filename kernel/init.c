#include <arch/apic.h>
#include <arch/ioremap.h>
#include <arch/paging.h>
#include <arch/setup.h>
#include <lucix/pci.h>

#include <lucix/console.h>
#include <lucix/device.h>
#include <lucix/vfs.h>
#include <lucix/init.h>
#include <lucix/initcall.h>
#include <lucix/mm.h>
#include <lucix/printk.h>
#include <lucix/sched.h>
#include <lucix/slab.h>
#include <lucix/task.h>

#include <pci/pci.h>

#include <arch/acpi.h>
#include <arch_generic/sched.h>

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

	/*console_init(startup_data->framebuffer);*/
	if(acpi_init()) {
		printf("ACPI NOT FOUND\n");
	}

	init_apic();

	initramfs_info.addr = startup_data->ramdisk->addr;
	initramfs_info.size = startup_data->ramdisk->size;

	printf("module_addr: %p\n", startup_data->ramdisk->addr);
	printf("module_size: %d\n", startup_data->ramdisk->size);

	vfs_init();

	/*pci_init();*/

	do_initcalls();

	sched_init();

	sched_per_cpu_init();
	create_task(&task_init, TASK_USER);
	cpu_ktask_setup(&task_init, kinit_task, NULL);
	sched_add_task(&task_init);

	/*	Given that at this point there is no active task in the scheduler,
	 *	this will force setting
	 *	up an active task and context switch into that task
	 **/
	sched_start();

	printf("Halt (should never reach here)\n");
	while(1) {
		asm volatile ("hlt");
	}
}
