#include <stddef.h>
#include "syscall.h"

static char *str = "Hello from userspace!!\n";

void _entry()
{
	write(1, str, 24);

	for(;;){}
}
