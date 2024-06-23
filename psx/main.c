/**
 * ############################################################################
 * #       NOTRIS - THE FAMILIAR BUT LEGALLY DISTINCT BLOCK PUZZLE GAME       #
 * ############################################################################
 */

#include <psxgpu.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <psxapi.h>

#include "gfx/gfx.h"
#include "gfx/ui.h"
#include "io/pad.h"
#include "defs.h"

int random() {
  // Pseudo-random number based on function timing (first piece is deterministic)
  int rootCount = GetRCnt(0);
  srand(rootCount);
  int result = rand();
}

int main(int argc, char** argv) {
  gfx_init();
  pad_init();

  TIM_IMAGE tim;
  gfx_loadFontTexture(&tim);

  while (1) {
    pad_debug();
    if (pad_buttons1()) {
      printf("RANDOM NUMBER %d \n", random());
    }

    // These could all be combined into a render frame fn
    ui_renderPlayArea();
    ui_renderTitle(true);
    ui_renderScores(5678, 9012);
    ui_renderControls(true);
    ui_renderKredits();

    ui_renderBlock(0, 2);
    ui_renderBlock(3, 3);
    ui_renderBlock(2, 4);
    ui_renderBlock(3, 4);
    ui_renderBlock(4, 4);
    ui_renderBlock(9, 25);

    // Performs vsync & frameswitch
    gfx_endFrame();
  }

  return 0;
}
