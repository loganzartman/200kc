#include <stdint.h>

#ifndef TIMER_GUARD
#define TIMER_GUARD

typedef struct Timer {
	unsigned int t0;
	unsigned int t;
} Timer;

#endif

/**
 * Returns a current timestamp.
 * Not necessarily with respect to any meaningful epoch.
 */
uint32_t Timer_now();

/**
 * Sets the start time for the timer.
 * Returns the current timestamp.
 */
uint32_t Timer_start(Timer *t);

/**
 * Determines the time elapsed since the timer was started.
 */
uint32_t Timer_elapsed(Timer *t);