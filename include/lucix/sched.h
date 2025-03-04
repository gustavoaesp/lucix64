#ifndef _SCHED_H_
#define _SCHED_H_

/*
*   
*/
void sched_init();

/*
*   Make the scheduler context switch into the first available process
*/
void sched_enable_irq();

/*
*   Called on each timer 0x20 interrupt
*/
void sched_irq();

/*
*   Creates a kernel task with the given entry point
*/
void sched_mk_kernel_task(void (*)(void*), void *args);


#endif