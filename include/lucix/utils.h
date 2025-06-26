#ifndef _LUCIX_UTILS_H_
#define _LUCIX_UTILS_H_
#include <stddef.h>
#include <stdint.h>

#define	BIT(a)	(1 << a)

int memcmp(const char* a, const char* b, uint32_t bytes);
void memcpy(void *dst, const void *src, size_t bytes);
void memset(void *dst, uint8_t , size_t bytes);
char *strcpy(char*, const char*);
char *strncpy(char*, const char*, size_t);

#endif
