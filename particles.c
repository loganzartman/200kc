#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "SDL.h"
#include "util.h"
#include "particles.h"
#include "graphics.h"
#include "game.h"

// #define FAST_BLENDING
#define GAMMA 1.4
#define INV255 1.0f/255.0f
#define TRAIL_QUALITY 4

float *particles_data;
unsigned char *color_map;
float particles_gravity[] = {0.0f, 0.65f};
float particles_wind = 0.35f;
int particles_idx;

void particles_init() {
	//todo: handle malloc error
	particles_data = calloc(PART_N * PART_NPROPS, sizeof(float));
	color_map = calloc(PART_N * 4, sizeof(char));
	particles_idx = 0;

	//generate particle colors
	for (int i=0,j=PART_N*4; i<j; i+=4) {
		float frac = ((float)i)/PART_N/4;
		int color = gfx_getHSL(frac);
		color_map[i + CR] = (color >> 16) & 0xFF;
		color_map[i + CG] = (color >> 8) & 0xFF;
		color_map[i + CB] = color & 0xFF;
		color_map[i + CA] = 255;
	}
}

void particles_step(float timescale) {
	//lock particle texture
	void *temp;
	int pitch;
	SDL_LockTexture(
		pixel_buffer,
		NULL,
		&temp,
		&pitch
	);
	pixels = (char*)temp;

	//clear pixel buffer and blending buffer
	for (int i=0, j=gfx_dim.w*gfx_dim.h*4; i<j; i++) {
		pixels[i] = 0;
		blend_buffer[i] = 0;
	}

	//update
	float t = game_timer.t;
	for (int i=0, j=PART_N; i<j; i++) {
		int pidx = i * PART_NPROPS;
		if (particles_data[pidx + PART_ALIVE]) {
			particle_step(i, timescale, t);
		}
	}

	//update particle texture
	SDL_UnlockTexture(pixel_buffer);
}

void particles_destroy() {
	free(particles_data);
}

void particle_new(float x, float y, float vx, float vy) {
	int idx = particles_idx*PART_NPROPS;
	particles_data[idx + PART_ALIVE] = 1;
	particles_data[idx + PART_X] = x;
	particles_data[idx + PART_Y] = y;
	particles_data[idx + PART_VX] = vx;
	particles_data[idx + PART_VY] = vy;
	particles_idx = (particles_idx+1)%PART_N;
}

void particle_step(int i, float timescale, float t) {
	int base_idx = i * PART_NPROPS;
	int w = gfx_dim.w, h = gfx_dim.h;

	//integrate velocity
	float x = particles_data[base_idx + PART_X] += particles_data[base_idx + PART_VX] * timescale;
	float y = particles_data[base_idx + PART_Y] += particles_data[base_idx + PART_VY] * timescale;

	//wind
	float wx = qsin(y*0.012 + t*0.00124) * 2.5 * particles_wind;
	float wy = qsin(x*0.02 + t*0.00124) * particles_wind;

	//integrate acceleration
	particles_data[base_idx + PART_VX] += (particles_gravity[0] + wx) * timescale;
	particles_data[base_idx + PART_VY] += (particles_gravity[1] + wy) * timescale;

	//bounds check
	if (x<0 || y<0 || x>=w || y>=h) {
		particles_data[base_idx + PART_ALIVE] = 0;
		return;
	}

	//draw
	float xx = x, yy = y;
	float dx = particles_data[base_idx + PART_VX];
	float dy = particles_data[base_idx + PART_VY];
	float len = MAX(1, sqrt(dx * dx + dy * dy));
	dx /= len;
	dy /= len;

	//precalculation for blending
	int mapidx = i << 2;
	int alpha_val = color_map[mapidx + CA];
	float density = GAMMA / (len / TRAIL_QUALITY);
	float alpha_adj = alpha_val * density;
	float red = color_map[mapidx + CR] * alpha_adj * INV255;
	float grn = color_map[mapidx + CG] * alpha_adj * INV255;
	float blu = color_map[mapidx + CB] * alpha_adj * INV255;
	float alp = 255;

	dx *= TRAIL_QUALITY;
	dy *= TRAIL_QUALITY;
	for (int i=0, j=MAX(1,len/TRAIL_QUALITY); i<j && xx>=0 && yy>=0 && xx<w && yy<h; i++) {
		int pidx = (((int)yy)*w + (int)xx)<<2;

		#ifndef FAST_BLENDING
		//blending
		float red1 = blend_buffer[pidx + CR] += red;
		float grn1 = blend_buffer[pidx + CG] += grn;
		float blu1 = blend_buffer[pidx + CB] += blu;

		//write pixels
		pixels[pidx + CR] = (int)MIN(255, red1);
		pixels[pidx + CG] = (int)MIN(255, grn1);
		pixels[pidx + CB] = (int)MIN(255, blu1);
		pixels[pidx + CA] = alp;
		#endif

		#ifdef FAST_BLENDING
		pixels[pidx + CR] = (int)(pixels[pidx + CR] + red);
		pixels[pidx + CG] = (int)(pixels[pidx + CG] + grn);
		pixels[pidx + CB] = (int)(pixels[pidx + CB] + blu);
		pixels[pidx + CA] = alp;
		#endif

		//move
		xx += dx;
		yy += dy;
	}
}

float particle_speed(int base_idx) {
	return 0;
}
