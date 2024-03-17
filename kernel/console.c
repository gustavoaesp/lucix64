#include <arch/ioremap.h>

#include <lucix/console_font.h>
#include <lucix/font_psf.h>
#include <lucix/slab.h>
#include <lucix/start.h>
#include <lucix/utils.h>

#include <stddef.h>

static uint32_t* fb_address = NULL;
static uint32_t fb_width, fb_height, fb_pitch;

static inline void fb_draw_pixel(int x, int y, uint32_t color)
{
	uint32_t y_index = fb_pitch * y;
	uint8_t* addr = (uint8_t*)fb_address;
	addr += y_index + x * sizeof(uint32_t);
	*((uint32_t*)addr) = color;
}

static void fb_draw_char(uint8_t* font, int32_t x, int32_t y, char c, uint32_t fg, uint32_t bg)
{
	struct psf_font *fontp = (struct psf_font*)font;
	uint8_t *glyphs = font + fontp->headersize;
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
			fb_draw_pixel(pixel_x + k, pixel_y + i, draw_color);
			curr_row <<= 1;
		}
	}
}

static char *buffer;
static int cw, ch;
static int cursor_x, cursor_y;

static struct psf_font* main_font = NULL;

struct kernel_buffer {
	char *buf;
	uint32_t start;
	uint32_t end;
};

void console_init(struct framebuffer_data* framebuffer)
{
	main_font = (struct psf_font*)console_font;

	cursor_x = 0;
	cursor_y = 0;

	cw = framebuffer->width / main_font->width;
	ch = framebuffer->height / main_font->height;

	buffer = kmalloc(cw * ch, 0);

	//
	fb_width = framebuffer->width;
	fb_height = framebuffer->height;
	fb_pitch = framebuffer->pitch;

	fb_address = ioremap(
		framebuffer->phys_addr,
		fb_height * fb_pitch,
		0
	);
}

static void _write_buff(int x, int y, char c)
{
	buffer[y * cw + x] = c;
	fb_draw_char((uint8_t*)main_font, x, y, c, 0xffffffff, 0x00000000);
}

static void _redraw()
{
	for (int y = 0; y < ch; ++y) {
		for(int x = 0; x < cw; ++x) {
			fb_draw_char((uint8_t*)main_font, x, y, buffer[y * cw + x], 0xffffffff, 0x00000000);
		}
	}
}

static void _newline()
{
	if(++cursor_y < ch) {
		return;
	}

	for (int i = 0; i < ch - 1; ++i) {
		memcpy(buffer + i * cw, buffer + (i + 1) * cw, cw);
	}

	_redraw();
	cursor_y--;
}

static void _cursor_sanity()
{
	if (cursor_x >= cw) {
		cursor_x = 0;
		_newline();
	}
}

void console_putchar(char c)
{
	_cursor_sanity();

	switch(c){
	case '\n':
		_write_buff(cursor_x, cursor_y, '\0');
		cursor_x = 0;
		_newline();
		break;
	case '\r':
		cursor_x = 0;
		break;
	default:
		_write_buff(cursor_x++, cursor_y, c);
	}
}

void console_print(const char *str)
{
	for(;*str;str++) {
		console_putchar(*str);
	}
}
