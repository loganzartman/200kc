#include <stdbool.h>
#include "SDL.h"
#include "vector.h"

#ifndef PARTICLE_GUARD
#define PARTICLE_GUARD
#define PART_N 150000
#define PART_NPROPS 5
#define PART_ALIVE 0
#define PART_X 1
#define PART_Y 2
#define PART_VX 3
#define PART_VY 4

/**
 * Initializes the particle system
 */
void particles_init();

/**
 * Advance the particle simulation.
 * timescale is a float representing the ratio of actual frame time to desired frame time.
 */
void particles_step(float timescale);

void particles_destroy();

/**
 * Adds a new particle to the particle system
 */
void particle_new(float x, float y, float vx, float vy);

/**
 * Advance the simulation of a single particle.
 * Called by particles_step
 */
void particle_step(int base_idx, float timescale, float t);

#endif
