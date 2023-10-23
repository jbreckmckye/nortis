#include <stdlib.h>
#include <time.h>

#include "./gfx/gfx.h"
#include "game.h"

const char* TITLE = "NOTRIS ";

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

  while (!quit) {
    // Drain queue of events since last loop iteration
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        quit = true;
        break;
      }
    }

    //Uint32 start = SDL_GetTicks();
    game_updateDrawState();
    gfx_drawDebug(game_p_drawField);
    //Uint32 ticks = SDL_GetTicks() - start;
    //printf("Frame took %d ms\n", ticks);
  }

  gfx_cleanup();
  return 0;
}
