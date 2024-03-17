#include <arch/gdt.h>

static struct gdt_entry gdt_entries[GDT_ENTRIES];

void setup_gdt(void)
{
	gdt_entries[_GDT_NULL] = (struct gdt_entry){ 0 };
	gdt_entries[_KERNEL_CS_N] = GDT_ENTRY64(GDT_TYPE_EXECUTABLE | GDT_TYPE_READABLE, 0, 0xFFFFFFFF, GDT_DPL_KERNEL);
	gdt_entries[_KERNEL_DS_N] = GDT_ENTRY64(GDT_TYPE_WRITABLE, 0, 0xFFFFFFFF, GDT_DPL_KERNEL);
	gdt_entries[_USER_CS_N] = GDT_ENTRY64(GDT_TYPE_EXECUTABLE | GDT_TYPE_READABLE, 0, 0xFFFFFFFF, GDT_DPL_USER);
	gdt_entries[_USER_DS_N] = GDT_ENTRY64(GDT_TYPE_WRITABLE, 0, 0xFFFFFFFF, GDT_DPL_USER);

	/* Setup CPU-local variable */
	//gdt_entries[_CPU_VAR_N] = GDT_ENTRY(GDT_TYPE_WRITABLE, (uintptr_t)&c->cpu, sizeof(&c->cpu) - 1, GDT_DPL_KERNEL);

	/* Setup CPU-tss */
	//gdt_entries[_GDT_TSS_N] = GDT_ENTRY16(GDT_STS_T32A, (uintptr_t)&c->tss, sizeof(c->tss) - 1, GDT_DPL_KERNEL);
	//gdt_entries[_GDT_TSS_N].des_type = 0;

	gdt_flush(gdt_entries, sizeof(struct gdt_entry) * 5);
	_gdt_reload_segments();

	// lets hope we reach here before triple-fault
}


