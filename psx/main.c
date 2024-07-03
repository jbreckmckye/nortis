/**
 * ############################################################################
 * #       NOTRIS - THE FAMILIAR BUT LEGALLY DISTINCT BLOCK PUZZLE GAME       #
 * ############################################################################
 */

#include <psxgpu.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include "game/game.h"
#include "game/pad.h"
#include "gfx/gfx.h"
#include "gfx/ui.h"
#include "defs.h"

void applyInput(GameInputs thisFrameInput) {
  switch (thisFrameInput) {
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
  }
}

int main(int argc, char** argv) {
  gfx_init();
  pad_init();

  TIM_IMAGE tim;
  gfx_loadFontTexture(&tim);

  // Set up game state
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
