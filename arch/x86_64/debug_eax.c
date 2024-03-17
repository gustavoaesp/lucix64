#include <arch/debug_eax.h>

void debug_eax(uint64_t eax, uint64_t ebx)
{
	asm volatile ("hlt");
}
