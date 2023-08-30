#include <SDL2/SDL_render.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_X 0
#define WINDOW_Y 0
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

void render_node(SDL_Renderer *renderer, uint32_t x, uint32_t y);
void render_text(SDL_Renderer *renderer, uint32_t x, uint32_t y);

int main(void) {
    TTF_Init();
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

        render_node(renderer, 100, 100);
        render_text(renderer, 100, 200);

        // Render loop end

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
        SDL_RenderPresent(renderer);

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void render_node(SDL_Renderer *renderer, uint32_t x, uint32_t y) {
    SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 255);
    uint32_t node_width = 100;
    uint32_t node_height = 50;
    SDL_Rect title_block;
    SDL_Rect data_block;
    SDL_Rect grad_block;

    title_block.w = node_width;
    title_block.h = node_height / 2;
    title_block.x = x;
    title_block.y = y;

    data_block.w = node_width / 2;
    data_block.h = node_height / 2;
    data_block.x = x;
    data_block.y = y + node_height / 2;

    grad_block.w = node_width / 2;
    grad_block.h = node_height / 2;
    grad_block.x = x + node_width / 2;
    grad_block.y =  y + node_height / 2;

    SDL_RenderDrawRect(renderer, &title_block);
    SDL_RenderDrawRect(renderer, &data_block);
    SDL_RenderDrawRect(renderer, &grad_block);

    TTF_Quit();

    return;
}

void render_text(SDL_Renderer *renderer, uint32_t x, uint32_t y)  {
    TTF_Font* Roboto = TTF_OpenFont("Roboto-Medium.ttf", 24);

    // this is the color in rgb format,
    // maxing out all would give you the color white,
    // and it will be your text's color
    SDL_Color White = {255, 255, 255};

    // as TTF_RenderText_Solid could only be used on
    // SDL_Surface then you have to create the surface first
    SDL_Surface* surfaceMessage =
        TTF_RenderText_Solid(Roboto, "put your text here", White); 

    // now you can convert it into a texture
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = x;  //controls the rect's x coordinate 
    Message_rect.y = y; // controls the rect's y coordinte
    Message_rect.w = 100; // controls the width of the rect
    Message_rect.h = 100; // controls the height of the rect
                          
    SDL_RenderDrawRect(renderer, &Message_rect);
                        
    SDL_RenderCopy(renderer, Message, NULL, &Message_rect);


}
