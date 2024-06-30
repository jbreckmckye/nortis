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

  // Inputs
  GameInputs lastInput = INPUT_NONE;
  GameInputs nextInput = INPUT_NONE;
  int inputHeldForFrames = 0;

  while (1) {
    // Implement game 'ticks'
    tickFrames++;
    if (tickFrames >= tickSpeed) {
      game_actionSoftDrop();
      tickFrames = 0;
      tickSpeed = game_getSpeed();
    }

    // Get input + length it has been held
    nextInput = pad_getInput();
    if (nextInput == lastInput) {
      inputHeldForFrames++;
    } else {
      inputHeldForFrames = 0;
    }

    // Reset the 'hold' every half second for movement inputs
    if (inputHeldForFrames > 30) {
      if (nextInput == INPUT_LEFT || nextInput == INPUT_RIGHT) {
        inputHeldForFrames = 0;
      }
    }

    // Apply fresh inputs that haven't been 'held' over frames
    if (inputHeldForFrames == 0) {
      applyInput(nextInput);
    }

    lastInput = nextInput;


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
