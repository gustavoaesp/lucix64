#include <arch_generic/sched.h>
#include <arch_generic/cpu.h>

#include <arch/cpu_state.h>
#include <arch/gdt.h>
#include <arch/interrupt.h>
#include <arch/paging.h>

#include <lucix/printk.h>
#include <lucix/task.h>
#include <lucix/vma.h>

void cpu_ktask_setup(struct task *t, void (*__entry)(void*), void *args)
{
    struct arch_x86_cpu_state *cpu = t->cpu_state;
    int64_t frame_offset = (PAGE_SIZE << KSTACK_PGORDER) - sizeof(struct interrupt_frame) + 8;
    struct interrupt_frame *frame = NULL;

    if (!cpu) {
        /* TODO maybe create a slab allocator for this object? */
        t->cpu_state = cpu = kmalloc(sizeof(struct arch_x86_cpu_state), 0);
    }

    memset(cpu, 0, sizeof(struct arch_x86_cpu_state));

    cpu->cs = _KERNEL_CS;
    cpu->ss = _KERNEL_DS;
    cpu->rip = (uint64_t)__entry;
    cpu->rsp = (uint64_t)t->kstack + t->kstack_size;
    /* the args pointer for __entry */
    cpu->rdi = (uint64_t)args;

    cpu->rflags = 0x200;

}

/*
*   This has to be implemented in assembly
*/
extern void __iret_context_switch(
    uint32_t ss,
    struct interrupt_frame *frame
);

void cpu_context_switch(void *new_cpu_state)
{
    /* Here we assume that current_task is the new task (this was done by the scheduler)*/
    struct interrupt_frame *frame = NULL;
    if (!new_cpu_state) {
	    frame = (struct interrupt_frame*)((uint64_t)current_task->ksp - sizeof(struct interrupt_frame) + 8);
    } else {
	    frame = (struct interrupt_frame*)((uint64_t)current_task->kstack);
    }
    struct arch_x86_cpu_state *cpu_state = current_task->cpu_state;
    if (new_cpu_state) {
	    cpu_state = new_cpu_state;
    }
    uint64_t irq_state = cpu_irq_save();

    g_tss.rsp[0] = (uint64_t)current_task->kstack + (PAGE_SIZE * 2);

    /*
    *   setup an interrupt frame on the *new* current process kstack
    *   to 'iretq' into its last cpu state.
    */
    frame->exception.noerr.cs = cpu_state->cs;
    frame->exception.noerr.rflags = cpu_state->rflags;
    frame->exception.noerr.rip = cpu_state->rip;
    frame->exception.noerr.rsp = cpu_state->rsp;
    frame->exception.noerr.ss = cpu_state->ss;
    frame->rax = cpu_state->rax;
    frame->rbx = cpu_state->rbx;
    frame->rcx = cpu_state->rcx;
    frame->rdx = cpu_state->rdx;
    frame->rbp = cpu_state->rbp;
    frame->rdi = cpu_state->rdi;
    frame->rsi = cpu_state->rsi;
    frame->r8 = cpu_state->r8;
    frame->r9 = cpu_state->r9;
    frame->r10 = cpu_state->r10;
    frame->r11 = cpu_state->r11;
    frame->r12 = cpu_state->r12;
    frame->r13 = cpu_state->r13;
    frame->r14 = cpu_state->r14;
    frame->r15 = cpu_state->r15;

    if (new_cpu_state)
	    kfree(new_cpu_state);

    cpu_irq_restore(irq_state);
    __iret_context_switch(cpu_state->ss, frame);
    /* we should not reach here*/
}
