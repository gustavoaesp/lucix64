#ifndef _ARCH_GENERIC_TASK_RET_H_
#define _ARCH_GENERIC_TASK_RET_H_

#include <lucix/task.h>

/*
*   Initializes a task struct for a kernel task entry point
*/
void cpu_ktask_setup(struct task *t, void (*__entry)(void*), void *args);

/*
*   Does not return, context switches to the last state of current_task
*/
void cpu_context_switch();

#endif