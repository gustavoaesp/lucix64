#include <lucix/sched.h>

void cpu_idle()
{
	for(;;) {
		asm volatile ("hlt");
	}
}
