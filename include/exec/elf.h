#ifndef _LUCIX_ELF_H_
#define _LUCIX_ELF_H_
#include <lucix/compiler.h>
int is_valid_elf(void *elf, uint64_t size);

void load_elf(void* elf, uint64_t size);

#endif
