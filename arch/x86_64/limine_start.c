#include <stdint.h>
#include <stddef.h>

#include <arch/acpi.h>
#include <arch/debug_eax.h>
#include <arch/early_paging.h>
#include <arch/init_mem.h>
#include <arch/paging.h>
#include <arch/framebuffer.h>

#include <lucix/limine.h>
#include <lucix/mm.h>
#include <lucix/slab.h>
#include <lucix/init.h>

static volatile struct limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0
};

static volatile struct limine_kernel_address_request kernel_address_request = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0
};

static volatile struct limine_memmap_request memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};

static volatile struct limine_rsdp_request rsdp_request = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0
};

static volatile struct limine_module_request module_request = {
	.id = LIMINE_MODULE_REQUEST,
	.revision = 0
};

static struct cpu_memory_info* cpu_info;
static struct lucix_startup_data setup;
static struct init_module module;

struct framebuffer_data efi_fb_data;

void _start(void)
{
	struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

	//setup_upper(kernel_address_request.response->physical_base);

	cpu_info = initialize_memory(memmap_request.response);

	efi_fb_data.width = framebuffer->width;
	efi_fb_data.height = framebuffer->height;
	efi_fb_data.pitch = framebuffer->pitch;
	efi_fb_data.phys_addr = VA2PA(framebuffer->address);

	if (module_request.response->module_count) {
		struct limine_file* file = module_request.response->modules[0];
		module.addr = file->address;
		module.size = file->size;
	}

	setup.mem_info = cpu_info;
	setup.ramdisk = &module;

	g_kernel_phys_base = kernel_address_request.response->physical_base;
	// 16 MiB fixed for now
	g_kernel_reserved_size = 0x1000000;

	// set the rsdp for acpi
	g_acpi_rsdp_desc = rsdp_request.response->address;

	start_kernel(&setup);
}
