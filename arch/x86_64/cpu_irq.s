.global cpu_irq_save
.global cpu_irq_restore
.global cpu_force_cli
.global cpu_force_sti

cpu_irq_save:
	pushfq
	pop %rax
	cli
	ret

cpu_irq_restore:
	push %rdi
	popfq
	ret

cpu_force_cli:
	cli
	ret

cpu_force_sti:
	sti
	ret
