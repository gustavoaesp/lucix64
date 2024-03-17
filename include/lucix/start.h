#ifndef _LUCIX_KERNEL_START_H_
#define _LUCIX_KERNEL_START_H_
#include <stdint.h>

struct framebuffer_data
{
	uint32_t	width, height;
	uint32_t	pitch;
	uint32_t	bpp;
	uint64_t	phys_addr;
};

struct init_module
{
	void* addr;
	uint64_t size;
};

struct lucix_startup_data
{
	struct cpu_memory_info* mem_info;
	struct framebuffer_data* framebuffer;
	struct init_module* ramdisk;
};

#endif
