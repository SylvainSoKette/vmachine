#ifndef __render_h
#define __render_h

#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

const char* WINDOW_TITLE = "vmachine";
const int WINDOW_WIDTH = 608;
const int WINDOW_HEIGHT = 512;
const Uint32 WINDOW_FLAGS = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

typedef struct {
    int r;
    int g;
    int b;
    int a;
} Color;

Color COLOR_BLACK           = { .r = 0,   .g = 0,   .b = 0,   .a = 255, };
Color COLOR_WHITE           = { .r = 230, .g = 230, .b = 230, .a = 255, };
Color COLOR_WHITE_HIGHLIGHT = { .r = 230, .g = 230, .b = 255, .a = 255, };    
Color COLOR_BLACK_HIGHLIGHT = { .r = 20,  .g = 20,  .b = 50,  .a = 255, };

void init_sdl_window_renderer(SDL_Window **window, SDL_Renderer **renderer)
{
    if(SDL_Init(SDL_INIT_EVERYTHING))
    {
        printf("[ERROR] Could not initialize SDL.\n");
    }

    *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        WINDOW_FLAGS
    );

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_SOFTWARE);
}

inline void draw_rect(SDL_Renderer* r, int x, int y, int w, int h, Color c)
{
	const SDL_Rect rect = {
		.x = x,
		.y = y,
		.w = w,
		.h = h
	};

    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);	
	SDL_RenderFillRect(r, &rect);
	/*
	for (int dy = y; dy < y + h; dy++)
    {
        for (int dx = x; dx < x + w; dx++)
        {
            SDL_RenderDrawPoint(r, dx, dy);
        }
    }
	*/
}

#endif // __render_h
