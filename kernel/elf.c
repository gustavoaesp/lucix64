#include <exec/elf.h>
#include <arch/paging.h>
#include <lucix/printk.h>
#include <lucix/mm.h>



void load_elf(void* elf, uint64_t size)
{
	/*struct elf_header_64* header = elf;
	if (memcmp(elf_magic, header->magic, 4)) {
		printf( "Not a valid elf\n");
		return;
	} else {
		printf("Valid elf\n");
	}
	
	if (header->bits == 1) {
		printf("32bits\n");
	} else if (header->bits == 2) {
		printf("64bits\n");
	} else {
		printf("Invalid bitcount\n");
	}

	if (header->endian == 1) {
		printf("Little-Endian\n");
	} else if (header->endian == 2) {
		printf("Big-Endian\n");
	} else {
		printf("Invalid endianness\n");
	}

	switch (header->type) {
	case elf_type_executable:
		printf("Executable elf\n");
		break;
	default:
		printf("Invalid elf type\n");
	}

	if (header->isa == elf_isa_x64) {
		printf("ISA: x86_64\n");
	}

	printf("entry point: %p\n", header->entry);

	printf("num sections: %d\n", header->num_section_header_entries);
	printf("num proghdrs: %d\n", header->num_program_header_entries);

	struct elf_header_entry_64* header_table = elf + header->program_header_table_pos;
	for (int i = 0; i < header->num_program_header_entries; ++i) {
		printf("Program header %d\n", i);
		printf("* vaddr: %p\n", header_table[i].p_vaddr);
		printf("* sizef: %d\n", header_table[i].size_in_file);
		printf("* sizem: %d\n", header_table[i].size_in_mem);
		printf("* flags: %d\n", header_table[i].flags);
	}

	struct page* dest_page = alloc_pages(PGALLOC_KERNEL, 0);
	uint64_t phys_base = VA2PA(dest_page->vaddr);

	//map_contiguous(phys_base, header_table->p_vaddr, 0x1000, 0);
	memcpy(dest_page->vaddr, elf + header_table->offset_data, header_table->size_in_mem);

	void (*entry)() = dest_page->vaddr;
	//void (*entry)() = (void*)header_table->p_vaddr;

	printf("entry: %p\n", entry);

	entry();*/
}
