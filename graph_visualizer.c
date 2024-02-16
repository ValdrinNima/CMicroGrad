#include "graph_visualizer.h"
#include "math.h"
#include <SDL2/SDL_stdinc.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#define WINDOW_X 0
#define WINDOW_Y 0
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000

int init(Value *node) {
  int res = TTF_Init();

  if (res == -1) {
    fprintf(stderr, "ERROR: TTF could not be initalized.");
  }
  SDL_Window *window;
  SDL_Renderer *renderer;

  if (SDL_INIT_VIDEO < 0) {
    fprintf(stderr, "ERROR: Could not initialize window.");
  }

  window = SDL_CreateWindow("Graph", WINDOW_X, WINDOW_Y, WINDOW_WIDTH,
                            WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS);

  if (!window) {
    fprintf(stderr, "ERROR: Could not initialize window.");
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer) {
    fprintf(stderr, "ERROR: Could not initialize renderer.");
  }
  // Draw white background
  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 255);
  SDL_RenderClear(renderer);

  int scroll_x = 0;            // Horizontal scrolling offset
  int scroll_y = 0;            // Vertical scrolling offset
  const int scroll_speed = 10; // Speed of scrolling

  // Render graph once
  _render_graph(renderer, node, scroll_x, scroll_y);
  reset_visited(node);
  SDL_RenderPresent(renderer);

  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
          running = false;
          break;
        case SDLK_UP:
          scroll_y += scroll_speed;
          break;
        case SDLK_DOWN:
          scroll_y -= scroll_speed;
          break;
        case SDLK_LEFT:
          scroll_x += scroll_speed;
          break;
        case SDLK_RIGHT:
          scroll_x -= scroll_speed;
          break;
        }
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 255);
        SDL_RenderClear(renderer);
        _render_graph(renderer, node, scroll_x, scroll_y);
        reset_visited(node);
        SDL_RenderPresent(renderer);

        break;
      }
    }

    SDL_Delay(16);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

uint32_t render_node(SDL_Renderer *renderer, Value *node, uint32_t x,
                     uint32_t y, char *label, int scroll_x, int scroll_y) {
  SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0x55, 255);
  uint32_t node_width = 100;
  uint32_t node_height = 50;
  uint32_t line_length = 50;
  uint32_t radius = 10;
  SDL_Rect title_block;
  SDL_Rect data_block;
  SDL_Rect grad_block;

  x += scroll_x;
  y += scroll_y;

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
  grad_block.y = mid_y;

  // Label
  render_text(renderer, x, y, label);

  // Data
  int len = snprintf(NULL, 0, "%f", node->data);
  char *data_repr = malloc(len + 1);
  snprintf(data_repr, len + 1, "%.2f", node->data);

  render_text(renderer, x, y + node_height / 2, data_repr);
  free(data_repr);

  // Grad
  len = snprintf(NULL, 0, "%f", node->grad);
  char *grad_repr = malloc(len + 1);
  snprintf(grad_repr, len + 1, "%.2f", node->grad);

  render_text(renderer, x + node_width / 2, y + node_height / 2, grad_repr);
  free(grad_repr);

  for (size_t i = 0; i < node->parent_y_positions.count; ++i) {
    uint32_t parent_y = node->parent_y_positions.yPositions[i];
    SDL_RenderDrawLine(renderer, x + node_width, mid_y,
                       x + node_width - radius + line_length, parent_y);
  }

  // Render the downstream edge with the operation type only if it is a leaf
  if (!(node->prev_count == 0)) {
    SDL_RenderDrawLine(renderer, x, mid_y, x - line_length + radius, mid_y);
    SDL_RenderDrawCircle(renderer, x - line_length, mid_y, radius);
  }

  SDL_RenderDrawRect(renderer, &title_block);
  SDL_RenderDrawRect(renderer, &data_block);
  SDL_RenderDrawRect(renderer, &grad_block);

  return mid_y;
}

void render_text(SDL_Renderer *renderer, uint32_t x, uint32_t y, char *text) {
  TTF_Font *Sans = TTF_OpenFont("Mono.ttf", 14);
  if (!Sans) {
    fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
    return;
  }

  SDL_Color font_color = {0, 0, 0, 0};
  SDL_Color bg_color = {0xff, 0xff, 0xff, 0xff};

  SDL_Surface *surfaceMessage =
      TTF_RenderText_Shaded(Sans, text, font_color, bg_color);
  if (!surfaceMessage) {
    fprintf(stderr, "ERROR: Failed to create surface: %s\n", TTF_GetError());
    TTF_CloseFont(Sans);
    return;
  }

  SDL_Texture *Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
  if (!Message) {
    fprintf(stderr, "ERROR: Failed to create texture: %s\n", SDL_GetError());
    SDL_FreeSurface(surfaceMessage);
    TTF_CloseFont(Sans);
    return;
  }

  SDL_Rect Message_rect;
  Message_rect.x = x;
  Message_rect.y = y;
  Message_rect.w = 40;
  Message_rect.h = 25;

  SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
  SDL_DestroyTexture(Message);
  SDL_FreeSurface(surfaceMessage);
  TTF_CloseFont(Sans);
}

