#include <stdint.h>
#include "SDL.h"
#include "Timer.h"

uint32_t Timer_now() {
	return SDL_GetTicks();
}

uint32_t Timer_start(Timer *t) {
	uint32_t t0 = Timer_now();
	t->t0 = t0;
	return t0;
}

uint32_t Timer_elapsed(Timer *t) {
	return Timer_now() - t->t0;
}