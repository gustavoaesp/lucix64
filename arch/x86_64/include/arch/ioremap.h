#ifndef _ARCH_IOREMAP_H_
#define _ARCH_IOREMAP_H_
#include <stdint.h>

void *ioremap(uint64_t phys_addr, uint64_t size, uint32_t flags);

#endif
