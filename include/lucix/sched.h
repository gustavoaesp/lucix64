#ifndef _SCHED_H_
#define _SCHED_H_

extern struct task task_init;

extern void cpu_idle();

/*
*   
*/
void sched_init();

/*
*   Called on each timer 0x20 interrupt
*/
void sched_irq();

/*
 *	Called by each cpu on initialization
 * */
void sched_per_cpu_init();

/*
*   Creates a kernel task with the given entry point
*/
struct task *mk_kernel_task(void (*)(void*), void *args);

/*
 *	Add a task to the scheduler
 * */
int sched_add_task(struct task *);

/*
 *	Start the scheduler with the first task
 * */
void sched_start();

/*
 *	Switch to the next available task in the runnable queue
 **/
void schedule();


#endif
