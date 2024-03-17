#ifndef _LUCIX_ELF_H_
#define _LUCIX_ELF_H_
#include <lucix/compiler.h>

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

void load_elf(void* elf, uint64_t size);

#endif
