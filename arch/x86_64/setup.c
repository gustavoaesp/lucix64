#include <arch/paging.h>
#include <arch/setup.h>
#include <arch/gdt.h>
#include <arch/interrupt.h>
#include <arch/acpi.h>

void setup_arch()
{
	paging_init();
	setup_gdt();
	setup_interrupts();
}
