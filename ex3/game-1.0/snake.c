#include "snake.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "graphics.h"

void set_tile(Game * game, int x, int y, uint8_t tile)
{
	uint8_t mask, val;
	int i;
	Color color;

	assert(x < game->board.width);
	assert(y < game->board.height);

	assert(tile <= 0xf);
	mask = 0xf;
	val = tile;
	i = (y * game->board.width + x);

	if (i % 2 != 0) {
		mask <<= 4;
		val <<= 4;
	}

	game->board.tiles[i / 2] = (game->board.tiles[i / 2] & ~mask) | val;

	COLOR_SET(color, COLOR(0, 0, 0));

	if (tile == TILE_FRUIT) {
		COLOR_SET(color, GREEN);
	} else if (SNAKE_DIR(tile) >= TILE_SNAKE_LEFT
		   && SNAKE_DIR(tile) <= TILE_SNAKE_DOWN) {
		COLOR_SET(color, game->board.snakes[SNAKE_NR(tile)].color);
	}

	draw_rectangle(game, color, x * 10 + 1, 20 + y * 10 + 1, 8, 8);
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
		if (pos.x == 0) {
			return V2(board->width - 1, pos.y);
		} else {
			return v2_sub(pos, V2(1, 0));
		}
	} else if (dir == DIR_RIGHT) {
		if (pos.x >= board->width - 1) {
			return V2(0, pos.y);
		} else {
			return v2_add(pos, V2(1, 0));
		}
	} else if (dir == DIR_UP) {
		if (pos.y == 0) {
			return V2(pos.x, board->height - 1);
		} else {
			return v2_sub(pos, V2(0, 1));
		}
	} else if (dir == DIR_DOWN) {
		if (pos.y >= board->height - 1) {
			return V2(pos.x, 0);
		} else {
			return v2_add(pos, V2(0, 1));
		}
	}
	return V2(0, 0);
}

