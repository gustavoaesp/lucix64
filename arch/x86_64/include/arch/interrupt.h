#ifndef _ARCH_INTERRUPT_H_
#define _ARCH_INTERRUPT_H_

#include <lucix/compiler.h>

struct interrupt_descriptor
{
	uint16_t offset_0;
	uint16_t seg_selector;
	uint8_t ist;
	uint8_t attributes;
	uint16_t offset_1;
	uint32_t offset_2;
	uint32_t reserved;
} __packed;

struct idtr {
	uint16_t limit;
	uint64_t base;
} __packed;

struct interrupt_exception
{
	uint64_t	rip;
	uint64_t	cs;
	uint64_t	rflags;
	uint64_t	rsp;
	uint64_t	ss;
} __packed;

struct interrupt_err_exception {
	uint64_t err_code;
	struct interrupt_exception exception;
};

struct interrupt_frame
{
	uint64_t	interrupt_id;
	uint64_t	r8;
	uint64_t	r9;
	uint64_t	r10;
	uint64_t	r11;
	uint64_t	r12;
	uint64_t	r13;
	uint64_t	r14;
	uint64_t	r15;
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


extern uint64_t*	isr_stub_table[256];

void setup_interrupts();

#endif
