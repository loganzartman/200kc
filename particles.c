#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "SDL.h"
#include "util.h"
#include "particles.h"
#include "graphics.h"

#define INV255 1.0f/255.0f

float *particles_data;
unsigned char *color_map;
float particles_gravity[] = {0.0f, 1.0f};
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
		color_map[i + CA] = 127;
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
	for (int i=0, j=PART_N; i<j; i++) {
		int pidx = i * PART_NPROPS;
		if (particles_data[pidx + PART_ALIVE]) {
			particle_step(i, timescale);
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

void particle_step(int i, float timescale) {
	int base_idx = i * PART_NPROPS;

	//integrate velocity
	particles_data[base_idx + PART_X] += particles_data[base_idx + PART_VX] * timescale;
	particles_data[base_idx + PART_Y] += particles_data[base_idx + PART_VY] * timescale;

	//integrate acceleration
	particles_data[base_idx + PART_VX] += particles_gravity[0] * timescale;
	particles_data[base_idx + PART_VY] += particles_gravity[1] * timescale;

	//bounds check
	float x = particles_data[base_idx + PART_X];
	float y = particles_data[base_idx + PART_Y];
	if (x<0 || y<0 || x>=gfx_dim.w || y>=gfx_dim.h) {
		particles_data[base_idx + PART_ALIVE] = 0;
		return;
	}

	//draw
	//bounds check
	int pidx = (((int)y)*gfx_dim.w + (int)x)<<2;
	if (pidx < 0 || pidx >= gfx_dim.w*gfx_dim.h*4)
		return;

	//blending
	int mapidx = i << 2;
	int alpha_val = color_map[mapidx + CA];
	float red = blend_buffer[pidx + CR] += (color_map[mapidx + CR] * alpha_val) * INV255;
	float grn = blend_buffer[pidx + CG] += (color_map[mapidx + CG] * alpha_val) * INV255;
	float blu = blend_buffer[pidx + CB] += (color_map[mapidx + CB] * alpha_val) * INV255;
	float alp = 255;

	//write pixels
	pixels[pidx + CR] = (int)MIN(255,red);
	pixels[pidx + CG] = (int)MIN(255,grn);
	pixels[pidx + CB] = (int)MIN(255,blu);
	pixels[pidx + CA] = (int)MIN(255,alp);
}

float particle_speed(int base_idx) {
	return 0;
}
