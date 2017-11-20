#ifndef GAME_GRAPHICS_H
#define GAME_GRAPHICS_H

#include "snake.h"

int framebuffer_init(Game * game);
void framebuffer_close(Game * game);
void blit_region(Game * game, uint16_t x, uint16_t y, uint16_t width,
		 uint16_t height);
void blit_screen(Game * game);
void draw_rectangle(Game * game, Color color, uint16_t x, uint16_t y,
		    uint16_t width, uint16_t height);
void draw_bitmap(Game * game, uint8_t * src, Color foreground, Color background,
		 uint16_t x, uint16_t y, uint16_t width, uint16_t height,
		 uint16_t scale);
void draw_text(Game * game, String string, Color foreground, Color background,
	       uint16_t x, uint16_t y, uint16_t scale);
void draw_text_centered(Game * game, String string, Color foreground,
			Color background, uint16_t x, uint16_t y,
			uint16_t scale);
void draw_text_right(Game * game, String string, Color foreground,
		     Color background, uint16_t x, uint16_t y, uint16_t scale);

#endif
