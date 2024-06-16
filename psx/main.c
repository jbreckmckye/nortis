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
#include "defs.h"

int main(int argc, char** argv) {
  gfx_init();

  // Code for displaying debug font
  FntLoad(960, 0);
  int printStream = FntOpen(10, 10, 300, 220, 0, 80);
  int framecount = 0;

  TIM_IMAGE tim;
  gfx_loadFontTexture(&tim);

  while (1) {
    //gfx_showFontTexture(&tim);

    ui_renderPlayArea();
    ui_renderTitle(true);
    ui_renderScores(1234, 4321);
    ui_renderControls(true);
    ui_renderKredits();

    framecount++;
    framecount = framecount % 60;
    // Write into debug text printstream & flush into drawing environment
    FntPrint(printStream, "FRAME %d", framecount);
    FntFlush(printStream);

    gfx_endFrame();
  }

  return 0;
}