uint8_t input_to_dir(uint8_t input)
{
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
	/*
	   This function places a fruit on a random, available tile one the
	   game board. It should be fairly uniform.
	 */

	uint16_t available_tiles, covered_tiles;
	int r, j;
	uint16_t selected;
	uint16_t x, y, i;

	r = rand();

	/*
	   Determine how many available spaces the board has, by first
	   figuring out how many are taken.
	 */
	covered_tiles = 0;
	for (j = 0; j < NUM_SNAKES; ++j) {
		covered_tiles += game->board.snakes[j].segments;
	}
	available_tiles =
	    game->board.width * game->board.height - covered_tiles;
	selected = r % available_tiles;

	/*
	   Iterate until we meet the tile we selected.
	 */
	i = 0;
	for (y = 0; y < game->board.height; ++y) {
		for (x = 0; x < game->board.width; ++x) {
			uint8_t tile = get_tile(game, x, y);
			if (SNAKE_DIR(tile) < TILE_SNAKE_LEFT
			    || SNAKE_DIR(tile) > TILE_SNAKE_DOWN) {
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

uint8_t inverse_dir(uint8_t dir)
{
	switch (dir) {
	case DIR_UP:
		return DIR_DOWN;
	case DIR_DOWN:
		return DIR_UP;
	case DIR_LEFT:
		return DIR_RIGHT;
	case DIR_RIGHT:
		return DIR_LEFT;
	}
	return 0;
}

void game_update_snake_input(Game * game, int snake, uint8_t input_down)
{
	if (input_down != 0) {
		uint8_t new_dir = input_to_dir(input_down);

		vec2 next_head =
		    move_and_wrap(&game->board, game->board.snakes[snake].head,
				  new_dir);
		uint8_t next_tile = get_tile(game, next_head.x, next_head.y);
		uint8_t inverse_new_dir = 0;

		inverse_new_dir = inverse_dir(new_dir);

		if (inverse_new_dir == SNAKE_DIR(next_tile)) {
			/*
			   This move would cause the snake to collide with its own
			   neck. Abort. 
			 */
			return;
		}

		set_tile(game, game->board.snakes[snake].head.x,
			 game->board.snakes[snake].head.y,
			 new_dir | SNAKE_FLAG(snake));
	}

}

#define GAME_OVER_TIE 2
#define GAME_OVER_PLAYER_1_WIN 0
#define GAME_OVER_PLAYER_2_WIN 1

static String winner_player1 = STR("Player 1 Won");
static String winner_player2 = STR("Player 2 Won");
static String winner_tie = STR("Tie");

static const struct timespec game_over_duration = { 1, NSEC / 2 };

void game_over(Game * game, uint8_t winner)
{
	Color color_bg, color_fg;
	String *text;

	if (winner < GAME_OVER_TIE) {
		game->board.snakes[winner].score += 1;
	}

	game->disable_blit = true;

	if (winner == 0) {
		color_bg = BLUE;
		color_fg = WHITE;
		text = &winner_player1;
	} else if (winner == 1) {
		color_bg = RED;
		color_fg = WHITE;
		text = &winner_player2;
	} else {
		color_bg = WHITE;
		color_fg = BLACK;
		text = &winner_tie;
	}

	draw_rectangle(game, color_bg, 0, 0,
		       game->framebuffer_width - 1,
		       game->framebuffer_height - 1);

	draw_text_centered(game, *text, color_fg, color_bg,
			   game->framebuffer_width / 2,
			   game->framebuffer_height / 2 - 4 * 6, 4);

	game->disable_blit = false;
	blit_screen(game);
	nsleep(NSEC / 2);
	//nanosleep(&game_over_duration, NULL);

	game->board.round_initialized = false;
	/*
	   Disable the blit to allow the first frame of the new game to be drawn.
	   game_update_and_render will disable this.
	 */
	game->disable_blit = false;
}

void game_update_snake(Game * game, int snake)
{
	bool skip_shrink = false;
	uint8_t next_dir =
	    SNAKE_DIR(get_tile(game, game->board.snakes[snake].head.x,
			       game->board.snakes[snake].head.y));
	if (next_dir < DIR_LEFT || next_dir > DIR_DOWN) {
		print_error("game", "Invalid next direction. Moving right.");
		next_dir = DIR_RIGHT;
	}

	vec2 next_head =
	    move_and_wrap(&game->board, game->board.snakes[snake].head,
			  next_dir);
	uint8_t next_tile = get_tile(game, next_head.x, next_head.y);
	if (SNAKE_DIR(next_tile) >= TILE_SNAKE_LEFT
	    && SNAKE_DIR(next_tile) <= TILE_SNAKE_DOWN) {

		if (inverse_dir(SNAKE_DIR(next_tile)) == SNAKE_DIR(next_dir)) {
			print_info("Tie");
			game_over(game, GAME_OVER_TIE);
		} else {
			print_info("Player %i died.", snake + 1);
			/* The other snake won */
			game_over(game, (snake + 1) % 2);
		}

		return;
	}

	if (next_tile == TILE_FRUIT) {
		game->board.snakes[snake].segments += 1;
		skip_shrink = true;
		place_fruit(game);
	}

	v2_set(game->board.snakes[snake].head, next_head);
	set_tile(game, game->board.snakes[snake].head.x,
		 game->board.snakes[snake].head.y,
		 next_dir | SNAKE_FLAG(snake));

	if (!skip_shrink) {
		vec2 prev_tail;
		v2_set(prev_tail, game->board.snakes[snake].tail);
		uint8_t prev_tail_tile =
		    get_tile(game, prev_tail.x, prev_tail.y);

		vec2 next_tail = move_and_wrap(&game->board, prev_tail,
					       SNAKE_DIR(prev_tail_tile));
		v2_set(game->board.snakes[snake].tail, next_tail);

		set_tile(game, prev_tail.x, prev_tail.y, TILE_NONE);
	}
}

String uint8_to_str(String buffer, uint8_t n)
{
	String result;
	uint8_t rest;

	assert(buffer.length == 3);

	rest = n;
	result.text = buffer.text + 3;
	result.length = 0;

	do {
		result.length += 1;
		result.text -= 1;
		*result.text = '0' + (rest % 10);
		rest /= 10;
	} while (result.length < 3);

	return result;
}

void draw_player_scores(Game * game)
{
	uint8_t buffer[3];
	String score_buffer;
	String score;

	score_buffer.text = buffer;
	score_buffer.length = 3;

	draw_text(game, STR("Player 1"), BLUE, BLACK, 0, 0, 3);
	draw_text_right(game, STR("Player 2"), RED, BLACK,
			game->framebuffer_width, 0, 3);

	score = uint8_to_str(score_buffer, game->board.snakes[0].score);
	draw_text(game, score, BLUE, BLACK, 3 * 4 * 10, 0, 3);

	score = uint8_to_str(score_buffer, game->board.snakes[1].score);
	draw_text_right(game, score, RED, BLACK,
			game->framebuffer_width - (3 * 4 * 10), 0, 3);
}

void game_reset(Game * game)
{
	game->board.snakes[0].color = BLUE;
	game->board.snakes[1].color = RED;

	/* Clear framebuffer */
	memset(game->framebuffer, 0,
	       game->framebuffer_width * game->framebuffer_height *
	       sizeof(Color));
	blit_screen(game);

	/* Clear gamefield */
	memset(game->board.tiles, 0,
	       game->board.width * game->board.height / 2);

	/* Reset snake 0 */
	game->board.snakes[0].head =
	    V2(game->board.width / 4, game->board.height / 2);
	game->board.snakes[0].tail =
	    V2(game->board.width / 4 - 1, game->board.height / 2);
	set_tile(game, game->board.snakes[0].head.x,
		 game->board.snakes[0].head.y,
		 TILE_SNAKE_RIGHT | SNAKE_FLAG(0));
	set_tile(game, game->board.snakes[0].tail.x,
		 game->board.snakes[0].tail.y,
		 TILE_SNAKE_RIGHT | SNAKE_FLAG(0));
	game->board.snakes[0].segments = 2;

	/* Reset snake 1 */
	game->board.snakes[1].head =
	    V2(3 * game->board.width / 4, game->board.height / 2);
	game->board.snakes[1].tail =
	    V2(3 * game->board.width / 4 + 1, game->board.height / 2);
	set_tile(game, game->board.snakes[1].head.x,
		 game->board.snakes[1].head.y, TILE_SNAKE_LEFT | SNAKE_FLAG(1));
	set_tile(game, game->board.snakes[1].tail.x,
		 game->board.snakes[1].tail.y, TILE_SNAKE_LEFT | SNAKE_FLAG(1));
	game->board.snakes[1].segments = 2;

	place_fruit(game);

	/* Only draw player scores here. That area of the screen is never
	 * obstructued or update during normal play.
	 */
	draw_player_scores(game);

	game->board.round_initialized = true;
}

void game_init(Game * game)
{
	game->board.width = DISPLAY_WIDTH / 10;
	/* Subtract 2 to compansate for score text */
	game->board.height = DISPLAY_HEIGHT / 10 - 2;
	/* Divide by 2 because each tile takes 4 bit */
	game->board.tiles =
	    calloc(game->board.width * game->board.height / 2, 1);
}

void game_input_changed(Game * game)
{
	if (game->input_down & 0xf) {
		game_update_snake_input(game, 0, game->input_down & 0xf);
	} else if (game->input_down & 0xf0) {
		game_update_snake_input(game, 1, game->input_down >> 4);
	}
}

void game_update_and_render(Game * game)
{
	/* Round start initialization */
	if (!game->board.round_initialized) {
		game_reset(game);
	}

	game_update_snake(game, 0);
	game_update_snake(game, 1);

	if (game->board.round_initialized && game->disable_blit) {
		game->disable_blit = false;
		blit_screen(game);
	}
}
