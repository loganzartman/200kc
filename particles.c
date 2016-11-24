#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "SDL.h"
#include "util.h"
#include "particles.h"
#include "graphics.h"
#include "game.h"

#define GAMMA 1
#define INV255 0.00392156862f
#define TRAIL_QUALITY 4

float *particles_data;
uint8_t *color_map;
float particles_gravity[] = {0.0f, 0.65f};
float particles_wind = 0.35f;
int particles_idx;

void particles_init() {
	//todo: handle malloc error
	particles_data = calloc(PART_N * PART_NPROPS, sizeof(float));
	color_map = calloc(PART_N * 4, sizeof(uint8_t));
	particles_idx = 0;

	//generate particle colors
	for (int i=0,j=PART_N*4; i<j; i+=4) {
		float frac = ((float)i)/PART_N/4;
		int color = gfx_getHSL(frac);

		//unpack and store
		color_map[i + CR] = (color >> 16) & 0xFF;
		color_map[i + CG] = (color >> 8) & 0xFF;
		color_map[i + CB] = color & 0xFF;
		color_map[i + CA] = 255;
	}
}

void particles_step(float timescale) {
	//lock particle texture to allow modification
	void *temp;
	int pitch;
	SDL_LockTexture(
		pixel_buffer,
		NULL,
		&temp,
		&pitch
	);
	pixels = (uint8_t*)temp;

	//clear pixel buffer and blending buffer
	for (int i=0, j=gfx_dim.w*gfx_dim.h*4; i<j; i++) {
		pixels[i] /= 2;
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

	//determine movement for rendering motion blur
	float xx = x, yy = y;
	float dx = particles_data[base_idx + PART_VX];
	float dy = particles_data[base_idx + PART_VY];
	float len = MAX(1, sqrt(dx * dx + dy * dy));
	dx /= len;
	dy /= len;

	//precalculation for rendering
	int mapidx = i << 2; //colormap index
	int alpha_val = color_map[mapidx + CA];

	//decrease intensity when more pixels are drawn per particle
	float density = GAMMA / (len / TRAIL_QUALITY); 
	float alpha_adj = alpha_val * density * INV255;
	
	//calculate color of this particle
	uint8_t red = (uint8_t)(MIN(255, color_map[mapidx + CR] * alpha_adj));
	uint8_t grn = (uint8_t)(MIN(255, color_map[mapidx + CG] * alpha_adj));
	uint8_t blu = (uint8_t)(MIN(255, color_map[mapidx + CB] * alpha_adj));
	uint8_t alp = 255;

	//increase motion blur step size for lower quality
	dx *= TRAIL_QUALITY;
	dy *= TRAIL_QUALITY;

	//render particle with motion blur
	for (int i=0, j=MAX(1,len/TRAIL_QUALITY); i<j && xx>=0 && yy>=0 && xx<w && yy<h; i++) {
		//calculate index in pixel buffer
		int pidx = (((int)yy)*w + (int)xx)<<2;

		//add buffered color and new pixel color
		uint16_t ar = pixels[pidx + CR] + red;
		uint16_t ag = pixels[pidx + CG] + grn;
		uint16_t ab = pixels[pidx + CB] + blu;

		//write blended color to buffer
		//uses branchless method to clip values greater than 255
		pixels[pidx + CR] = (uint8_t)(((!!(ar >> 8))*255) | (uint8_t)ar);
		pixels[pidx + CG] = (uint8_t)(((!!(ag >> 8))*255) | (uint8_t)ag);
		pixels[pidx + CB] = (uint8_t)(((!!(ab >> 8))*255) | (uint8_t)ab);
		pixels[pidx + CA] = alp;

		//move
		xx += dx;
		yy += dy;
	}
}
