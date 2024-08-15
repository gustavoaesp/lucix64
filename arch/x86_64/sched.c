#include <arch_generic/task_ret.h>
#include <arch_generic/cpu.h>

#include <arch/cpu_state.h>
#include <arch/gdt.h>
#include <arch/interrupt.h>

#include <lucix/task.h>

void cpu_task_setup(struct task *t, void *cpu_state)
{
    struct arch_x86_cpu_state *cpu = cpu_state;
    if (!cpu) {
        t->cpu_state = cpu = kmalloc(sizeof(struct arch_x86_cpu_state), 0);
    }

    memset(cpu, 0, sizeof(struct arch_x86_cpu_state));

    cpu->cs = _USER_CS | 3;
    cpu->ss = _USER_DS | 3;
    cpu->rip = t->entry;
    cpu->rsp = 0x00007ffffffffff8;
}

extern void __iret_context_switch(
    uint32_t ss,
    struct interrupt_frame *frame
);

void cpu_context_switch()
{
    /* Here we assume that current_task is the new task (this was done by the scheduler)*/
    struct interrupt_frame *frame =
        (struct interrupt_frame*)((uint64_t)current_task->ksp - sizeof(struct interrupt_frame));
    struct arch_x86_cpu_state *cpu_state = current_task->cpu_state;
    cpu_cli();

    g_tss.rsp[0] = current_task->kstack_top;

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

    cpu_sti();
    __iret_context_switch(cpu_state->ss, frame);
    /* we should not reach here*/
}
