#include <lucix/console.h>
#include <lucix/console_font.h>
#include <lucix/font_psf.h>
#include <lucix/slab.h>
#include <lucix/utils.h>
#include <lucix/initcall.h>

#include <arch/framebuffer.h>
#include <arch/ioremap.h>

struct efi_fb_console_private
{
	int cursor_x;
	int cursor_y;
	char *buffer;
	int rows, cols;
	struct psf_font *font;
	uint32_t *fb_address;
	uint32_t fb_pitch;
};

static struct efi_fb_console_private console_data;

static inline void fb_draw_pixel(struct efi_fb_console_private *console_data,
				int x, int y, uint32_t color)
{
	uint32_t y_index = console_data->fb_pitch * y;
	uint8_t* addr = (uint8_t*)console_data->fb_address;
	addr += y_index + x * sizeof(uint32_t);
	*((uint32_t*)addr) = color;
}

static void fb_draw_char(struct efi_fb_console_private *console_data,
			int32_t x, int32_t y, char c, uint32_t fg, uint32_t bg)
{
	struct psf_font *fontp = (struct psf_font*)console_data->font;
	uint8_t *glyphs = ((uint8_t*)console_data->font) + fontp->headersize;
	uint32_t char_offset = c * (fontp->width / 8) * (fontp->height);
	uint8_t *glyph = glyphs + char_offset;

	int pixel_x = x * fontp->width;
	int pixel_y = y * fontp->height;

	for (int i = 0; i < fontp->height; ++i) {
		uint8_t curr_row = glyph[i];
		for (int k = 0; k < fontp->width; ++k) {
			uint32_t draw_color = 0;
			if (0x80 & (curr_row)) {
				draw_color = fg;
			} else {
				draw_color = bg;
			}
			fb_draw_pixel(console_data, pixel_x + k, pixel_y + i, draw_color);
			curr_row <<= 1;
		}
	}
}

static void initialize(struct console *con)
{
	console_data.cursor_x = 0;
	console_data.cursor_y = 0;
	console_data.font = (struct psf_font*)&console_font;
	console_data.rows = efi_fb_data.height / console_data.font->height;
	console_data.cols = efi_fb_data.width / console_data.font->width;
	console_data.fb_pitch = efi_fb_data.pitch;
	console_data.buffer = kmalloc(console_data.rows * console_data.cols, 0);

	console_data.fb_address = ioremap(
		efi_fb_data.phys_addr,
		efi_fb_data.height * efi_fb_data.pitch,
		0
	);

	con->private = &console_data;
}

static void __efi_fb_write_buff(struct efi_fb_console_private *con, int x, int y, char c)
{
	con->buffer[y * con->cols + x] = c;
	fb_draw_char(con, x, y, c, 0xffffffff, 0x00000000);
}

static void __efi_fb_redraw(struct efi_fb_console_private *con)
{
	for (int y = 0; y < con->rows; ++y) {
		for(int x = 0; x < con->cols; ++x) {
			fb_draw_char(
				con, x, y,
				con->buffer[y * con->cols + x],
				0xffffffff,
				0x00000000
			);
		}
	}
}

static void __efi_fb_newline(struct efi_fb_console_private *con)
{
	if(++(con->cursor_y) < con->rows) {
		return;
	}

	for (int i = 0; i < con->rows - 1; ++i) {
		memcpy(
			con->buffer + i * con->cols,
			con->buffer + (i + 1) * con->cols,
			con->cols
		);
	}
	memset(con->buffer + (con->rows - 1), 0, con->cols);

	__efi_fb_redraw(con);
	con->cursor_y--;
}

static void __efi_fb_cursor_sanity(struct efi_fb_console_private *con)
{
	if (con->cursor_x >= con->cols) {
		con->cursor_x = 0;
		__efi_fb_newline(con);
	}
}

static void efi_fb_console_putchar(struct console *con, char c)
{
	struct efi_fb_console_private *private = con->private;
	__efi_fb_cursor_sanity(private);

	switch(c){
	case '\n':
		__efi_fb_write_buff(private, private->cursor_x, private->cursor_y, '\0');
		private->cursor_x = 0;
		__efi_fb_newline(private);
		break;
	case '\r':
		private->cursor_x = 0;
		break;
	default:
		__efi_fb_write_buff(private, private->cursor_x++, private->cursor_y, c);
	}
}

struct console efi_fb_console = {
	.name = "fbcon",
	.private = &console_data,
	.flags = 0,
	.putchar = efi_fb_console_putchar
};

int efi_fb_console_load()
{
	initialize(&efi_fb_console);

	register_console(&efi_fb_console);

	return 0;
}

int efi_fb_console_remove()
{
	return 0;
}

initcall_early(efi_fb_console_load);
