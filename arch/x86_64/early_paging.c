#include <stdint.h>
#include <stddef.h>
#include <arch/debug_eax.h>
#include <arch/paging.h>
#include <lucix/mm.h>

uint64_t pml4[512] __attribute__((aligned(0x1000)));

static uint64_t pdpt_identity[512] __attribute__((aligned(0x1000)));
static uint64_t pdpt_vm_region[512] __attribute__((aligned(0x1000)));
static uint64_t pdpt_kernel[512] __attribute__((aligned(0x1000)));

static uint64_t pdet_identity[4][512] __attribute__((aligned(0x1000)));
static uint64_t pdet_kernel[512] __attribute__((aligned(0x1000)));

static uint64_t pte_kernel[4][512] __attribute__((aligned(0x1000)));

static uint64_t kernel_phys_base;

static uint64_t kernel_va2phys(void* va)
{
	uint64_t offset_from_mapping = ((uint64_t)va) ^ BASE_KERNEL_VA;
	return kernel_phys_base + offset_from_mapping;
}

void map_contig_phys(uint64_t size, uint64_t base_phys, uint64_t base_vaddress,
			uint64_t* pdpt, uint64_t* pdet, uint64_t* pte, uint32_t _2mb)
{
	uint64_t pgsize = (_2mb) ? 0x200000 : PAGE_SIZE;
	uint64_t max_size = pgsize * 512;
	uint64_t num_pages = size / pgsize + ((size % pgsize) ? 1 : 0);

	if (size > max_size) {
		debug_eax(0xdeadbeef, 0xdeadbeef);
	}

	for (int p = 0; p < num_pages; ++p) {
		uint32_t pml4_idx = PML4_FROM_VA(base_vaddress);
		uint32_t pdpt_idx = PDPT_FROM_VA(base_vaddress);
		uint32_t pdet_idx = PDET_FROM_VA(base_vaddress);
		uint32_t pte_idx = PTE_FROM_VA(base_vaddress);

		pml4[pml4_idx] = PAGE_FRAME(kernel_va2phys(pdpt)) |
			PML4_PRESENT_FLAG | PML4_RW_FLAG;
		pdpt[pdpt_idx] = PAGE_FRAME(kernel_va2phys(pdet)) |
			PDPT_PRESENT_FLAG | PDPT_RW_FLAG;

		if (_2mb) {
			pdet[pdet_idx] = PAGE_FRAME(base_phys) |
				PDET_PAGE_SIZE | PDET_PRESENT_FLAG | PDET_RW_FLAG;
		} else {
			pdet[pdet_idx] = PAGE_FRAME(kernel_va2phys(pte)) |
				PDET_PRESENT_FLAG | PDET_RW_FLAG;
			pte[pte_idx] = PAGE_FRAME(base_phys) |
				PTE_PRESENT_FLAG | PTE_RW_FLAG;
		}

		base_phys += pgsize;
		base_vaddress += pgsize;
	}
}

void set_cr3(uint64_t val)
{
	asm volatile ("mov %rdi, %cr3");
}

void setup_upper(uint64_t kernel_phys_address, uint64_t kernel_base_va)
{
	uint64_t identity_phys_size	=	0x000100000000;
	uint64_t vm_region_size		=	0x3fff80000000;

	kernel_phys_base = kernel_phys_address;

	for(uint64_t i = 0; i < 4; ++i) {
		uint64_t base_frame = i * (1024 * 1024 * 1024);

		map_contig_phys(
			identity_phys_size / 4,
			base_frame, BASE_PHYS_IDENTITY + base_frame,
			pdpt_identity, pdet_identity[i], NULL,
			1
		);

		map_contig_phys(
			0x200000,
			kernel_phys_base + 0x200000*i, BASE_KERNEL_VA + 0x200000*i,
			pdpt_kernel, pdet_kernel, pte_kernel[i],
			0
		);
	}

	set_cr3(kernel_va2phys(pml4));
}
