#include "snake.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define set_pixel_i(fb, i, color) COLOR_SET(fb[i], color)

void blit_region(Game * game, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
	struct fb_copyarea rect;

	rect.dx = x;
	rect.dy = y;
	rect.width = width;
	rect.height = height;

	ioctl(game->framebuffer_fp, 0x4680, &rect);
}

void blit_screen(Game * game)
{
	blit_region(game, 0, 0, game->framebuffer_width, game->framebuffer_height);
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

void set_tile(Game * game, int x, int y, uint8_t tile)
{
	uint8_t mask, val;
	int i;
	Color color;

	assert(x < game->board.width);
	assert(y < game->board.height);

	assert(tile <= 0xf);
	mask = 0xf;
	val  = tile;
	i = (y * game->board.width + x);

	if (i % 2 != 0) {
		mask <<= 4;
		val <<= 4;
	}

	game->board.tiles[i / 2] = (game->board.tiles[i / 2] & ~mask) | val;

	COLOR_SET(color, COLOR(0, 0, 0));

	if (tile == TILE_FRUIT) {
		COLOR_SET(color, RED);
	} else if (tile >= TILE_SNAKE_LEFT && tile <= TILE_SNAKE_DOWN) {
		COLOR_SET(color, BLUE);
	}

	draw_rectangle(game, color, x * 10 + 1, y * 10 + 1, 8, 8);
}

uint8_t get_tile(Game * game, uint8_t x, uint8_t y)
{
	uint8_t tile;
	int i;

	i = (y * game->board.width + x);
	tile = game->board.tiles[i / 2];

	if (i % 2 == 0) {
		tile &= 0xf;
	} else {
		tile >>= 4;
	}

	return tile;
}

vec2 move_and_wrap(GameBoard * board, vec2 pos, uint8_t dir)
{
	if (dir == DIR_LEFT) {
		if (Vx(pos) == 0) {
			return V2(board->width - 1, Vy(pos));
		} else {
			return v2_sub(pos, V2(1, 0));
		}
	} else if (dir == DIR_RIGHT) {
		if (Vx(pos) >= board->width - 1) {
			return V2(0, Vy(pos));
		} else {
			return v2_add(pos, V2(1, 0));
		}
	} else if (dir == DIR_UP) {
		if (Vy(pos) == 0) {
			return V2(Vx(pos), board->height - 1);
		} else {
			return v2_sub(pos, V2(0, 1));
		}
	} else if (dir == DIR_DOWN) {
		if (Vy(pos) >= board->height - 1) {
			return V2(Vx(pos), 0);
		} else {
			return v2_add(pos, V2(0, 1));
		}
	}
	return V2(0, 0);
}

uint8_t input_to_dir(uint8_t input) {
	if (input & 0x1) {
		return DIR_LEFT;
	} else if (input & 0x2) {
		return DIR_UP;
	} else if (input & 0x4) {
		return DIR_RIGHT;
	} else if (input & 0x8) {
		return DIR_DOWN;
	}

	return 0;
}

void place_fruit(Game * game)
{
	uint16_t available_tiles;
	int r;
	uint16_t selected;
	uint16_t x, y, i;

	r = rand();

	available_tiles = game->board.width * game->board.height - game->board.snake_segments;
	selected = r % available_tiles;

	i = 0;
	for (y = 0; y < game->board.height; ++y) {
		for (x = 0; x < game->board.width; ++x) {
			uint8_t tile = get_tile(game, x, y);
			if (tile < TILE_SNAKE_LEFT || tile > TILE_SNAKE_DOWN) {
				if (i == selected) {
					set_tile(game, x, y, TILE_FRUIT);
					return;
				}

				i += 1;
			}
		}
	}

	print_error("game", "Could not place fruit");
}

void game_update_and_render(Game * game)
{
	/* First time initializatoin */
	if (!game->initialized) {
		game->board.width = DISPLAY_WIDTH / 10;
		game->board.height = DISPLAY_HEIGHT / 10;

		game->board.round_initialized = false;
		game->initialized = true;
	}

	/* Round start initialization */
	if (!game->board.round_initialized) {
		memset(game->framebuffer, 0, game->framebuffer_width * game->framebuffer_height * sizeof(Color));
		memset(game->board.tiles, 0, game->board.width * game->board.height / 2);

		blit_screen(game);

		game->board.snake_head = V2(game->board.width / 2, game->board.height / 2);
		game->board.snake_tail = V2(game->board.width / 2 - 1, game->board.height / 2);
		set_tile(game, game->board.snake_head.x, game->board.snake_head.y, DIR_RIGHT);
		set_tile(game, game->board.snake_tail.x, game->board.snake_tail.y, DIR_RIGHT);
		game->board.snake_segments = 2;

		place_fruit(game);

		game->board.ticks_to_step = TICK_INTERVAL;
		game->board.round_initialized = true;
	}

	if (game->input_down != 0) {
		uint8_t new_dir = input_to_dir(game->input_down);
		set_tile(game, game->board.snake_head.x, game->board.snake_head.y, new_dir);
		game->board.ticks_to_step = 1;
	}

	game->board.ticks_to_step -= 1;
	if (game->board.ticks_to_step == 0) {
		bool skip_shrink = false;
		game->board.ticks_to_step = TICK_INTERVAL;
		uint8_t next_dir = get_tile(game, game->board.snake_head.x, game->board.snake_head.y);
		if (next_dir < DIR_LEFT || next_dir > DIR_DOWN) {
			print_error("game", "Invalid next direction. Moving right.");
			next_dir = DIR_RIGHT;
		}

		vec2 next_head = move_and_wrap(&game->board, game->board.snake_head, next_dir);
		uint8_t next_tile = get_tile(game, next_head.x, next_head.y);
		if (next_tile >= TILE_SNAKE_LEFT && next_tile <= TILE_SNAKE_DOWN) {
			print_info("You died.");
			game->board.round_initialized = false;
			return;
		}

		if (next_tile == TILE_FRUIT) {
			game->board.snake_segments += 1;
			skip_shrink = true;
			place_fruit(game);
		}

		v2_set(game->board.snake_head, next_head);
		set_tile(game, game->board.snake_head.x, game->board.snake_head.y, next_dir);

		if (!skip_shrink) {
			vec2 prev_tail;
			v2_set(prev_tail, game->board.snake_tail);
			uint8_t prev_tail_tile = get_tile(game, prev_tail.x, prev_tail.y);

			vec2 next_tail = move_and_wrap(&game->board, prev_tail, prev_tail_tile);
			v2_set(game->board.snake_tail, next_tail);

			set_tile(game, prev_tail.x, prev_tail.y, TILE_NONE);
		}
	}
}
