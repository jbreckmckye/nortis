/**
 * ############################################################################
 * #       NOTRIS - THE FAMILIAR BUT LEGALLY DISTINCT BLOCK PUZZLE GAME       #
 * ############################################################################
 */

#include <psxapi.h>
#include <psxgpu.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include "game/game.h"
#include "game/pad.h"
#include "gfx/gfx.h"
#include "gfx/ui.h"
#include "defs.h"

int main(int argc, char** argv) {
  // Configure graphics
  gfx_init();
  pad_init();

  TIM_IMAGE tim;
  gfx_loadFontTexture(&tim);

  // Title screen - helps influence RNG
  bool started = false;
  while (!started) {
    if (pad_getInput() == INPUT_RESTART) {
      started = true;
    }

    ui_renderTitleScreen();

    gfx_endFrame();
  }

  // Set up new game state
  game_actionRestart();
  int tickFrames = 0;
  int tickSpeed = game_getSpeed();

  while (1) {
    // Take controller input
    switch (pad_getInput()) {
      case INPUT_LEFT:
        game_actionMovement(MOVE_LEFT);
        break;
      case INPUT_RIGHT:
        game_actionMovement(MOVE_RIGHT);
        break;
      case INPUT_ROTATE:
        game_actionRotate();
        break;
      case INPUT_DROP:
        game_actionHardDrop();
        break;
      case INPUT_RESTART:
        if (game_p_state->playState == PLAY_GAMEOVER) {
          game_actionRestart();
        }
        break;
    }

    // Gravity
    tickFrames++;
    if (tickFrames >= tickSpeed) {
      if (game_p_state->playState == PLAY_PLAYING) {
        game_actionSoftDrop();
      }
      tickFrames = 0;
      tickSpeed = game_getSpeed();
    }

    // Draw UI
    ui_render(game_p_state);

    // Draw pieces
    game_updateDrawState();
    for (int y = 0; y < DRAW_HEIGHT; y++) {
      for (int x = 0; x < WIDTH; x++) {
        BlockNames block = (*game_p_drawField)[y][x];
        if (block) {
          ui_renderBlock(x, y, block);
        }
      }
    }

    // Performs vsync & frameswitch
    gfx_endFrame();
  }

  return 0;
}
