#include <arch/msr.h>

void cpu_get_msr(uint32_t msr, uint32_t* lo, uint32_t* hi)
{
	asm volatile ("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

void cpu_set_msr(uint32_t msr, uint32_t lo, uint32_t hi)
{
	asm volatile ("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}
