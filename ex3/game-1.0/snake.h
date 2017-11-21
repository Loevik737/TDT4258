#ifndef SNAKE_H
#define SNAKE_H

#include <stdint.h>
#include <stdbool.h>

#include "utils.h"

#define FRAME_RATE 6
#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

typedef uint16_t Color;

#define RMAX 0x1f
#define GMAX 0x3f
#define BMAX 0x1f

#define COLOR(r, g, b) ((((uint16_t)r & 0x1f) << 11) | ((uint16_t)(g & 0x3f) << 5) | ((uint16_t)b & 0x1f))
#define COLOR_SET(dest, c) dest = c;

#define RED   COLOR(RMAX,    0,    0)
#define GREEN COLOR(   0, GMAX,    0)
#define BLUE  COLOR(   0,    0, BMAX)
#define WHITE COLOR(RMAX, GMAX, BMAX)
#define BLACK COLOR(   0,    0,    0)

typedef struct {
	uint8_t x, y;
} vec2;

#define V2(x, y) (vec2){x, y}
#define v2_add(lhs, rhs) (vec2){lhs.x + rhs.x, lhs.y + rhs.y}
#define v2_sub(lhs, rhs) (vec2){lhs.x - rhs.x, lhs.y - rhs.y}
#define v2_set(dest, val) dest.x=val.x;dest.y=val.y;

/*
  A tile is represeted by a 4 bit integer.
  
  A snake is represented:
  |    3    | 2       0 |
  | snakenr | direction |
  
  Snakenr is 0 for snake 1, and 1 for snake 2.
  Direction is
  0x1: left
  0x2: right
  0x3: up
  0x4: down

  Tile types:
  0x0     Empty tile
  0x1-0x4 Snake 1
  0x5-0x8 (unused)
  0x9-0xc Snake 2
  0xd-0xe (unused)
  0xf     Fruit
 */
#define TILE_NONE 0x0

#define TILE_SNAKE_LEFT  0x1
#define TILE_SNAKE_RIGHT 0x2
#define TILE_SNAKE_UP    0x3
#define TILE_SNAKE_DOWN  0x4

#define TILE_SNAKE_1  0x0
#define TILE_SNAKE_2  0x8

#define TILE_FRUIT    0xf

#define SNAKE_MAX_LENGTH 100

#define DIR_LEFT TILE_SNAKE_LEFT
#define DIR_RIGHT TILE_SNAKE_RIGHT
#define DIR_UP TILE_SNAKE_UP
#define DIR_DOWN TILE_SNAKE_DOWN

/* Returns the direction of a snake tile */
#define SNAKE_DIR(tile) (tile & 0x7)
/* Returns the snake id of a snake tile */
#define SNAKE_NR(tile) ((tile & 0x8) >> 3)

#define SNAKE_FLAG(snake) (snake << 3)

/*
  I prefer this kind of string, where the length is known, to cstrings,
  which is 0-terminated.
*/
typedef struct {
	uint8_t *text;
	uint16_t length;
} String;

/* Macro to create a string struct form a cstring. */
#define STR(s) (String){(uint8_t*)s, (uint16_t)sizeof(s) - 1}

typedef struct {
	vec2 head;
	vec2 tail;
	uint16_t segments;
	Color color;
	uint8_t score;
} Snake;

#define NUM_SNAKES 2

typedef struct {
	uint8_t width, height;
	uint8_t ticks_to_step;

	/*
	   The game board tiles is represented as a 2D array of 4 bit
	   tiles.

	   A snake is represented on the game board as tiles, where each
	   snake-tile points towards the next snake tile. This makes a "chain"
	   from the tail of the snake to the head. The head and tail
	   coordinates are stored on the Snake structures.
	 */
	uint8_t *tiles;
	Snake snakes[NUM_SNAKES];

	bool round_initialized;
} GameBoard;

typedef struct {
	/*
	   Framebuffer data
	 */
	uint32_t framebuffer_width;
	uint32_t framebuffer_height;
	Color *framebuffer;
	int framebuffer_fp;
	bool disable_blit;

	/*
	   Gamepad data
	 */
	int gamepad_fp;
	uint8_t input;
	uint8_t input_down;
	uint8_t input_up;

	GameBoard board;
} Game;

void game_update_and_render(Game * game);
void game_init(Game * game);
void game_input_changed(Game * game);

#endif
