#include <arch/apic.h>
#include <arch/cpu_state.h>
#include <arch/interrupt.h>
#include <arch/gdt.h>

#include <lucix/printk.h>
#include <lucix/task.h>

#define MAX_IDT_DESCRIPTORS	(256)

static struct interrupt_descriptor idt[MAX_IDT_DESCRIPTORS] __attribute__((aligned(0x10)));
static struct idtr _idtr;

static void __load_idtr()
{
	asm volatile ("lidt %0" : : "m"(_idtr));
	asm volatile ("sti");
}

static void set_descriptor(struct interrupt_descriptor* descriptor, void* function, uint8_t flags)
{
	descriptor->offset_0 = (uint64_t)function & 0xffff;
	descriptor->seg_selector = _KERNEL_CS;
	descriptor->attributes = flags;
	descriptor->offset_1 = ((uint64_t)function >> 16) & 0xffff;
	descriptor->offset_2 = ((uint64_t)function >> 32);
	descriptor->reserved = 0;

	descriptor->ist = 0;
}

extern void jump_to_user(struct interrupt_frame *);

void setup_interrupts()
{
	_idtr.base = (uint64_t)idt;
	_idtr.limit = sizeof(struct interrupt_descriptor) * MAX_IDT_DESCRIPTORS - 1;

	for (int i = 0; i < MAX_IDT_DESCRIPTORS; ++i) {
		uint8_t flags = 0x8E;
		if (i == 0x80) {
			flags = 0xEE;
		}
		set_descriptor(idt + i, isr_stub_table[i], flags);
	}

	__load_idtr();
}

//
void _exception_handler_stub_noerr(struct interrupt_frame* frame)
{
	printf("Interrupt (NOERR)! id: %d\n", frame->interrupt_id);
}

void _exception_handler_stub_err(struct interrupt_frame* frame)
{
	printf("Interrupt (ERR)! id: %d\n", frame->interrupt_id);
	if (frame->interrupt_id == 14) while(1);
}

static int counter = 0;

void __irq_handler_stub(struct interrupt_frame* frame)
{
	if (frame->interrupt_id == 0x20) {
		/*if (counter++ == 100) {
			//printf("IRQ Timer (aprox 1s)\n");
			counter = 0;
		}*/
		apic_write32(APIC_EOI_REG, 0);
		struct arch_x86_cpu_state *cpu = NULL;
		if (current_task) {
			cpu = current_task->cpu_state;
			cpu->cs = frame->exception.noerr.cs;
			cpu->ss = frame->exception.noerr.ss;
			cpu->rax = frame->rax;
			cpu->rbx = frame->rbx;
			cpu->rcx = frame->rcx;
			cpu->rdx = frame->rdx;
			cpu->rbp = frame->rbp;
			cpu->rdi = frame->rdi;
			cpu->rsi = frame->rsi;
			cpu->r8 = frame->r8;
			cpu->r9 = frame->r9;
			cpu->r10 = frame->r10;
			cpu->r11 = frame->r11;
			cpu->r12 = frame->r12;
			cpu->r13 = frame->r13;
			cpu->r14 = frame->r14;
			cpu->r15 = frame->r15;
			cpu->rsp = frame->exception.noerr.rsp;
			cpu->rip = frame->exception.noerr.rip;
			cpu->rflags = frame->exception.noerr.rflags;
		}
		if (sched_irq()) {
			cpu = current_task->cpu_state;
			frame->exception.noerr.cs = cpu->cs;
			frame->exception.noerr.ss = cpu->ss;
			frame->rax = cpu->rax;
			frame->rbx = cpu->rbx;
			frame->rcx = cpu->rcx;
			frame->rdx = cpu->rdx;
			frame->rbp = cpu->rbp;
			frame->rdi = cpu->rdi;
			frame->rsi = cpu->rsi;
			frame->r8 = cpu->r8;
			frame->r9 = cpu->r9;
			frame->r10 = cpu->r10;
			frame->r11 = cpu->r11;
			frame->r12 = cpu->r12;
			frame->r13 = cpu->r13;
			frame->r14 = cpu->r14;
			frame->r15 = cpu->r15;
			frame->exception.noerr.rsp = cpu->rsp;
			frame->exception.noerr.rip = cpu->rip;
			frame->exception.noerr.rflags = cpu->rflags;
			g_tss.rsp[0] = current_task->ksp;

			jump_to_user(frame);
		}

		return;
	} else {
		printf("Other IRQ: %x\n", frame->interrupt_id);
	}
	if (frame->interrupt_id == 0x7f) {
		printf("ISR 0x7f");
	}
	if (frame->interrupt_id == 0x9f) {
		printf("ISR 0x9f");
	}

	if (frame->interrupt_id == 0x80) {
		printf("int 0x80 received:)\n");
		printf("* rax: %d\n", frame->rax);
	}
}
