#ifndef _H_GRAPH_VISUALIZER_
#define _H_GRAPH_VISUALIZER_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "autograd.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

typedef struct Node
{
    Value value;
    int layer;    // Added to store the layer of the node
    int position; // Added to store the position in the layer
} Node;

int init(void);
uint32_t render_node(SDL_Renderer *renderer, uint32_t x, uint32_t y,
                     int32_t out_y, char *label, int32_t data, int32_t grad);
void render_text(SDL_Renderer *renderer, uint32_t x, uint32_t y, char *text);
uint32_t SDL_RenderDrawCircle(SDL_Renderer *renderer, uint32_t x, uint32_t y, uint32_t radius);

void render_graph(SDL_Renderer *renderer, Node *output_node);
uint32_t calculate_x_position(Node *node, int layer_width);
uint32_t calculate_y_position(Node *node, int layer);
void add_child(Node *parent, Node *child);
Node *create_node(char *label, float value, float grad);

#endif
