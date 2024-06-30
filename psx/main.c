/**
 * ############################################################################
 * #       NOTRIS - THE FAMILIAR BUT LEGALLY DISTINCT BLOCK PUZZLE GAME       #
 * ############################################################################
 */

#include <psxgpu.h>
#include <stdio.h>
#include <stdbool.h>

#include "game/game.h"
#include "game/pad.h"
#include "gfx/gfx.h"
#include "gfx/ui.h"
#include "defs.h"

void listenPlayControls(GameInputs lastFrameInput, GameInputs thisFrameInput) {
  if (thisFrameInput == lastFrameInput) {
    return;
  }

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

  // Inputs
  GameInputs lastFrameInput = INPUT_NONE;
  GameInputs thisFrameInput = INPUT_NONE;

  while (1) {
    // Implement game 'ticks'
    tickFrames++;
    if (tickFrames >= tickSpeed) {
      // Gravity on blocks
      game_actionSoftDrop();
      // Reset movement debounce
      if (lastFrameInput == INPUT_LEFT || lastFrameInput == INPUT_RIGHT) {
        lastFrameInput = INPUT_NONE;
      }

      tickFrames = 0;
      tickSpeed = game_getSpeed();
    }

    // Apply controls
    thisFrameInput = pad_getInput();
    if (game_p_gameState->playState == PLAY_PLAYING) {
      listenPlayControls(lastFrameInput, thisFrameInput);
    }
    lastFrameInput = thisFrameInput;

    // Draw UI
    ui_render(game_p_gameState);

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
