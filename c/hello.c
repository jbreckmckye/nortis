#include "SDL2/SDL.h"
#include <stdio.h>
#include <stdbool.h>

/**
 * SDL2 Hello World adapted from https://lazyfoo.net/tutorials/SDL/01_hello_SDL/index2.php
 */

const int WIDTH = 640;
const int HEIGHT = 480;

SDL_Window* window = NULL;
SDL_Surface* surface = NULL; // A surface is a pixel buffer we draw to using software rendering

bool initSDL() {
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

  surface = SDL_GetWindowSurface(window);

  return true;
}

void cleanup() {
  // Frees memory associated with window & its display surface
  SDL_DestroyWindow(window);
  window = NULL;

  SDL_Quit();
}

// SDL requires this function signature of main() for cross-platform compatibility
int main(int argc, char* args[]) {
  printf("Starting SDL app\n");

  if (!initSDL()) {
    printf("Exiting SDL app with error code 1\n");
    return 1;
  }

  bool quit = false;
  SDL_Event e;

  // Draw to surface
  SDL_FillRect(
    surface, // target surface
    NULL, // rect to fill, or NULL for whole surface
    SDL_MapRGB( // "Map RGB triple to an opaque pixel value for a given pixel format" - docs
      surface->format,
      0x00,
      0x48,
      0x89
    ) // A nice blue colour
  );

  // Have to update a window after drawing to its surface
  // This swaps the front and back buffer (dual buffer rendering)
  SDL_UpdateWindowSurface(window);

  // On MacOS we must call SDL_PumpEvents / SDL_PollEvent before window will display. But we need to do that to handle input anyway.
  // Context here: https://stackoverflow.com/questions/34424816/sdl-window-does-not-show
  //SDL_PumpEvents();

  while (!quit) {
    // Goes through all input events until event queue empty. Queue is FIFO
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
        printf("We are quitting in 2 seconds!\n");

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

  SDL_Delay(2000);

  cleanup();

  return 0;
}


