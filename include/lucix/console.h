#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include <lucix/start.h>

void console_init(struct framebuffer_data *framebuffer);

extern void console_putchar(char c);

void console_print(const char* str);

#endif
