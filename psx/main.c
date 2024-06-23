/**
 * ############################################################################
 * #       NOTRIS - THE FAMILIAR BUT LEGALLY DISTINCT BLOCK PUZZLE GAME       #
 * ############################################################################
 */

#include <psxgpu.h>
#include <stdio.h>
#include <stdbool.h>

#include "game/pad.h"
#include "gfx/gfx.h"
#include "gfx/ui.h"
#include "defs.h"

int main(int argc, char** argv) {
  gfx_init();
  pad_init();

  TIM_IMAGE tim;
  gfx_loadFontTexture(&tim);

  int vsyncs = 0;
  int timeTarget = 120;

  while (1) {
    pad_debug();

    // These could all be combined into a render frame fn
    ui_renderPlayArea();
    ui_renderTitle(true);
    ui_renderScores(5678, 9012);
    ui_renderControls(true);
    ui_renderKredits();

    ui_renderBlock(3, 3);
    ui_renderBlock(2, 4);
    ui_renderBlock(3, 4);
    ui_renderBlock(4, 4);

    // Performs vsync & frameswitch
    gfx_endFrame();

    // Test timer code
    vsyncs++;
    if (vsyncs >= timeTarget) {
      vsyncs = 0;
      printf("Reached time target\n");
    }
  }

  return 0;
}
