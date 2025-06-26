#ifndef _LUCIX_ARCH_GENERIC_H_
#define _LUCIX_ARCH_GENERIC_H_

uint64_t cpu_irq_save();
void cpu_irq_restore(uint64_t irq_state);

#endif