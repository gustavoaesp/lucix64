#include <arch/acpi/hpet.h>
#include <lucix/printk.h>
#include <arch/hpet.h>
#include <arch/paging.h>

void parse_hpet(struct acpi_hpet* hpet)
{
	if (hpet->header.length != sizeof(struct acpi_hpet)) {
		return;
	}

	struct hpet_regs* regs = PA2VA(hpet->address.address);
	hpet_init(regs);
}
