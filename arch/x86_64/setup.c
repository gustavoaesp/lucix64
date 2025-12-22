#include <arch/paging.h>
#include <arch/setup.h>
#include <arch/gdt.h>
#include <arch/interrupt.h>
#include <arch/acpi.h>
#include <arch/cpu_state.h>
#include <arch/acpi.h>
#include <arch/apic.h>

void setup_arch()
{
	paging_init();
	setup_gdt(&up_cpu);
	setup_interrupts();
	acpi_init();
	init_apic();
}
