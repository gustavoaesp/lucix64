#include <arch/paging.h>
#include <arch_generic/sched.h>

#include <lucix/binfmt.h>
#include <lucix/compiler.h>
#include <lucix/cpu.h>
#include <lucix/errno.h>
#include <lucix/vfs.h>
#include <lucix/fs/file.h>
#include <lucix/initcall.h>
#include <lucix/mmap.h>
#include <lucix/printk.h>
#include <lucix/slab.h>
#include <lucix/task.h>
#include <lucix/utils.h>
#include <lucix/vma.h>
#include <stdint.h>

static uint8_t elf_magic[4] = {
	0x7f,
	'E',
	'L',
	'F'
};

enum elf_isa
{
	elf_isa_x86 = 3,
	elf_isa_arm = 0x28,
	elf_isa_x64 = 0x3E,
	elf_isa_a64 = 0xB7
};

enum elf_type
{
	elf_type_relocatable = 1,
	elf_type_executable = 2,
	elf_type_shared = 3,
	elf_type_core = 4
};

enum elf_segment_type
{
	elf_null_segment = 0,
	elf_load_segment = 1,
	elf_dyn_segment = 2,
	elf_interp_segment = 3,
	elf_note_section_segment = 4
};

enum elf_segment_flags
{
	elf_segment_executable = 0x1,
	elf_segment_writable = 0x2,
	elf_segment_readable = 0x4
};

struct elf_header_64
{
	char magic[4];
	uint8_t bits; // 1 -> 32; 2 -> 64
	uint8_t endian; // 1 -> little; 2 -> big
	uint8_t elf_header_version;
	uint8_t abi; // 0 -> sysv
	uint64_t _unused;
	uint16_t type;
	uint16_t isa;
	uint32_t elf_version;
	uint64_t entry;
	uint64_t program_header_table_pos;
	uint64_t section_header_table_pos;
	uint32_t flags;
	uint16_t header_size;
	uint16_t program_header_table_entry_size;
	uint16_t num_program_header_entries;
	uint16_t section_header_table_entry_size;
	uint16_t num_section_header_entries;
	uint16_t index_section_names;
} __packed;

struct elf_header_entry_64
{
	uint32_t segment_type;
	uint32_t flags;
	uint64_t offset_data;
	uint64_t p_vaddr;
	uint64_t undef; // not used in sysv abi
	uint64_t size_in_file;
	uint64_t size_in_mem;
	uint64_t alignment;
} __packed;

static int is_valid_elf(void *elf)
{
	struct elf_header_64* header = elf;
	if (memcmp(elf_magic, header->magic, 4)) {
		return 0;
	}
	
	if (header->bits == 1) {
		/*printf("32bits\n");*/
	} else if (header->bits == 2) {
		/*printf("64bits\n");*/
	} else {
		printf("Invalid bitcount\n");
		return 0;
	}

	if (header->endian == 1) {
		/*printf("Little-Endian\n");*/
	} else if (header->endian == 2) {
		/*printf("Big-Endian\n");*/
	} else {
		printf("Invalid endianness\n");
		return 0;
	}

	switch (header->type) {
	case elf_type_executable:
		/*printf("Executable elf\n");*/
		break;
	default:
		printf("Invalid elf type\n");
		return 0;
	}

	if (header->isa == elf_isa_x64) {
		/*printf("ISA: x86_64\n");*/
	}
	return 1;
}

static int elf_load_segments(struct file *file, struct elf_header_64 *header)
{
	struct cpu *cpu = cpu_get_cpu();
	struct elf_header_entry_64 *entries = kmalloc(sizeof(struct elf_header_entry_64) * header->num_program_header_entries, 0);
	vfs_lseek(file, header->program_header_table_pos, SEEK_SET);
	file->ops->read(file, entries, sizeof(struct elf_header_entry_64) * header->num_program_header_entries, &file->offset);

	for (int i = 0; i < header->num_program_header_entries; ++i) {
		struct elf_header_entry_64 *entry = entries + i;
		/*printf("entry %d\n", i);
		printf(" alignment:    %p\n", entry->alignment);
		printf(" vaddr:        %p\n", entry->p_vaddr);
		printf(" type:         %d\n", entry->segment_type);
		printf(" sizef:        %d\n", entry->size_in_file);
		printf(" sizem:        %d\n", entry->size_in_mem);*/
		uint32_t vm_prot_flags = 0;
		vm_prot_flags |= (entry->flags & elf_segment_readable)   ? VM_READ 	: 0;
		vm_prot_flags |= (entry->flags & elf_segment_writable)   ? VM_WRITE 	: 0;
		vm_prot_flags |= (entry->flags & elf_segment_executable) ? VM_EXEC	: 0;
		vm_prot_flags |= VM_USER;

		do_mmap(
			cpu->current->task->mm,
			file,
			PAGE_FRAME(entry->p_vaddr),
			entry->size_in_file,
			vm_prot_flags,
			VM_FLAG_PRIVATE,
			VMA_FILE,
			entry->offset_data / PAGE_SIZE
	       );
	}

	kfree(entries);

	return 0;
}

static int elf_load_binary(struct file *file, const char *filename, const char **argv,
			const char **envp, void**cpu_state)
{
	struct elf_header_64 *header =(struct elf_header_64*) kmalloc(
		sizeof(struct elf_header_64), 0
	);
	int ret = 0;
	file->ops->read(file, header, sizeof(struct elf_header_64), &file->offset);
	if (!is_valid_elf(header)) {
		kfree(header);
		return -EINVAL;
	}

	/* TODO fun stuff in here */
	ret = elf_load_segments(file, header);

	cpu_setup_process(cpu_state, header->entry);

	kfree(header);

	return 0;

}

static struct binfmt binfmt_elf = {
	.name = "ELF",
	.load_binary = elf_load_binary
};

static int binfmt_elf_init()
{
	register_binfmt(&binfmt_elf);
	return 0;
}

initcall_core(binfmt_elf_init);
