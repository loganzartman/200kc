#include <stdio.h>
#include <stdlib.h>
#include "SDL.h"
#include "graphics.h"
#include "particles.h"
#include "util.h"

SDL_Window *gfx_win;
SDL_Renderer *gfx_rnd;

int gfx_init() {
	//initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("SDL_Init Error: %s\n",SDL_GetError());
		return 1;
	}
	
	//create window
	gfx_dim.w = 800;
	gfx_dim.h = 600;
	gfx_win = SDL_CreateWindow(
		"Hello SDL!",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		gfx_dim.w, gfx_dim.h,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
	);
	if (gfx_win == NULL) {
		printf("SDL_CreateWindow error: %s\n", SDL_GetError());
		return 1;
	}

	//create renderer
	gfx_rnd = SDL_CreateRenderer(
		gfx_win,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (gfx_rnd == NULL) {
		printf("SDL_CreateRenderer error: %s\n", SDL_GetError());
		return 1;
	}

	gfx_resize(gfx_dim.w, gfx_dim.h);

	printf("SDL_Init success!\n");
	return 0;
}

void gfx_resize(int width, int height) {
	//store dimensions
	gfx_dim.w = width;
	gfx_dim.h = height;

	//create pixel buffer
    if (pixel_buffer != NULL)
    	free(pixel_buffer);
    pixel_buffer = SDL_CreateTexture(
		gfx_rnd,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		width, height
	);

	//create blending buffer
	blend_buffer = calloc(gfx_dim.w * gfx_dim.h * 4, sizeof(float));

	printf("resized to %dx%d\n", width, height);
}

void gfx_draw() {
	SDL_SetRenderDrawColor(gfx_rnd, 43, 38, 40, 255);
	SDL_RenderFillRect(gfx_rnd, &gfx_dim);

	SDL_RenderCopy(gfx_rnd, pixel_buffer, NULL, NULL );

	SDL_RenderPresent(gfx_rnd);
}

void gfx_destroy() {
	free(blend_buffer);
	SDL_DestroyRenderer(gfx_rnd);
	SDL_DestroyWindow(gfx_win);
	SDL_Quit();
}

int gfx_getHSL(float hue) {
	const float THIRD = 1.0f/3.0f;
	hue -= (int)hue;
	float r,g,b,hf;
	if (hue < THIRD) {
		hf = 2*hue/THIRD;
		r = MIN(1, 2-hf);
		g = MIN(1, hf);
		b = 0;
	}
	else if (hue < THIRD*2) {
		hf = 2*(hue-THIRD)/THIRD;
		r = 0;
		g = MIN(1, 2-hf);
		b = MIN(1, hf);
	}
	else {
		hf = 2*(hue-THIRD*2)/THIRD;
		r = MIN(1, hf);
		g = 0;
		b = MIN(1, 2-hf);
	}
	return ((int)(r*255)<<16) | ((int)(g*255)<<8) | (int)(b*255);
}