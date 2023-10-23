#include <stdlib.h>
#include <time.h>

#include "./gfx/gfx.h"
#include "game.h"

int main(int argc, char* argv[]) {
  printf("Start\n");

  // Ignore unused args
  (void) argv[argc - 1];

  if (gfx_init() == false) {
    printf("Startup failed, exiting\n");
    gfx_cleanup();
    return 1;
  }

  // Seed random number generator
  srand(time(NULL));

  game_actionRestart();

  bool quit = false;
  SDL_Event event;

  Uint64 gravityTicker = SDL_GetTicks64();

  while (!quit) {
    Uint64 start = SDL_GetTicks64();

    // Drain queue of events since last loop iteration
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        quit = true;
        break;
      }
    }

    if ((start - gravityTicker) > game_getSpeed()) {
      game_actionSoftDrop();
      gravityTicker = start;
    }

    game_updateDrawState();
    gfx_drawDebug(game_p_drawField);
    
    // Uint64 duration = SDL_GetTicks64() - start;
    // printf("Frame took %d ms\n", duration);

    // Limit to 60 fps
    while ((SDL_GetTicks64() - start) < 18) {
      SDL_Delay(1);
    }
  }

  gfx_cleanup();
  return 0;
}
