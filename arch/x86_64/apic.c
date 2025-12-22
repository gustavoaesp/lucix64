#include <arch/apic.h>
#include <arch/msr.h>
#include <arch/paging.h>
#include <arch/ioports.h>
#include <arch/hpet.h>
#include <lucix/mm.h>
#include <lucix/printk.h>

#include <stddef.h>

#define IA32_APIC_BASE_MSR	0x1B
#define IA32_APIC_BASE_MSR_BSP	0x100
#define IA32_APIC_BASE_2XAPIC	0x400
#define IA32_APIC_BASE_MSR_ENABLE	0x800

volatile void *apic_regs = NULL;

static uint32_t count = 0;
static uint32_t ticks = 0;

uint32_t *get_apic_base()
{
	uint32_t eax, edx;
	cpu_get_msr(0x1B, &eax, &edx);
	return (uint32_t*)((uint64_t)eax & 0xfffff000);
}

void set_apic_base(uint64_t base)
{
	uint32_t edx = 0;
	uint32_t eax = (uint32_t)(base & 0xfffff0000)
			| IA32_APIC_BASE_MSR_ENABLE;
			//| IA32_APIC_BASE_2XAPIC;

	cpu_set_msr(IA32_APIC_BASE_MSR, eax, edx);
}

static void __disable_pic()
{
	asm volatile (
		"mov	$0xff,	%al\n"
		"out	%eax,	$0xa1\n"
		"out	%eax,	$0x21\n"
	);
}

static void __init_timer()
{
	apic_write32(APIC_DIVIDE_CONF_REG, APIC_TIMER_DIVIDE_BY_8);
	apic_write32(APIC_LVT_TIMER_REG, APIC_TIMER_PERIODIC | 0x20);
	apic_write32(APIC_LVT_INIT_COUNT_REG, 0xffffffff); /* -1 */
}

/*
 *	For io port 0x43 Mode/Command register for the PIT :)
	Bits         Usage
	6 and 7      Select channel :
			0 0 = Channel 0
			0 1 = Channel 1
			1 0 = Channel 2
			1 1 = Read-back command (8254 only)
	4 and 5      Access mode :
			0 0 = Latch count value command
			0 1 = Access mode: lobyte only
			1 0 = Access mode: hibyte only
			1 1 = Access mode: lobyte/hibyte
	1 to 3       Operating mode :
			0 0 0 = Mode 0 (interrupt on terminal count)
			0 0 1 = Mode 1 (hardware re-triggerable one-shot)
			0 1 0 = Mode 2 (rate generator)
			0 1 1 = Mode 3 (square wave generator)
			1 0 0 = Mode 4 (software triggered strobe)
			1 0 1 = Mode 5 (hardware triggered strobe)
			1 1 0 = Mode 2 (rate generator, same as 010b)
			1 1 1 = Mode 3 (square wave generator, same as 011b)
	0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
*/

static void pit_calibration()
{
	int tmp = 0;
	// binary 10110010
	out8(0x43, 0xB2);
	/* 1193180/100 Hz = 11931 = 2e9bh */
	out8(0x42, 0x9b);
	tmp = in8(0x60); // short and small delay
	out8(0x42, 0x2e);
	/* RESET apic timer */
	apic_write32(APIC_LVT_INIT_COUNT_REG, 0xffffffff);

	printf("Setting PIT calibration\n");
	for(;;) {
		uint32_t pit = 0;
		pit = in8(0x42);
		pit |= in8(0x42) << 8;
		if (! (pit)) {
			break;
		}
	}

	ticks = 0xffffffff - apic_read32(APIC_LVT_CURR_COUNT_REG);
	printf("PIT ticks counter: %d\n", ticks);
	apic_write32(APIC_LVT_INIT_COUNT_REG, ticks);
}

static void hpet_calibration()
{
	apic_write32(APIC_LVT_INIT_COUNT_REG, 0xffffffff);

	hpet_wait_millis(10);

	ticks = 0xffffffff - apic_read32(APIC_LVT_CURR_COUNT_REG);
	printf("HPET ticks counter: %d\n", ticks);
	apic_write32(APIC_LVT_INIT_COUNT_REG, ticks);
}

void init_apic()
{
	uint64_t apic_base = (uint64_t)get_apic_base();

	set_apic_base(apic_base);

	apic_regs = PA2VA((uint64_t)apic_base);
	apic_write32(APIC_SPURIOUS_INT_VECTOR_REG, 0x1ff);
	__disable_pic();
	__init_timer();

	if (!g_hpet_regs) {
		pit_calibration();
	} else {
		hpet_calibration();
	}
}

void apic_write32(uint32_t reg_offset, uint32_t val)
{
	uint32_t *reg = (uint32_t*)(apic_regs + reg_offset);
	*reg = val;
}

uint32_t apic_read32(uint32_t reg_offset)
{
	uint32_t *reg = (uint32_t*)(apic_regs + reg_offset);
	return *reg;
}
