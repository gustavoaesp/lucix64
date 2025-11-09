OUTPUT_FORMAT(elf64-x86-64)

ENTRY(_start)

PHDRS
{
	text	PT_LOAD	FLAGS((1 << 0) | (1 << 2));
	rodata	PT_LOAD	FLAGS((1 << 2));
	data	PT_LOAD	FLAGS((1 << 1) | (1 << 2));
	dynamic	PT_DYNAMIC	FLAGS((1 << 1) | (1 << 2));
}

SECTIONS
{
	. = 0xffffffff80000000;
	_kernel_bin_start = .;

	. = ALIGN(0x1000);
	.text : {
		*(.text .text.*)
	} :text

	. += CONSTANT(MAXPAGESIZE);

	.rodata : {
		*(.rodata .rodata.*)
	} :rodata

	. += CONSTANT(MAXPAGESIZE);

	. = ALIGN(0x1000);
	.data : {
		*(.data .data.*)
	} :data

	. = ALIGN(0x1000);
	.dynamic : {
		*(.dynamic)
	} :data :dynamic

	. = ALIGN(0x1000);
	.bss : {
		*(.bss .bss.*)
		*(COMMON)
	} :data

	. = ALIGN(0x1000);
	.init : {
		__initcall_start = .;
		*(.init .init.*)
		__initcall0_start = .;
		*(.initcall0.init)
		__initcall1_start = .;
		*(.initcall1.init)
		__initcall2_start = .;
		*(.initcall2.init)
		__initcall3_start = .;
		*(.initcall3.init)
		__initcall4_start = .;
		*(.initcall4.init)
		__initcall5_start = .;
		*(.initcall5.init)
		__initcallrootfs_start = .;
		*(.initcallrootfs.init)
		__initcall6_start = .;
		*(.initcall6.init)
		__initcall7_start = .;
		*(.initcall7.init)
		__initcall_end = .;
	} :data

	/DISCARD/ : {
		*(.eh_frame)
		*(.node .node.*)
	}
}
