#ifndef _ARCH_EARLY_PAGING_H_
#define _ARCH_EARLY_PAGING_H_

extern uint64_t pml4[512];

void setup_upper(uint64_t kernel_phys_address);

#endif
