
static void perform_syscall(unsigned long value)
{
	asm volatile ("movq	%rdi, %rax");
	asm volatile ("int $0x80");

	for (int i = 0; i < 0xffffffff; ++i) {}
}

void _entry()
{
	perform_syscall(0);
	for (;;) {
		for(int i = 0; i < 0xffffffff; i++) {
			perform_syscall(i);
		}
	}
}
