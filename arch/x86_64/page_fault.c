#include <arch/interrupt.h>
#include <arch/page_fault.h>
#include <arch/paging.h>
#include <lucix/page_fault.h>
#include <lucix/printk.h>

void __int_page_fault(struct interrupt_frame *frame)
{
	uint32_t flags = 0;
	uint64_t addr = __get_cr2();
	flags |= (frame->exception.err.err_code & 0x01) ? PGFAULT_PROTECTION : 0;
	flags |= (frame->exception.err.err_code & 0x02) ? PGFAULT_WRITE : 0;
	flags |= (frame->exception.err.err_code & 0x04) ? PGFAULT_USR : 0;
	flags |= (frame->exception.err.err_code & 0x10) ? PGFAULT_INSTRUCTION : 0;
	printf("Handling addr %p\n", addr);
	kernel_page_fault(addr, flags, frame->exception.err.exception.rip);
}
