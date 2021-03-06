#include <stdbool.h>
#include "Timer.h"

#ifndef GAME_GUARD
#define GAME_GUARD

typedef struct mouse_obj {
	int x;
	int y;
	int px;
	int py;
	bool down;
} mouse_obj;

mouse_obj mouse;
Timer game_timer;

int main(int argc, char *argv[]);
void game_loop(float timescale);

#endif