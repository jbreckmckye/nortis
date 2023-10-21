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
  game_updateDrawField();
  gfx_drawDebug(game_p_drawField);

  bool quit = false;
  SDL_Event event;

  while (!quit) {
    while (SDL_PollEvent(&event) != 0) { // Drain queue of events since last loop iteration
      if (event.type == SDL_QUIT) {
        quit = true;
      }
    }
  }

  printf("Quit\n");
  gfx_cleanup();
  return 0;
}
