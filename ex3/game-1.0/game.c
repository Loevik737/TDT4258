#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>

/* enable clock_* functions */
/* #define __USE_POSIX199309 */
/* #define __USE_XOPEN2K */
#include <time.h>

#include "snake.h"
#include "utils.h"

#define FB_SIZE (DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(uint16_t))

#define NSEC (1000000000)

static const struct timespec frame_duration = { 0, NSEC / FRAME_RATE };

static struct timespec read_time()
{
	struct timespec time;
	int error;

	error = clock_gettime(CLOCK_MONOTONIC, &time);

	/*
	   NOTE: We should already have checked that we support
	   CLOCK_MONOTONIC_RAW, so we should not receive EINVAL error. It
	   can return EFAULT if the second param is outside the memory we
	   can access. This would be a programming error.
	 */
	assert(!error);

	return time;
}

static struct timespec timespec_add(struct timespec begin, struct timespec end)
{
	struct timespec temp;
	temp.tv_sec = begin.tv_sec + end.tv_sec;
	temp.tv_nsec = begin.tv_nsec + end.tv_nsec;
	temp.tv_sec += begin.tv_nsec / NSEC;
	temp.tv_nsec %= NSEC;
	return temp;
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
	int fp_fb, fp_gamepad;
	unsigned char prev_button_state = 0;

	printf("Hello World, I'm game!\n");

	fp_fb = open("/dev/fb0", O_RDWR);
	if (fp_fb < 0) {
		perror("fb open");
		exit(-1);
	}

	fp_gamepad = open("/dev/gamepad", O_RDONLY);
	if (fp_gamepad < 0) {
		perror("gamepad open");
		exit(-1);
	}


	uint16_t *fb = mmap(NULL, FB_SIZE, PROT_WRITE, MAP_SHARED, fp_fb, 0);

	if (fb == MAP_FAILED) {
		perror("mmap");
		exit(-1);
	}

	Game game = {};

	game.framebuffer = fb;
	game.framebuffer_fp = fp_fb;
	game.framebuffer_width = DISPLAY_WIDTH;
	game.framebuffer_height = DISPLAY_HEIGHT;
	game.board.width = DISPLAY_WIDTH / 10;
	game.board.height = DISPLAY_HEIGHT / 10;
	game.board.tiles = calloc(game.board.width * game.board.height / 2, 1);

	memset(fb, 0, FB_SIZE);
	blit_screen(&game);


	struct timespec tick_begin;
	tick_begin = read_time();

	for (;;) {
		if (read(fp_gamepad, &game.input, 1) < 0) {
			perror("gamepad read");
			exit(-1);
		}

		game.input_down =  game.input & ~prev_button_state;
		game.input_up   = ~game.input &  prev_button_state;

		prev_button_state = game.input;

		game_update_and_render(&game);

		struct timespec tick_end = read_time();
		struct timespec tick_duration = timespec_diff(tick_begin, tick_end);
		struct timespec tick_remaining = timespec_diff(tick_duration, frame_duration);

		nanosleep(&tick_remaining, NULL);

		tick_begin = read_time();
	}
	

	// munmap(fb, FB_SIZE);
	// close(fp_fb);

	exit(EXIT_SUCCESS);
}

#include "snake.c"
#include "utils.c"
