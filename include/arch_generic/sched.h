#ifndef _ARCH_GENERIC_TASK_RET_H_
#define _ARCH_GENERIC_TASK_RET_H_

#include <lucix/cpu.h>
#include <lucix/task.h>

/*
*   Initializes a task struct for a kernel task entry point
*/
void cpu_ktask_setup(struct task *t, void (*__entry)(void*), void *args);

/*
 *  Initialize a cpu state for execve
 * */
void cpu_setup_process(void **cpu_state, uintptr_t entry);

/*
*   Does not return, context switches to the last state of current_task
*/
void cpu_context_switch(void *cpu_state);

/*
 *	Switch from one task to other (context switch)
 * */
void cpu_switch_to(struct task *prev, struct task *next);

/*
 *	Do required schedule stuff for the cpu
 * */
void cpu_schedule(struct task *prev, struct task *next, struct cpu *);

#endif
