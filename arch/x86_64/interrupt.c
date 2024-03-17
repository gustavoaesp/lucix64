#include <arch/interrupt.h>
#include <arch/gdt.h>
#include <lucix/printk.h>
#include <arch/apic.h>

#define MAX_IDT_DESCRIPTORS	(256)

static struct interrupt_descriptor idt[MAX_IDT_DESCRIPTORS] __attribute__((aligned(0x10)));
static struct idtr _idtr;

struct interrupt_exception
{
	uint64_t	rip;
	uint64_t	cs;
	uint64_t	rflags;
	uint64_t	rsp;
} __packed;

struct interrupt_err_exception {
	uint64_t err_code;
	struct interrupt_exception exception;
};

struct interrupt_frame
{
	uint64_t	interrupt_id;
	uint64_t	rdi;
	uint64_t	rsi;
	uint64_t	rbp;
	uint64_t	rbx;
	uint64_t	rdx;
	uint64_t	rcx;
	uint64_t	rax;
	union {
		struct interrupt_err_exception err;
		struct interrupt_exception noerr;
	} exception;
} __packed;

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
		set_descriptor(idt + i, isr_stub_table[i], 0x8E);
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
}

static int counter = 0;

void __irq_handler_stub(struct interrupt_frame* frame)
{
	if (frame->interrupt_id == 0x20) {
		if (counter++ == 100) {
			//printf("IRQ Timer (aprox 1s)\n");
			counter = 0;
		}

		apic_write32(APIC_EOI_REG, 0);
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
	}
}
