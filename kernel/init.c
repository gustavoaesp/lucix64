#include <arch/ioremap.h>
#include <arch/paging.h>
#include <arch/setup.h>

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

#include <arch/acpi.h>

struct initramfs initramfs_info;

static void hcf(void)
{
	for(;;) {
		asm ("hlt");
	}
}

/*static void kernel_init_task(void *__unused)
{
	printf("kernel_init_task\n");
	for (int i = 0; i < 16; ++i) {
		for(uint32_t counter = 0; counter <= 0xfffffff; counter++) {}
		printf("kit: %d\n", i);
	}
	for(;;) {
		asm volatile ("hlt");
	}
}

static void concurrent_task_test(void *__unused)
{
	printf("concurrent_task_test\n");

	for (int i = 0; i < 16; ++i) {
		for (uint32_t counter = 0; counter <= 0x7ffffff; counter++) {}
		printf("ctt: %d\n", i);
	}
	for(;;) {
		asm volatile ("hlt");
	}
}*/

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

	vfs_init();

	pci_init();

	do_initcalls();
	//load_elf(startup_data->ramdisk->addr, startup_data->ramdisk->size);
	sched_init();

	/*task_exec(t, initramfs_info.addr, initramfs_info.size);*/
	/*sched_mk_kernel_task(kernel_init_task, NULL);
	sched_mk_kernel_task(concurrent_task_test, NULL);*/
	sched_mk_kernel_task(kinit_task, NULL);

	/*	Given that at this point there is no active task in the scheduler, this will force setting
	*	up an active task and context switch into that task
	*/
	sched_irq();

	printf("Halt (should never reach here)\n");
	while(1) {
		asm volatile ("hlt");
	}
}
