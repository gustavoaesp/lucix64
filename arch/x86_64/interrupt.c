#include <arch/apic.h>
#include <arch/cpu_state.h>
#include <arch/interrupt.h>
#include <arch/gdt.h>
#include <arch/paging.h>
#include <arch_generic/cpu.h>

#include <lucix/printk.h>
#include <lucix/task.h>
#include <lucix/sched.h>
#include <lucix/page_fault.h>
#include <lucix/syscall.h>

#define MAX_IDT_DESCRIPTORS	(256)

static struct interrupt_descriptor idt[MAX_IDT_DESCRIPTORS] __attribute__((aligned(0x10)));
static struct idtr _idtr;

void custom_print(uint64_t val)
{
	printf("Custom print: %p\n", val);
}

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
	uint64_t irq_state = cpu_irq_save();
	printf("Interrupt (ERR)! id: %d\n", frame->interrupt_id);
		printf("err %x\n", frame->exception.err.err_code);
		printf("rsp: %p\n", frame->exception.err.exception.rsp);
		printf("rip: %p\n", frame->exception.err.exception.rip);
		printf(" ss: %p\n", frame->exception.err.exception.ss);
		printf(" cs: %p\n", frame->exception.err.exception.cs);
		printf(" ts: %p\n", g_tss.rsp[0]);
	if (frame->interrupt_id == 14) {
		/*printf("addr: %p\n", __get_cr2());
		printf("rip:  %p\n", frame->exception.err.exception.rip);*/
		/*for(;;) {
			asm volatile ("hlt");
		}*/
		uint32_t flags = 0;
		uint64_t addr = __get_cr2();
		flags |= (frame->exception.err.err_code & 0x01) ? PGFAULT_PROTECTION : 0;
		flags |= (frame->exception.err.err_code & 0x02) ? PGFAULT_WRITE : 0;
		flags |= (frame->exception.err.err_code & 0x04) ? PGFAULT_USR : 0;
		flags |= (frame->exception.err.err_code & 0x10) ? PGFAULT_INSTRUCTION : 0;
		printf("Handling addr %p\n", __get_cr2());
		kernel_page_fault(addr, flags, frame->exception.err.exception.rip);
		cpu_irq_restore(irq_state);
		//printf("Reading: %x\n", *((uint32_t*)addr));
		return;
	}
	if (frame->interrupt_id == 13) {
		while(1);
	}
	if (frame->interrupt_id == 12) {
		while(1);
	}
	cpu_irq_restore(irq_state);
}

/*static int counter = 0;*/

void __irq_handler_stub(struct interrupt_frame* frame)
{
	if (frame->interrupt_id == 0x20) {
		/*if (counter++ == 100) {
			printf("Timer IRQ (aprox 1s)\n");
			counter = 0;
		}*/
		if (current_task) {
			struct arch_x86_cpu_state *cpu = current_task->cpu_state;
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
			if (cpu->cs == _KERNEL_CS) {
				current_task->ksp = cpu->rsp;
			} else {
				current_task->ksp = (uint64_t)current_task->kstack + current_task->kstack_size;
			}
		}
		apic_write32(APIC_EOI_REG, 0);
		sched_irq();

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
		int64_t syscall_n = frame->rax;
		if (syscall_n >= SYSCALLS_NR) {
			return;
		}
		frame->rax = syscall_table[syscall_n](
			frame->rdi,
			frame->rsi,
			frame->rdx,
			frame->r10,
			frame->r8,
			frame->r9
		);
	}
}
