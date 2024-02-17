#include "gfx.h"

#include "ui.h"
#include "../defs.h"

#define TITLE_X 120

static const char* MSG_PLAYNOTRIS = "PLAY NOTRIS!";
static const char* MSG_GAMEOVER   = "GAME OVER!!!";

static const char* MSG_LINES      = "LINES";
static const char* MSG_SCORE      = "SCORE";
static const char* MSG_SCORE_UL   = "-----";

static const char* MSG_CONTROL    = "CONTROL";
static const char* MSG_CONTROL_UL = "-------";
static const char* MSG_CTRL_MOVE  = "*( MOVE";
static const char* MSG_CTRL_ROT   = "O  ROTATE";
static const char* MSG_CTRL_DROP  = "X  DROP";

static const char* MSG_RESTART    = "[START] TO RETRY";

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

void ui_renderTitle(bool isAlive) {
  if (isAlive) {
     gfx_drawFontString(TITLE_X, SIZE_PADDING, MSG_PLAYNOTRIS, 0);
  } else {
     gfx_drawFontString(TITLE_X, SIZE_PADDING, MSG_GAMEOVER, 0);
  }
}
