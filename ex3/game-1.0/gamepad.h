#ifndef GAME_GAMEPAD_H
#define GAME_GAMEPAD_H

#include "snake.h"

int gamepad_init(Game * game);
void gamepad_close(Game * game);
int gamepad_read(Game * game);

#endif
