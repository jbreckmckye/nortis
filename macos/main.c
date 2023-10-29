#include <stdlib.h>
#include <time.h>

#include "./gfx/gfx.h"
#include "game.h"

void initRand() {
  // Seed random number generator
  // Discard the first rand, as it always is a multiple of 7
  // https://stackoverflow.com/questions/7866754/why-does-rand-7-always-return-0)
  srand(time(NULL));
  rand();
}

GameInputs parseKey(int keycode) {
  switch (keycode) {
    case SDLK_ESCAPE:
      return INPUT_QUIT;
    case SDLK_UP:
    case SDLK_w:
      return INPUT_UP;
    case SDLK_DOWN:
    case SDLK_s:
      return INPUT_DOWN;
    case SDLK_LEFT:
    case SDLK_a:
      return INPUT_LEFT;
    case SDLK_RIGHT:
    case SDLK_d:
      return INPUT_RIGHT;
    default:
      return 0;
  }
}

int main(int argc, char* argv[]) {
  printf("Start\n");

  // Ignore unused args
  (void) argv[argc - 1];

  if (gfx_init() == false) {
    printf("Startup failed, exiting\n");
    gfx_cleanup();
    return 1;
  }

  initRand();

  game_actionRestart();

  bool quit = false;
  SDL_Event event;
  Uint64 timeLastDrop = SDL_GetTicks64();

  while (!quit) {
    GameInputs input = INPUT_NONE;

    // Drain queue of events since last loop iteration
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_KEYDOWN) {
        input = parseKey(event.key.keysym.sym);
      }

      if (input == INPUT_QUIT || event.type == SDL_QUIT) {
        quit = true;
        break;
      }
    }

    Uint64 timeFrameStart = SDL_GetTicks64();

    if (game_getPlayState() == PLAY_PLAYING) {
      // Handle rotations and left/right before dropping
      if (input == INPUT_LEFT) {
        game_actionMovement(MOVE_LEFT);
      } else if (input == INPUT_RIGHT) {
        game_actionMovement(MOVE_RIGHT);
      } else if (input == INPUT_UP) {
        game_actionRotate();
      }

      // Handle timed or forced drops
      // These should reset the gravity timer
      if (input == INPUT_DOWN) {
        game_actionHardDrop();
        timeLastDrop = timeFrameStart;
      } else if ((timeFrameStart - timeLastDrop) > game_getSpeed()) {
        game_actionSoftDrop();
        timeLastDrop = timeFrameStart;
      }

    } else {
      printf("Game over, quitting :p\n");
      quit = true;
    }

    game_updateDrawState();
    gfx_drawDebug(game_p_drawField);
    
    // Uint64 duration = SDL_GetTicks64() - start;
    // printf("Frame took %d ms\n", duration);

    // Limit to 60 fps
    while ((SDL_GetTicks64() - timeFrameStart) < 18) {
      SDL_Delay(1);
    }
  }

  gfx_cleanup();
  return 0;
}
