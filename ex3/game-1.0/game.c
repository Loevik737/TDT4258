#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>

#define FB_WIDTH 320
#define FB_HEIGHT 240

//typedef uint16_t Color;

#define FB_SIZE (FB_WIDTH*FB_HEIGHT*sizeof(uint16_t))

#define COLOR(r, g, b) ((((uint16_t)r & 0x1f) << 11) | ((uint16_t)(g & 0x3f) << 5) | ((uint16_t)b & 0x1f))

#define RMAX (0x1f)
#define GMAX (0x3f)
#define BMAX (0x1f)

#define RED   (COLOR(RMAX,    0,    0))
#define GREEN (COLOR(   0, GMAX,    0))
#define BLUE  (COLOR(   0,    0, BMAX))
#define WHITE (COLOR(RMAX, GMAX, BMAX))

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

	int x, y;

	/* for (y = 0; y < FB_HEIGHT; ++y) { */
	/* 	for (x = 0; x < FB_WIDTH; ++x) { */
	/* 		fb[x+y*FB_WIDTH] = RED; */
	/* 	} */
	/* } */

	for (y = 0; y < FB_HEIGHT; ++y) {
		for (x = 0; x < FB_WIDTH; ++x) {
			int i = (x + y * FB_WIDTH);
			fb[i] = COLOR(x * RMAX / FB_WIDTH,
						  0,
						  y * BMAX / FB_HEIGHT);
		}
	}

	struct fb_copyarea rect;

	rect.dx = 0;
	rect.dy = 0;
	rect.width = FB_WIDTH;
	rect.height = FB_HEIGHT;
	//rect.sx = 0;
	//rect.sy = 0;

	ioctl(fp_fb, 0x4680, &rect);

	for (;;) {
		unsigned char button_state, button_down, button_up;

		if (read(fp_gamepad, &button_state, 1) < 0) {
			perror("gamepad read");
			exit(-1);
		}

		button_down =  button_state & ~prev_button_state;
		button_up   = ~button_state &  prev_button_state;

		if (button_down != 0) {
			printf("button down: %02x\n", button_down);
		}

		if (button_up != 0) {
			printf("button up:   %02x\n", button_up);
		}

		prev_button_state = button_state;
	}
	

	// munmap(fb, FB_SIZE);
	// close(fp_fb);

	exit(EXIT_SUCCESS);
}
