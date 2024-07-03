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

// Spells out 'notris' when masked & shifted with a 22 bit long mask
// static int32_t titleMask = 0b1000000000000000000000;
// static int32_t titlePattern[5] = {
//   0b1001011101110111010111,
//   0b1101010100100101010100,
//   0b1011010100100110010111,
//   0b1001010100100101010001,
//   0b1001011100100101010111
// };

// BlockNames getTitleEffectBlock(int x, int y, int timer) {
//   // Blocks are printed in gradually
//   int ticks = timer / 2;
//   int position = (y * 22) + x;
//   if (position > ticks) {
//     return BLOCK_NONE; // skip if not to be displayed yet
//   }

//   int32_t line = titlePattern[y];
//   int32_t mask = titleMask >> x;
//   if (line & mask) {
//     // Gradient effect
//     return
//       x < 4 ? BLOCK_S :
//       x < 8 ? BLOCK_L :
//       x < 12 ? BLOCK_T :
//       x < 16 ? BLOCK_J :
//       x < 19 ? BLOCK_I :
//       BLOCK_O;
//   }
// }

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
