#ifndef _LUCIX_ARCH_GENERIC_H_
#define _LUCIX_ARCH_GENERIC_H_

#include <stdint.h>

uint64_t cpu_irq_save();
void cpu_irq_restore(uint64_t irq_state);

void cpu_force_cli();
void cpu_force_sti();

#endif
