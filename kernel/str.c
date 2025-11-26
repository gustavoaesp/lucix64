#include <lucix/string.h>
#include <stddef.h>

int strcmp(const char *s1, const char *s2)
{
	for (; *s1 && *s2; s1++, s2++)
		if (*s1 > *s2)
			return 1;
		else if (*s1 < *s2)
			return -1;

	if (*s1)
		return 1;
	if (*s2)
		return -1;

	return 0;
}

int strlen(const char *s)
{
	int count = 0;
	while (*s++) count++;
	return count;
}

char *strrchr(const char *s, char c) {
	const char *p = NULL;

	for (;;) {
		if (*s == (char)c)
			p = s;
		if (*s++ == '\0')
			return (char *)p;
	}
}
