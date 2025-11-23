.global _gdt_reload_segments

_gdt_reload_segments:
	sub		$16,		%rsp
	movq		$8,		8(%rsp)
	movabsq		$reload_cs,	%rax
	mov		%rax,		(%rsp)
	lretq

reload_cs:
	mov	$0x10,	%ax
	mov	%ax,	%ds
	mov	%ax,	%es
	mov	%ax,	%fs
	mov	%ax,	%gs
	mov	%ax,	%ss
	mov	$0x28,	%ax
	ltr	%ax
	ret
