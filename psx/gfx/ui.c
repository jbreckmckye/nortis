#include "gfx.h"
#include <stdio.h>

#include "ui.h"
#include "../defs.h"
#include "colours.h"

/**
 * UI.C
 * ############################################################################
 * High level functions for drawing the play state
 */

#define TITLE_X 120
#define MAX_SCORE 100000
#define Y_POS(N) SIZE_PADDING + (N * FONT_GLYPH_SIZE) + (2 * N)

// Special characters in font:
// $ = happy emoji, % = sad emoji, () = left/right arrow, & = up arrow, ' = down arrow, * = circle, :;<=> = notronimos

static const char* MSG_PLAYNOTRIS = "PLAY NOTRIS! :;<=> $";
static const char* MSG_GAMEOVER   = "GAME OVER! TRY AGAIN";
static const char* MSG_DIVIDER    = "--------------------";

static const char* MSG_LINES      = "LINES";
static const char* MSG_SCORE      = "SCORE";

static const char* MSG_CONTROLS   = "CONTROLS";

static const char* MSG_CTRL_MOVE  = "MOVE   ()";
static const char* MSG_CTRL_ROT   = "ROTATE *";
static const char* MSG_CTRL_DROP  = "DROP   X";

static const char* MSG_RESTART    = "PRESS START";

static const char* MSG_KREDITS    = "BUILT 2024 WITH";
static const char* MSG_PSNOOB     = "PSNOOBSDK $";

void ui_renderPlayArea() {
  RGB squareRed = {0xF5, 0x1B, 0x1B};
  RGB circleOrange = {0xF5, 0xB0, 0x1B};
  RGB triangeGreen = {0x1B, 0xF5, 0x26};
  RGB crossBlue = {0x1B, 0x22, 0xF5};

  XYWH coords = {
    .x = SIZE_PADDING - 1,
    .y = SIZE_PADDING - 1,
    .w = 1 + (WIDTH * BLOCK_SIZE),
    .h = 1 + ((HEIGHT - 2) * BLOCK_SIZE)
  };

  // Draw the play area frame
  // - Removes two rows from the top, these will be invisible (used for handling rotations at top of area)
  // - Frame lines are outside the pieces, not beneath them
  gfx_drawCornerColouredBox(
    coords,
    squareRed,
    circleOrange,
    triangeGreen,
    crossBlue
  );
}

void ui_renderTitle(bool isAlive) {
  if (isAlive) {
     gfx_drawFontString(TITLE_X, SIZE_PADDING, MSG_PLAYNOTRIS, 0);
  } else {
     gfx_drawFontString(TITLE_X, SIZE_PADDING, MSG_GAMEOVER, 0);
  }
  gfx_drawFontString(TITLE_X, Y_POS(1), MSG_DIVIDER, 0);
}

void ui_renderControls(bool isAlive) {
  int y1 = Y_POS(8);
  int y2 = Y_POS(9);
  int y3 = Y_POS(10);
  int y4 = Y_POS(11);
  int y5 = Y_POS(12);

  gfx_drawFontString(TITLE_X, y1, MSG_CONTROLS, 0);
  gfx_drawFontString(TITLE_X, y2, MSG_DIVIDER, 0);

  if (isAlive) {
    gfx_drawFontString(TITLE_X, y3, MSG_CTRL_MOVE, 0);
    gfx_drawFontString(TITLE_X, y4, MSG_CTRL_ROT, 0);
    gfx_drawFontString(TITLE_X, y5, MSG_CTRL_DROP, 0);
  } else {
    gfx_drawFontString(TITLE_X, y4, MSG_RESTART, 0);
  }
}

void ui_renderScores(int score, int lines) {
  int y1 = Y_POS(3);
  int y2 = Y_POS(4);
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

void ui_renderKredits() {
  int y1 = Y_POS(16);
  int y2 = Y_POS(17);

  gfx_drawFontString(TITLE_X, y1, MSG_KREDITS, 0);
  gfx_drawFontString(TITLE_X, y2, MSG_PSNOOB, 0);
}

void ui_renderBlock(int u, int v) {
  int y = SIZE_PADDING + ((v - 2) * BLOCK_SIZE);
  int x = SIZE_PADDING + (u * BLOCK_SIZE);

  XYWH coords = {
    .x = SIZE_PADDING + (u * BLOCK_SIZE),
    .y = SIZE_PADDING + ((v - 2) * BLOCK_SIZE),
    .w = BLOCK_SIZE,
    .h = BLOCK_SIZE,
  };

  ColourPalette* p_colours = colours_getBlockColours(BLOCK_T);
  gfx_drawBlock(coords, p_colours->main, p_colours->light, p_colours->dark);
}
