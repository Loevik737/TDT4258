#ifndef SNAKE_H
#define SNAKE_H

#include <stdint.h>
#include <stdbool.h>

#include "utils.h"

#define FRAME_RATE 5
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240


typedef uint16_t Color;

#define RMAX 0xff
#define GMAX 0xff
#define BMAX 0xff

#define COLOR(r, g, b) ((((uint16_t)r & 0x1f) << 11) | ((uint16_t)(g & 0x3f) << 5) | ((uint16_t)b & 0x1f))
#define COLOR_SET(dest, c) dest = c;

#define RED   COLOR(RMAX,    0,    0)
#define GREEN COLOR(   0, GMAX,    0)
#define BLUE  COLOR(   0,    0, BMAX)

typedef struct {uint8_t x,y;} vec2;

#define Vx(v) (v.x)
#define Vy(v) (v.y)
#define V2(x, y) (vec2){x, y}

#define v2_add(lhs, rhs) (vec2){lhs.x + rhs.x, lhs.y + rhs.y}
#define v2_sub(lhs, rhs) (vec2){lhs.x - rhs.x, lhs.y - rhs.y}

#define v2_set(dest, val) dest.x=val.x;dest.y=val.y;

#define TILE_NONE 0x0

#define TILE_SNAKE_LEFT  0x1
#define TILE_SNAKE_RIGHT 0x2
#define TILE_SNAKE_UP    0x3
#define TILE_SNAKE_DOWN  0x4

#define TILE_FRUIT       0xf

#define SNAKE_MAX_LENGTH 100

#define DIR_LEFT TILE_SNAKE_LEFT
#define DIR_RIGHT TILE_SNAKE_RIGHT
#define DIR_UP TILE_SNAKE_UP
#define DIR_DOWN TILE_SNAKE_DOWN

#define TICK_INTERVAL 1


typedef struct {
	uint8_t width, height;
	uint8_t ticks_to_step;

	uint8_t *tiles;
	vec2 snake_head;
	vec2 snake_tail;
	uint16_t snake_segments;

	bool round_initialized;
} GameBoard;

typedef struct {
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;
	Color *framebuffer;
	int framebuffer_fp;
	uint64_t tick;
	uint8_t input;
	uint8_t input_down;
	uint8_t input_up;

	GameBoard board;
	bool initialized;
} Game;

void game_update_and_render(Game * game);
void blit_region(Game * game, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void blit_screen(Game * game);

#endif
