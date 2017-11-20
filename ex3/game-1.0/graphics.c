#include "graphics.h"
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FB_SIZE (DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(Color))

#define set_pixel_i(fb, i, color) COLOR_SET(fb[i], color)

int framebuffer_init(Game * game)
{
	game->framebuffer_fp = open("/dev/fb0", O_RDWR);
	game->framebuffer_width = DISPLAY_WIDTH;
	game->framebuffer_height = DISPLAY_HEIGHT;
	if (game->framebuffer_fp < 0) {
		perror("open framebuffer");
		return -1;
	}

	game->framebuffer =
	    mmap(NULL, FB_SIZE, PROT_WRITE, MAP_SHARED, game->framebuffer_fp,
		 0);

	if (game->framebuffer == MAP_FAILED) {
		perror("mmap");
		close(game->framebuffer_fp);
		return -1;
	}

	memset(game->framebuffer, 0, FB_SIZE);
	blit_screen(game);

	return 0;
}

void framebuffer_close(Game * game)
{
	munmap(game->framebuffer, FB_SIZE);
	close(game->framebuffer_fp);
}

void blit_region(Game * game, uint16_t x, uint16_t y, uint16_t width,
		 uint16_t height)
{
	if (!game->disable_blit) {
		struct fb_copyarea rect;

		rect.dx = x;
		rect.dy = y;
		rect.width = width;
		rect.height = height;

		ioctl(game->framebuffer_fp, 0x4680, &rect);
	}
}

void blit_screen(Game * game)
{
	blit_region(game, 0, 0, game->framebuffer_width,
		    game->framebuffer_height);
}

void draw_rectangle(Game * game, Color color, uint16_t x, uint16_t y,
		    uint16_t width, uint16_t height)
{
	uint16_t i, j;

	assert(x + width < game->framebuffer_width);
	assert(y + height < game->framebuffer_height);

	for (j = y; j < y + height; j++) {
		for (i = x; i < x + width; i++) {
			int n = j * game->framebuffer_width + i;
			set_pixel_i(game->framebuffer, n, color);
		}
	}

	blit_region(game, x, y, width, height);
}

void draw_bitmap(Game * game, uint8_t * src, Color foreground, Color background,
		 uint16_t x, uint16_t y, uint16_t width, uint16_t height,
		 uint16_t scale)
{
	uint16_t i, j;

	for (j = y; j < y + height * scale; j++) {
		for (i = x; i < x + width * scale; i++) {
			uint16_t glyph_x, glyph_y, bit;
			uint8_t b;
			int n;

			glyph_x = ((i - x) / scale);
			glyph_y = ((j - y) / scale);

			bit = glyph_y * width + glyph_x;
			b = src[bit / 8] & (1 << (7 - (bit % 8)));
			n = j * game->framebuffer_width + i;

			if (b == 0) {
				set_pixel_i(game->framebuffer, n, foreground);
			} else {
				set_pixel_i(game->framebuffer, n, background);
			}
		}
	}
}

void draw_text(Game * game, String string, Color foreground, Color background,
	       uint16_t x, uint16_t y, uint16_t scale)
{
	uint16_t i, dx;

	dx = x;

	for (i = 0; i < string.length; ++i) {
		uint8_t c = string.text[i] & 0x7f;
		draw_bitmap(game, &font_data[3 * (uint16_t) c], foreground,
			    background, dx, y, 4, 6, scale);
		dx += 4 * scale;
	}
	blit_region(game, x, y, string.length * scale * 4, scale * 6);
}

void draw_text_centered(Game * game, String string, Color foreground,
			Color background, uint16_t x, uint16_t y,
			uint16_t scale)
{
	uint16_t width = string.length * scale * 4;
	draw_text(game, string, foreground, background, x - width / 2, y,
		  scale);
}

void draw_text_right(Game * game, String string, Color foreground,
		     Color background, uint16_t x, uint16_t y, uint16_t scale)
{
	uint16_t width = string.length * scale * 4;
	draw_text(game, string, foreground, background, x - width, y, scale);
}
