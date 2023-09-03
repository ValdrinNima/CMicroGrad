#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_X 0
#define WINDOW_Y 0
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000


uint32_t render_node(SDL_Renderer *renderer, uint32_t x, uint32_t y, uint32_t out_y);
void render_text(SDL_Renderer *renderer, uint32_t x, uint32_t y, char *text);
int SDL_RenderDrawCircle(SDL_Renderer * renderer, int x, int y, int radius);

int main(void) {
    int res = TTF_Init();

    if (res == -1) {
        fprintf(stderr, "ERROR: TTF could not be initalized.");
    }
    SDL_Window *window;
    SDL_Renderer *renderer;

    if (SDL_INIT_VIDEO < 0) {
        fprintf(stderr, "ERROR: Could not initialize window.");
    }

    window = SDL_CreateWindow(
            "Graph",
            WINDOW_X,
            WINDOW_Y,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_BORDERLESS
            );

    putenv("SDL_VIDEO_CENTERED=1"); 


    if (!window) {
        fprintf(stderr, "ERROR: Could not initialize window.");
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        fprintf(stderr, "ERROR: Could not initialize renderer.");
    }



    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYUP:
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                    }
                    break;
            }

        }

        SDL_RenderClear(renderer);
        // Render loop start

        uint32_t start_position_x = 800;
        uint32_t start_position_y = 450;
        uint32_t layer_offset = 100 + (2 * 50) + 10;
        uint32_t start = render_node(renderer, start_position_x, start_position_y, -1);
        uint32_t start2 = render_node(renderer, start_position_x-layer_offset, start_position_y-200, start);
        uint32_t start3 = render_node(renderer, start_position_x-layer_offset, start_position_y, start);


        // Render loop end

        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 255);
        SDL_RenderPresent(renderer);
        SDL_Delay(1000);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

uint32_t render_node(SDL_Renderer *renderer, uint32_t x, uint32_t y, uint32_t out_y) {
    SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 255);
    uint32_t node_width = 100;
    uint32_t node_height = 50;
    uint32_t line_length = 50;
    uint32_t radius = 10;
    SDL_Rect title_block;
    SDL_Rect data_block;
    SDL_Rect grad_block;


    uint32_t mid_y = y + node_height / 2;
    uint32_t mid_x = x + node_width / 2;

    title_block.w = node_width;
    title_block.h = node_height / 2;
    title_block.x = x;
    title_block.y = y;

    data_block.w = node_width / 2;
    data_block.h = node_height / 2;
    data_block.x = x;
    data_block.y = mid_y;

    grad_block.w = node_width / 2;
    grad_block.h = node_height / 2;
    grad_block.x = mid_x;
    grad_block.y =  mid_y;
    // Label
    render_text(renderer, x, y, "Label");
    // Data
    render_text(renderer, x , y + node_height / 2, "10");
    // Grad
    render_text(renderer, x + node_width / 2 , y + node_height / 2, "42");

    // Upstream edge
    if (out_y != -1){
        SDL_RenderDrawLine(renderer, x+node_width, mid_y, x+node_width+line_length, out_y);
    }
    // Downstream edge
    SDL_RenderDrawLine(renderer, x, mid_y, x-line_length+radius, mid_y);

    SDL_RenderDrawCircle(renderer, x-line_length, mid_y, radius);
    // Operation
    //render_text(renderer, x-line_length-(radius/2), mid_y, "+");

    SDL_RenderDrawRect(renderer, &title_block);
    SDL_RenderDrawRect(renderer, &data_block);
    SDL_RenderDrawRect(renderer, &grad_block);

    return mid_y;
}

void render_text(SDL_Renderer *renderer, uint32_t x, uint32_t y, char *text)  {
    TTF_Font *Sans = TTF_OpenFont("Mono.ttf", 14);
    SDL_Color font_color = {0, 0, 0, 0};
    SDL_Color bg_color = {0xff, 0xff, 0xff};

    SDL_Surface *surfaceMessage = TTF_RenderText_Shaded(Sans, text,font_color, bg_color); 

    SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect;
    Message_rect.x = x;
    Message_rect.y = y;
    Message_rect.w = 25;
    Message_rect.h = 25;
                          //
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
    SDL_DestroyTexture(Message);
    SDL_FreeSurface(surfaceMessage);
}

int SDL_RenderDrawCircle(SDL_Renderer * renderer, int x, int y, int radius) {
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }
    return status;
}
