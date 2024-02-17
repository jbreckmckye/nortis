#include "gfx.h"

#include "ui.h"
#include "../defs.h"

void ui_renderPlayArea() {
  RGB rgb = {0x0, 0x0, 0x0};
  gfx_drawRect(
    SIZE_PADDING,
    SIZE_PADDING,
    WIDTH * BLOCK_SIZE,
    (HEIGHT - 2) * BLOCK_SIZE,
    rgb
  );
}
