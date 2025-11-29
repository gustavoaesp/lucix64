#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include <stdint.h>

#include <lucix/list.h>

#define CONSOLE_SERIAL	0x01
#define CONSOLE_COLOR	0x02

struct console {
	struct list_head list;
	const char *name;
	void (*putchar)(struct console *, char);
	uint32_t flags;

	/* private data the console might need */
	void *private;
};

void register_console(struct console *);
void unregister_console(struct console *);

extern void console_putchar(char c);

#endif
