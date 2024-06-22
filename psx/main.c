/**
 * ############################################################################
 * #       NOTRIS - THE FAMILIAR BUT LEGALLY DISTINCT BLOCK PUZZLE GAME       #
 * ############################################################################
 */

#include <psxgpu.h>
#include <stdio.h>
#include <stdbool.h>

#include "./gfx/gfx.h"
#include "./gfx/ui.h"
#include "./io/pad.h"
#include "defs.h"

int main(int argc, char** argv) {
  gfx_init();
  pad_init();

  TIM_IMAGE tim;
  gfx_loadFontTexture(&tim);

  while (1) {
    pad_debug();

    ui_renderPlayArea();
    ui_renderTitle(true);
    ui_renderScores(1234, 4321);
    ui_renderControls(true);
    ui_renderKredits();

    // Performs vsync & frameswitch
    gfx_endFrame();
  }

  return 0;
}
