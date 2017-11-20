#include "snake.h"
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

/* A global pointer to the game struct for use in the interrupt. */
static Game *_gamep;

static void gamepad_handler(int sig)
{
	gamepad_read(_gamep);
	game_input_changed(_gamep);
}

int gamepad_init(Game * game)
{
	struct sigaction act;
	int old_flags;
	int err;

	game->gamepad_fp = open("/dev/gamepad", O_RDONLY);
	if (game->gamepad_fp < 0) {
		perror("gamepad open");
		return -1;
	}

	_gamep = game;

	memset(&act, 0, sizeof(struct sigaction));

	act.sa_handler = gamepad_handler;
	act.sa_flags = 0;

	err = sigaction(SIGIO, &act, NULL);
	if (err) {
		perror("gamepad signal");
	}

	fcntl(game->gamepad_fp, F_SETOWN, getpid());

	// Get the old flags so we can update instead of override.
	old_flags = fcntl(game->gamepad_fp, F_GETFL);
	fcntl(game->gamepad_fp, F_SETFL, old_flags | FASYNC);

	return 0;
}

void gamepad_close(Game * game)
{
	close(game->gamepad_fp);
}

int gamepad_read(Game * game)
{
	uint8_t prev_state;

	prev_state = game->input;

	if (read(game->gamepad_fp, &game->input, 1) < 0) {
		perror("gamepad read");
		return -1;
	}

	game->input_down = game->input & ~prev_state;
	game->input_up = ~game->input & prev_state;

	return 0;
}
