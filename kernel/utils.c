#include <lucix/utils.h>

int memcmp(const void* a, const void*b, uint32_t bytes)
{
	int sum = 0;
	for (int i = 0; i < bytes; ++i) {
		sum += ((const uint8_t*)a)[i] - ((const uint8_t*)b)[i];
		if (sum)
			return sum;
	}

	return sum;
}

void memcpy(void *dst, const void *src, size_t size)
{
	uint8_t* dst_ptr = dst;
	const uint8_t* src_ptr = src;
	for (size_t i = 0; i < size; ++i) {
		dst_ptr[i] = src_ptr[i];
	}
}

void memset(void *dst, uint8_t val, size_t bytes)
{
	for (size_t i = 0; i < bytes; ++i) {
		uint8_t* b = dst + i;
		*b = val;
	}
}

char *strcpy(char* dest, const char *b)
{
	char* s_ptr = dest;
	while ((*dest++ = *b++));

	return s_ptr;
}

char *strncpy(char *dst, const char *src, size_t ncount)
{
	char *s_ptr = dst;
	for (int i = 0; i < ncount; ++i) {
		dst[i] = src[i];
		if (!dst[i]) {
			break;
		}
	}
	return s_ptr;
}
