#include <arch/acpi.h>
#include <arch/acpi/madt.h>
#include <arch/acpi/mcfg.h>
#include <arch/acpi/hpet.h>
#include <arch/paging.h>

#include <lucix/compiler.h>
#include <lucix/printk.h>
#include <lucix/utils.h>
#include <lucix/vm.h>
#include <stddef.h>

const char* rsdp_signature = "RSD PTR ";

struct rspd_descriptor2 *g_acpi_rsdp_desc;
struct acpi_xsdt* g_acpi_xsdt;

static struct rspd_descriptor* find_rspd(void)
{
	for (char* ptr = (char*)0xFFFF800000000000; ptr < (char*)0xFFFF800000100000; ptr += 16) {
		if(memcmp(ptr, rsdp_signature, 8)) {
			continue;
		}
		return (struct rspd_descriptor*)ptr;
	}
	return NULL;
}

static uint32_t __acpi_match(const void *table, const char* signature)
{
	return (!memcmp(table, signature, 4));
}

void parse_xsdt ()
{
	uint32_t num_pointers = (g_acpi_xsdt->header.length - sizeof(struct sdt_header))
				/ sizeof(uint64_t);

	/*
	for (int i = 0; i < num_pointers; ++i) {
		char sig_print[5] = {};
		char *sig_table = (char*)PA2VA(g_acpi_xsdt->pointers[i]);
		memcpy(sig_print, sig_table, 4);
		sig_print[4] = 0;
		printf("Found table: %s\n", sig_print);
	}*/

	for (int i = 0; i < num_pointers; ++i) {
		void *table = (void*)PA2VA(g_acpi_xsdt->pointers[i]);
		if (__acpi_match(table, "APIC")) {
			//parse_madt(table);
		}
		if (__acpi_match(table, "MCFG")) {
			parse_mcfg(table);
		}
		if (__acpi_match(table, "HPET")) {
			parse_hpet(table);
		}
	}
}

int acpi_init()
{
	if (!g_acpi_rsdp_desc) {
		g_acpi_rsdp_desc = (struct rspd_descriptor2*)find_rspd();
	}
	g_acpi_xsdt = PA2VA(g_acpi_rsdp_desc->xsdt_address);

	parse_xsdt();

	return 0;
}
