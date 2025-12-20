/*
   void __iret_context_switch(uint32_t ss, struct interrupt_frame *)
    rdi: ss
    rsi: interrupt_frame
*/
.global __iret_context_switch
__iret_context_switch:

	/*mov	$0x28,	%ax*/
	/*ltr	%ax*/

	mov	%rdi,	%rsp
	mov	%rdi,	%rax
	mov	%ax,	%ds
	mov	%ax,	%es
	mov	%ax,	%fs
	mov	%ax,	%gs

	/*
	 *	set the new stack!
	 */
	mov	%rsi,   %rax
	mov	%rax,   %rsp
	/*
	 *	Remove the interrupt id parameter from the stack
	 */
	add	$0x08,	%rsp
	/*
	*	Pop all registers before iretq
	*/
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
	iretq

.global cpu_switch_to
cpu_switch_to:
	/*
	 *	rdi: prev
	 *	rsi: next
	 **/
	pushq	%rbp
	pushq	%rbx
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15

	/* save old stack */
	movq	%rsp,		0x40(%rdi)
	movq	0x40(%rsi),	%rsp

	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%rbx
	popq	%rbp

	ret
