#include <arch/hpet.h>
#include <lucix/printk.h>

struct hpet_regs* g_hpet_regs;
uint64_t *g_hpet_counter;

uint64_t femto = 1000000000000000;

void hpet_init(struct hpet_regs* regs)
{
	g_hpet_regs = regs;
	void *tmp_ptr = regs;
	g_hpet_counter = tmp_ptr + 0xf0;

	printf("HPET femtos: %d\n", regs->main_caps >> 32);
	printf("HPET freque: %d\n", femto / (regs->main_caps >> 32));

}

void hpet_wait_millis(uint32_t millis)
{
	uint64_t freq = femto / (g_hpet_regs->main_caps >> 32);
	uint64_t count_max = freq * millis / 1000;

	*g_hpet_counter = 0;
	g_hpet_regs->general_cfg = 1;

	while (*g_hpet_counter <= count_max);

	g_hpet_regs->general_cfg = 0;
}
