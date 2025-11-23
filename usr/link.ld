OUTPUT_FORMAT(elf64-x86-64)

ENTRY(_entry)

/*
PHDRS
{
	text	PT_LOAD	FLAGS((1 << 0) | (1 << 2));
	rodata	PT_LOAD	FLAGS((1 << 2));
	data	PT_LOAD	FLAGS((1 << 1) | (1 << 2));
	dynamic	PT_DYNAMIC	FLAGS((1 << 1) | (1 << 2));
}

SECTIONS
{
	. = 0x00400000;

	.text : {
		*(.text .text.*)
	} :text

	.rodata : {
		*(.text .text.*)
	} :rodata

	.data : {
		*(.data .data.*)
	} :data

	.dynamic : {
		*(.dynamic)
	} :data :dynamic

	.bss : {
		*(.bss .bss.*)
		*(COMMON)
	} :data

	/DISCARD/ : {
		*(.eh_frame)
		*(.node .node.*)
	}
}*/
SECTIONS
{
  . = 0x40000;
  .text : { *(.text) }
  .data : { *(.data) }
  .bss : { *(.bss) }
}
