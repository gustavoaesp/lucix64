.macro pushaq_macro
	pushq	%rax
	pushq	%rcx
	pushq	%rdx
	pushq	%rbx
	pushq	%rbp
	pushq	%rsi
	pushq	%rdi
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%r11
	pushq	%r10
	pushq	%r9
	pushq	%r8
.endm

.macro popaq_macro
	popq	%r8
	popq	%r9
	popq	%r10
	popq	%r11
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rdi
	popq	%rsi
	popq	%rbp
	popq	%rbx
	popq	%rdx
	popq	%rcx
	popq	%rax
.endm

/* Interrupt function macros */
.macro isr_no_err_stub	id
isr_stub_\id:
	pushaq_macro
	pushq	$\id
	movq	%rsp,	%rdi
	call	_exception_handler_stub_noerr
	popq	%rax
	popaq_macro
	iretq
.endm

.macro isr_err_stub id
.global isr_stub_\id
isr_stub_\id:
	pushaq_macro
	pushq	$\id
	movq	%rsp,	%rdi
	call	_exception_handler_stub_err
	popq	%rax
	popaq_macro
	add	$0x08,	%rsp/* This little fucker made me lose sleep.*/
	iretq
.endm

.macro isr_irq id
isr_irq_stub_\id:
	pushaq_macro
	pushq	$\id
	movq	%rsp,	%rdi
	call __irq_handler_stub
	popq	%rax
	popaq_macro
	iretq
.endm

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

.altmacro

.set i, 32
.rept 224
	isr_irq %i
	.set i, i+1
.endr

/* this macro expands to a function pointer table */
.macro gen_isr_entry id
.quad isr_stub_\id
.endm

.macro gen_isr_irq_entry id
.quad isr_irq_stub_\id
.endm

.global	isr_stub_table
isr_stub_table:
.set i, 0
.rept 32
	gen_isr_entry %i
	.set i, i+1
.endr
.set i, 32
.rept 224
	gen_isr_irq_entry %i
	.set i, i+1
.endr
