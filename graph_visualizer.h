#ifndef _H_GRAPH_VISUALIZER_
#define _H_GRAPH_VISUALIZER_

#include "autograd.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

int init(Value *node);
uint32_t render_node(SDL_Renderer *renderer, Value *node, uint32_t x,
                     uint32_t y, char *label, int scroll_x, int scroll_y);
void render_text(SDL_Renderer *renderer, uint32_t x, uint32_t y, char *text);
uint32_t SDL_RenderDrawCircle(SDL_Renderer *renderer, uint32_t x, uint32_t y,
                              uint32_t radius);

void _render_graph(SDL_Renderer *renderer, Value *root, int scroll_x,
                   int scroll_y);

#endif
