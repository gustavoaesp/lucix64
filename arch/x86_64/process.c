#include <arch_generic/sched.h>

#include <arch/paging.h>
#include <arch/gdt.h>
#include <arch/cpu_state.h>
#include <arch_generic/cpu.h>

#include <lucix/cpu.h>
#include <lucix/vma.h>
#include <lucix/mmap.h>

#define USER_STACK_SIZE ((uint64_t)(2048 * PAGE_SIZE))
#define USER_STACK_BOTTOM	(0x00007fffff000000)

void cpu_setup_process(void **cpu_state, uintptr_t entry)
{
	struct cpu *cpu = cpu_get_cpu();
	struct task *current_task = NULL; 
	if (cpu->current) {
		current_task = cpu->current;
	} else {
		/* fail miserably */
		return;
	}
	/* Stack */
	struct arch_x86_cpu_state *new_cpu_state = kmalloc(sizeof(struct arch_x86_cpu_state), 0);

	uintptr_t stack_start = do_mmap(current_task->mm,
		NULL,
		USER_STACK_BOTTOM,
		USER_STACK_SIZE,
		VM_READ | VM_WRITE | VM_USER,
		VM_FLAG_ANON,
		VMA_STACK,
		0
	);

	memset(new_cpu_state, 0, sizeof(struct arch_x86_cpu_state));

	new_cpu_state->cs = _USER_CS | 3;
	new_cpu_state->ss = _USER_DS | 3;
	new_cpu_state->rsp = stack_start + (USER_STACK_SIZE);
	new_cpu_state->rip = entry;
	new_cpu_state->rflags = 0x202;

	*cpu_state = new_cpu_state;
}
