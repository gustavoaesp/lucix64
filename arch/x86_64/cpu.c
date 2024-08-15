#include <arch_generic/cpu.h>

void cpu_cli()
{
    asm volatile ("cli");
}

void cpu_sti()
{
    asm volatile ("sti");
}