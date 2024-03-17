#ifndef _MSR_H_
#define _MSR_H_

#include <stdint.h>

void cpu_get_msr(uint32_t msr, uint32_t* lo, uint32_t* hi);
void cpu_set_msr(uint32_t msr, uint32_t lo, uint32_t hi);

#endif
