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

int main(int argc, char** argv) {
  gfx_init();
  pad_init();

  TIM_IMAGE tim;
  gfx_loadFontTexture(&tim);

  // Set up game state
  game_actionRestart();
  int frameCount = 0;
  int speed = game_getSpeed();

  while (1) {
    //printf("Frame %d, speed %d, playstate %d, block %d\n", frameCount, speed, game_p_gameState->playState, game_p_gameState->blockName);

    // Apply timed events
    frameCount++;
    if (frameCount >= speed) {
      frameCount = 0;
      game_actionSoftDrop();
      speed = game_getSpeed();
    }

    //pad_debug();

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

    printf(
      "Pointers buf0=%p, buf0.prims=%p; buf1=%p, buf1.prims=%p, prim=%p, bufID=%d \n",
      &(gfx_p_ctx->buffers[0]),
      &(gfx_p_ctx->buffers[0].primitivesBuffer),
      &(gfx_p_ctx->buffers[1]),
      &(gfx_p_ctx->buffers[1].primitivesBuffer),
      gfx_p_ctx->p_primitive,
      gfx_p_ctx->bufferID
    );

    // Performs vsync & frameswitch
    gfx_endFrame();
  }

  return 0;
}
