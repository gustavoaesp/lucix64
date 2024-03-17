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

extern uint64_t*	isr_stub_table[256];

void setup_interrupts();

#endif
