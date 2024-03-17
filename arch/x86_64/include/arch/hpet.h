#ifndef _ARCH_HPET_H_
#define _ARCH_HPET_H_
#include <stdint.h>

struct hpet_regs
{
	uint64_t main_caps;
	uint64_t reserved1;
	uint64_t general_cfg;
	uint64_t reserved2;
	uint64_t general_interrupt_status;
	uint64_t reserved3;
};

extern struct hpet_regs* g_hpet_regs;
extern uint64_t* g_hpet_counter;

void hpet_init(struct hpet_regs* regs);

void hpet_wait_millis(uint32_t millis);

#endif
