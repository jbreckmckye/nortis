#include "gfx.h"

int main(int argc, char* args[]) {
  printf("Start");

  if (gfx_init() == false) {
    printf("Startup failed, exiting");
    gfx_cleanup();
    return 1;
  }

  gfx_drawDebug();

  bool quit = false;
  SDL_Event event;

  while (!quit) {
    while (SDL_PollEvent(&event) != 0) { // Drain queue of events since last loop iteration
      if (event.type == SDL_QUIT) {
        quit = true;
      }
    }
  }

  printf("Quit");
  gfx_cleanup();
  return 0;
}
