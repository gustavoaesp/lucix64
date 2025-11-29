#include <arch/ioremap.h>

#include <lucix/console.h>
#include <lucix/console_font.h>
#include <lucix/font_psf.h>
#include <lucix/slab.h>
#include <lucix/start.h>
#include <lucix/utils.h>

#include <stddef.h>

struct list_head console_list = LIST_HEAD_INIT(console_list);

void register_console(struct console *con)
{
	list_add(&con->list, &console_list);
}

void unregister_console(struct console *con)
{
	/* TODO for now */
}

void console_putchar(char c)
{
	struct list_head *pos = NULL;
	list_for_each(pos, &console_list) {
		struct console *con = (struct console*) pos;
		if (!con->putchar)
			continue;
		con->putchar(con, c);
	}
}
