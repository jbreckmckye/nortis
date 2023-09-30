#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <stdio.h>
#include <stdbool.h>

/**
 * SDL2 Hello World adapted from https://lazyfoo.net/tutorials/SDL/01_hello_SDL/index2.php
 * Just using this to learn the ropes :-)
 */


const int WIDTH = 640;
const int HEIGHT = 480;
const char* FONT_PATH = "macos/IBMPlexMono-SemiBold.ttf";

SDL_Window* window = NULL;
// See previous commit for software renderer version using SDL_Surface
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

bool init_SDL() {
  // Can retrieve last error using SDL_GetError(), useful
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialise, error: %s\n", SDL_GetError());
    return false;
  }

  window = SDL_CreateWindow(
    "Hello SDL", // Title
    SDL_WINDOWPOS_UNDEFINED, // x position
    SDL_WINDOWPOS_UNDEFINED, // y position
    WIDTH, // width
    HEIGHT, // height
    SDL_WINDOW_SHOWN // visible flag
  );

  if (window == NULL) {
    printf("SDL could not create window, error: %s\n", SDL_GetError());
    return false;
  }

  // See previous commit for software renderer version

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == NULL) {
    printf("SDL could not create renderer, error: %s\n", SDL_GetError());
  }

  if (TTF_Init() != 0) {
    printf("SDL could not initialise TTF font module: %s\n", SDL_GetError());
  }
  font = TTF_OpenFont(FONT_PATH, 30);
  if (font == NULL) {
    printf("SDL could not open the font at path %s, error was: %s\n", FONT_PATH, TTF_GetError());
  }
  TTF_SetFontHinting(font, TTF_HINTING_NORMAL);

  return true;
}

void draw_text(char* text, int x, int y, int* widthWas) {
  SDL_Color white = {0xFF, 0xFF, 0xFF};
  SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, white);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_Rect textRect = {x, y, surface->w, surface->h};
  *widthWas = textRect.w;
  SDL_RenderCopy(renderer, texture, NULL, &textRect);

  // Must free resources after allocating them :-)
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

SDL_Vertex* corner_triangle(float x1, float y1, float widthHeight, SDL_Color solidColour) {
  SDL_Vertex bottomLeft = {{x1, y1}, solidColour, {1, 1}};
  SDL_Vertex topRight = {{x1 + widthHeight, y1 - widthHeight}, solidColour, {1, 1}};
  SDL_Vertex bottomRight = {{x1 + widthHeight, y1}, solidColour, {1, 1}};

  // Had a bit of doubt about how to free this structure, was confirmed at
  // https://stackoverflow.com/a/60262738
  SDL_Vertex* vertices = malloc(3 * sizeof(SDL_Vertex));
  if (!vertices) {
    return NULL;
  }
  vertices[0] = bottomLeft;
  vertices[1] = topRight;
  vertices[2] = bottomRight;
  return vertices;
}

void draw_block() {
  SDL_SetRenderDrawColor(renderer, 0xB3, 0x0C, 0x0C, 0xFF);
  SDL_Rect fillRect = { WIDTH / 4, HEIGHT / 4, WIDTH / 2, HEIGHT / 2};
  SDL_RenderFillRect(renderer, &fillRect);

  SDL_SetRenderDrawColor(renderer, 0xD3, 0x2C, 0x2C, 0xFF);
  SDL_Rect topBright = { WIDTH / 4, HEIGHT / 4, WIDTH / 2, 8 };
  SDL_Rect leftBright = { WIDTH / 4, HEIGHT / 4, 8, HEIGHT / 2 };
  SDL_RenderFillRect(renderer, &topBright);
  SDL_RenderFillRect(renderer, &leftBright);

  SDL_SetRenderDrawColor(renderer, 0x83, 0x0C, 0x0C, 0xFF);
  SDL_Rect bottomDark = { WIDTH / 4 + 8, ((HEIGHT / 4) * 3) - 8, (WIDTH / 2) - 8, 8 };
  SDL_Rect rightDark = { ((WIDTH / 4) * 3) - 8, HEIGHT / 4 + 8, 8, HEIGHT / 2 - 8 };
  SDL_RenderFillRect(renderer, &bottomDark);
  SDL_RenderFillRect(renderer, &rightDark);

  SDL_Color darkColour = {0x83, 0x0C, 0x0C};
  SDL_Vertex* cornerTop = corner_triangle(
    ((WIDTH / 4) * 3) - 8,
    (HEIGHT / 4) + 8,
    8,
    darkColour
  );
  SDL_Vertex* cornerBottom = corner_triangle(
    (WIDTH / 4),
    ((HEIGHT / 4) * 3),
    8,
    darkColour
  );

  SDL_RenderGeometry(renderer, NULL, cornerTop, 3, NULL, 0);
  free(cornerTop);
  SDL_RenderGeometry(renderer, NULL, cornerBottom, 3, NULL, 0);
  free(cornerBottom);
}

void draw_window() {
  SDL_SetRenderDrawColor(renderer, 0x00, 0x48, 0x89, 0xFF); // A nice blue colour
  SDL_RenderFillRect(
    renderer, // target renderer
    NULL // rect to fill, or NULL for whole renderer
  ); // renders using previously set renderDrawColor

  int drawnTextWidth = 0;
  draw_text("Hello SDL! This is some text.", 64, 56, &drawnTextWidth);
  printf("Drawn text width was %d pixels\n", drawnTextWidth);

  draw_block();

  // Updates screen by flipping front and back buffer (dual buffer rendering model)
  // Would use SDL_UpdateWindowSurface(window) if software rendering
  SDL_RenderPresent(renderer);
}

void cleanup() {
  SDL_DestroyRenderer(renderer);
  renderer = NULL;

  SDL_DestroyWindow(window);
  window = NULL;

  // Must call TTF_CloseFont() before TTF_Quit(), not safe to do in reverse order
  TTF_CloseFont(font);

  TTF_Quit();

  SDL_Quit();
}

// SDL requires this function signature of main() for cross-platform compatibility
int main(int argc, char* args[]) {
  printf("Starting SDL app\n");

  if (!init_SDL()) {
    printf("Exiting SDL app with error code 1\n");
    cleanup();
    return 1;
  }

  draw_window();

  bool quit = false;
  SDL_Event e;

  // On MacOS we must call SDL_PumpEvents / SDL_PollEvent before window will display. But we need to do that to handle input anyway.
  // Context here: https://stackoverflow.com/questions/34424816/sdl-window-does-not-show
  //SDL_PumpEvents();

  while (!quit) {
    // Goes through all input events until event queue empty. Queue is FIFO
    while (SDL_PollEvent(&e) != 0) {
      // This will handle Ctrl+C too
      if (e.type == SDL_QUIT) {
        quit = true;

      } else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
          case SDLK_ESCAPE:
            quit = true;
            break;
          case SDLK_UP:
          case SDLK_w:
            printf("Hit UP or W\n");
            break;
          case SDLK_DOWN:
          case SDLK_s:
            printf("Hit DOWN or S\n");
            break;
          case SDLK_LEFT:
          case SDLK_a:
            printf("Hit LEFT or A\n");
            break;
          case SDLK_RIGHT:
          case SDLK_d:
            printf("Hit RIGHT or D\n");
            break;
          default:
            break;
        }
      }
    }
  }

  printf("We are quitting in 2 seconds!\n");
  SDL_Delay(2000);

  cleanup();

  return 0;
}