uint32_t SDL_RenderDrawCircle(SDL_Renderer *renderer, uint32_t x, uint32_t y,
                              uint32_t radius) {
  uint32_t offset_x, offset_y, d;
  int32_t status;

  offset_x = 0;
  offset_y = radius;
  d = radius - 1;
  status = 0;

  while (offset_y >= offset_x) {
    status += SDL_RenderDrawPoint(renderer, x + offset_x, y + offset_y);
    status += SDL_RenderDrawPoint(renderer, x + offset_y, y + offset_x);
    status += SDL_RenderDrawPoint(renderer, x - offset_x, y + offset_y);
    status += SDL_RenderDrawPoint(renderer, x - offset_y, y + offset_x);
    status += SDL_RenderDrawPoint(renderer, x + offset_x, y - offset_y);
    status += SDL_RenderDrawPoint(renderer, x + offset_y, y - offset_x);
    status += SDL_RenderDrawPoint(renderer, x - offset_x, y - offset_y);
    status += SDL_RenderDrawPoint(renderer, x - offset_y, y - offset_x);

    if (status < 0) {
      status = -1;
      break;
    }

    if (d >= 2 * offset_x) {
      d -= 2 * offset_x + 1;
      offset_x += 1;
    } else if (d < 2 * (radius - offset_y)) {
      d += 2 * offset_y - 1;
      offset_y -= 1;
    } else {
      d += 2 * (offset_y - offset_x - 1);
      offset_y -= 1;
      offset_x += 1;
    }
  }
  return status;
}

void init_parent_y_positions(ParentYPositions *pyPositions) {
  pyPositions->count = 0;
  pyPositions->capacity = 4;
  pyPositions->yPositions =
      (uint32_t *)malloc(pyPositions->capacity * sizeof(uint32_t));
}

void add_parent_y_position(ParentYPositions *pyPositions, uint32_t yPos) {
  if (pyPositions->count >= pyPositions->capacity) {
    // Resize the dynamic array if more space is needed
    pyPositions->capacity *= 2;
    pyPositions->yPositions = (uint32_t *)realloc(
        pyPositions->yPositions, pyPositions->capacity * sizeof(uint32_t));
  }
  pyPositions->yPositions[pyPositions->count++] = yPos;
}

void free_parent_y_positions(ParentYPositions *pyPositions) {
  free(pyPositions->yPositions);
  pyPositions->yPositions = NULL;
  pyPositions->count = 0;
  pyPositions->capacity = 0;
}

void _render_graph(SDL_Renderer *renderer, Value *node, int scroll_x,
                   int scroll_y) {
  uint32_t start_position_x = 850;
  uint32_t start_position_y = 150;
  uint32_t vertical_layer_offset = 100;
  uint32_t horizontal_layer_offset = 200;

  Value *queue[100];
  size_t front = 0;
  size_t rear = 0;

  // Grad of output with respect to outupt is 1
  node->grad = 1;

  // Enqueue the output node and initialize y-positions
  node->visited = true;
  init_parent_y_positions(&node->parent_y_positions);
  queue[rear++] = node;

  // Keep track of the current depth for rendering the graph
  size_t depth = 0;

  while (front < rear) {
    size_t nodesAtCurrentLevel = rear - front;

    // Iterate over all nodes in the queue i.e. all nodes at the same level
    for (size_t i = 0; i < nodesAtCurrentLevel; ++i) {
      Value *current = queue[front++];

      uint32_t current_x = start_position_x - (depth * horizontal_layer_offset);
      uint32_t current_y = start_position_y + (i * vertical_layer_offset);

      char label[50];
      sprintf(label, "node %ld", front);
      // Render the current node and store its y-position so that we can add it
      // to the y-positions
      uint32_t out_y = render_node(renderer, current, current_x, current_y,
                                   label, scroll_x, scroll_y);

      // Enqueue child nodes and update their parent y-positions
      for (size_t j = 0; j < current->prev_count; j++) {
        Value *child = current->prev[j];
        if (child != NULL && !child->visited) {
          child->visited = true;
          // Initialize parent y-positions for child if it's the first time
          if (child->parent_y_positions.yPositions == NULL) {
            init_parent_y_positions(&child->parent_y_positions);
          }
          queue[rear++] = child;
        }
        if (child != NULL) {
          add_parent_y_position(&child->parent_y_positions, out_y);
        }
      }
    }
    // Increment depth after processing all nodes at the current level
    depth++;
  }

  // Free the parent y-positions of all nodes after rendering
  for (size_t i = 0; i < rear; i++) {
    free_parent_y_positions(&queue[i]->parent_y_positions);
  }
}
