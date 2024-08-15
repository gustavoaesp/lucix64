/* */

.global jump_to_user
jump_to_user:
    mov     $0x28,  %ax
    ltr     %ax
    mov     %rdi,   %rsp

    /* 0x20 | 3 -> User data segment */
    mov     $0x23,  %ax
    mov     %ax,    %ds
    mov     %ax,    %es
    mov     %ax,    %fs
    mov     %ax,    %gs

    /* remove a uint64_t from the stack that has the interrupt id */
    add     $0x08,  %rsp
	popq	%r8
	popq	%r9
	popq	%r10
	popq	%r11
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
    popq    %rdi
    popq    %rsi
    popq    %rbp
    popq    %rbx
    popq    %rdx
    popq    %rcx
    popq    %rax
    iretq

/*
    void __iret_context_switch(uint32_t ss, struct interrupt_frame *)
    rdi: ss
    rsi: interrupt_frame
*/
.global __iret_context_switch
__iret_context_switch:
    mov     %rdi,   %rax
    mov     %ax,    %ds
    mov     %ax,    %es
    mov     %ax,    %fs
    mov     %ax,    %gs

    /*
    *   set the new stack!
    */
    mov     %rsi,   %rax
    mov     %rax,   %rsp
    /*
        Remove the interrupt id parameter from the stack
    */
    add     $0x08,  %rsp
	popq	%r8
	popq	%r9
	popq	%r10
	popq	%r11
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
    popq    %rdi
    popq    %rsi
    popq    %rbp
    popq    %rbx
    popq    %rdx
    popq    %rcx
    popq    %rax
    iretq