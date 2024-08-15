#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include <stdint.h>

#include <lucix/start.h>

struct console {
    void (*write)(const char*, uint32_t);
    uint32_t flags;
};

void console_init(struct framebuffer_data *framebuffer);

extern void console_putchar(char c);

void console_print(const char* str);

#endif