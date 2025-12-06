
.global write

write:
	/* Set the syscall number */
	movq	$0x01,	%rax
	/* These registers are already set */
	/* RDI -> fd */
	/* RSI -> src */
	/* RDX -> count */
	int $0x80 /* perform the syscall */
	ret
