#include <stdint.h>
#include "SDL.h"
#include "Timer.h"

uint32_t Timer_now(Timer *t) {
	t->t = SDL_GetTicks();
	return t->t;
}

uint32_t Timer_start(Timer *t) {
	uint32_t t0 = Timer_now(t);
	t->t0 = t0;
	return t0;
}

uint32_t Timer_elapsed(Timer *t) {
	return Timer_now(t) - t->t0;
}