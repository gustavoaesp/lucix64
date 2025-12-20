#include <lucix/cpu.h>

static struct cpu up_cpu = {
	.runqueue.tasks = LIST_HEAD_INIT(up_cpu.runqueue.tasks)
};

struct cpu *cpu_get_cpu()
{
	return &up_cpu;
}
