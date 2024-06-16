#include "gfx.h"
#include <stdio.h>

#include "ui.h"
#include "../defs.h"

#define TITLE_X 120
#define MAX_SCORE 100000

// Special characters in font:
// $ = happy emoji, % = sad emoji, () = left/right arrow, & = up arrow, ' = down arrow

static const char* MSG_PLAYNOTRIS = "PLAY NOTRIS!";
static const char* MSG_GAMEOVER   = "GAME OVER!!!";

static const char* MSG_LINES      = "LINES";
static const char* MSG_SCORE      = "SCORE";

static const char* MSG_CTRL_MOVE  = "MOVE   ()";
static const char* MSG_CTRL_ROT   = "ROTATE O";
static const char* MSG_CTRL_DROP  = "DROP   X";

static const char* MSG_RESTART    = "PRESS START";

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

void ui_renderControls(bool isAlive) {
  int y1 = SIZE_PADDING + (6 * FONT_GLYPH_SIZE);
  int y2 = SIZE_PADDING + (7 * FONT_GLYPH_SIZE);
  int y3 = SIZE_PADDING + (8 * FONT_GLYPH_SIZE);

  if (isAlive) {
    gfx_drawFontString(TITLE_X, y1, MSG_CTRL_MOVE, 0);
    gfx_drawFontString(TITLE_X, y2, MSG_CTRL_ROT, 0);
    gfx_drawFontString(TITLE_X, y3, MSG_CTRL_DROP, 0);
  } else {
    gfx_drawFontString(TITLE_X, y2, MSG_RESTART, 0);
  }
}

void ui_renderScores(int score, int lines) {
  int y1 = SIZE_PADDING + (2 * FONT_GLYPH_SIZE);
  int y2 = SIZE_PADDING + (3 * FONT_GLYPH_SIZE);
  int x2 = TITLE_X + (FONT_GLYPH_SIZE * 6);

  gfx_drawFontString(TITLE_X, y1, MSG_SCORE, 0);
  gfx_drawFontString(TITLE_X, y2, MSG_LINES, 0);

  char scoreText[10] = "";
  char linesText[10] = "";

  // Values should wrap at 99999
  while (score >= MAX_SCORE) {
    score -= MAX_SCORE;
  }
  while (lines >= MAX_SCORE) {
    lines -= MAX_SCORE;
  }

  sprintf(scoreText, "%d", score);
  gfx_drawFontString(x2, y1, scoreText, 0);

  sprintf(linesText, "%d", lines);
  gfx_drawFontString(x2, y2, linesText, 0);
}
