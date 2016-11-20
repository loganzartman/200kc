#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SDL.h"
#include "vector.h"
#include "Timer.h"
#include "util.h"
#include "game.h"
#include "graphics.h"
#include "particles.h"

const int GAME_FPS = 60;
bool game_running = true;
SDL_Event event;
Timer game_timer;
float px, py;

int main(int argc, char *argv[]) {
	//try initializing SDL2
	if (gfx_init() != 0) {
		return 1;
	}

	//initialize particle system
	particles_init();
	printf("particles initialized\n");

	//start game timer
	Timer_start(&game_timer);
	
	while (game_running) {
		//determine actual time since last frame, then reset timer
		uint32_t dt = Timer_elapsed(&game_timer);
		Timer_start(&game_timer);

		//poll events from queue
		while (SDL_PollEvent(&event) != 0) {
			//close button
			if (event.type == SDL_QUIT) {
				game_running = false;
				break;
			}

			//mouse moved
			else if (event.type == SDL_MOUSEMOTION) {
				mouse.x = event.motion.x;
				mouse.y = event.motion.y;
			}

			//any mouse button down
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					mouse.down = true;
				}
			}

			//any mouse button up
			else if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					mouse.down = false;
				}
			}

			//a window event
			else if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					gfx_resize(event.window.data1 ,event.window.data2);
				}
			}
		}

		game_loop(dt / (1000.0f / GAME_FPS));

		//cap FPS
		uint32_t dtCurrent = Timer_elapsed(&game_timer);
		if (dtCurrent < 1000/GAME_FPS) {
			SDL_Delay(1000/GAME_FPS - dtCurrent);
		}
	}

	particles_destroy();
	gfx_destroy();
	return 0;
}

void game_loop(float timescale) {
	//calculate parameters for new particles
	float nx,ny;  //new position
	float dx,dy;  //change in position
	float len;    //length of position change vector
	float vm;     //velocity multiplier

	//mouse input
	if (true || mouse.down) {
		nx = mouse.x;
		ny = mouse.y;
		dx = nx - px;
		dy = ny - py;
		len = sqrt(dx*dx + dy*dy);
		vm = 0.45;
	}

	//demo mode
	else {
		nx = gfx_dim.w * 0.5 + sin(Timer_now() * 0.008) * gfx_dim.w * 0.2;
		ny = gfx_dim.h * 0.5 + cos(Timer_now() * 0.008) * gfx_dim.h * 0.2;
		dx = nx - px;
		dy = ny - py;
		len = sqrt(dx*dx + dy*dy);
		vm = 1.2;
	}

	//create new particles
	//draws a line of particles between old and new position
	float x=px, y=py; //set start position to previous (last frame) position
	float d0 = atan2(dy,dx); //set base direction to the angle of change in position
	float s0 = MIN(len, 140); //set base speed

	//step through change in position, one pixel at a time
	for (int i=0; i<len; i++) {
		float xx = x + dx * i / len; //calculate current x
		float yy = y + dy * i / len; //calculate current y

		//make 5 particles
		for (int j=0; j<50; j++) {
			//calculate a random velocity vector
			float d = randfl(0, 6.28);
			float s = randfl(0.2, 0.8);

			float rd0 = randfl(-0.4,0.4); //random variation to base direction
			float rs = randfl(0.5,1.0); //random variation to base speed

			//calculate velocity as sum of random vector and base direction and speed
			//base direction and speed have random variation added.
			float vx = qcos(d0 + rd0)*s0*rs*vm + qcos(d) * s;
			float vy = qsin(d0 + rd0)*s0*rs*vm + qsin(d) * s;

			//add thew new particle
			particle_new(xx, yy, vx, vy);
		}
	}

	//store current position as new previous position
	px = nx;
	py = ny;

	//advance simulation and render
	particles_step(timescale);
	gfx_draw();
}