#ifndef GFX_GUARD
#define GFX_GUARD

#define CR 2
#define CG 1
#define CB 0
#define CA 3

SDL_Rect gfx_dim;
SDL_Texture *pixel_buffer;
char *pixels;

#endif

int gfx_init();
void gfx_destroy();
void gfx_draw();
void gfx_resize(int width, int height);
void gfx_setDrawHue(SDL_Renderer *rnd, float hue);