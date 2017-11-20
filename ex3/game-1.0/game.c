#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "snake.h"
#include "graphics.h"
#include "gamepad.h"
#include "utils.h"

static const struct timespec frame_duration = { 0, NSEC / FRAME_RATE };

static bool should_run = true;

static struct timespec read_time()
{
	struct timespec time;
	int error;

	error = clock_gettime(CLOCK_MONOTONIC, &time);
	assert(!error);

	return time;
}

static struct timespec timespec_diff(struct timespec begin, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec - begin.tv_nsec) < 0) {
		temp.tv_sec = end.tv_sec - begin.tv_sec - 1;
		temp.tv_nsec = end.tv_nsec - begin.tv_nsec + 1000000000;
	} else {
		temp.tv_sec = end.tv_sec - begin.tv_sec;
		temp.tv_nsec = end.tv_nsec - begin.tv_nsec;
	}
	return temp;
}

int main(int argc, char *argv[])
{
	int err;
	Game game;

	memset(&game, 0, sizeof(Game));

	err = framebuffer_init(&game);
	if (err) {
		return err;
	}

	game_init(&game);

	err = gamepad_init(&game);
	if (err) {
		return err;
	}

	struct timespec tick_begin;
	tick_begin = read_time();

	while (should_run) {
		game_update_and_render(&game);

		struct timespec tick_end = read_time();
		struct timespec tick_duration =
		    timespec_diff(tick_begin, tick_end);
		struct timespec tick_remaining =
		    timespec_diff(tick_duration, frame_duration);

		while (nanosleep(&tick_remaining, &tick_remaining) == -1
				&& errno == EINTR) ;

		tick_begin = read_time();
	}

	framebuffer_close(&game);
	gamepad_close(&game);

	exit(EXIT_SUCCESS);
}

/*
	Note: I currently was unable to compile this as multiple units, so I just
	include everything here. This still works as intended, and should have
	worked as multiple separate units.
 */
#include "snake.c"
#include "utils.c"
#include "graphics.c"
#include "gamepad.c"
