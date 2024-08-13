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
#define MAIN_TEXT(Y_INDEX, TXT) gfx_drawFontString(TITLE_X, Y_POS(Y_INDEX), TXT, 0)

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

static const char* MSG_START      = ":;<=> PRESS START TO BEGIN :;<=>";

static const char* MSG_KREDITS    = "BUILT 2024 WITH";
static const char* MSG_PSNOOB     = "PSNOOBSDK $";

// Spells out 'notris' when masked & shifted with a 22 bit long mask
static int32_t titleMask = 0b1000000000000000000000;
static int32_t titlePattern[5] = {
  0b1001011101110111010111,
  0b1101010100100101010100,
  0b1011010100100110010111,
  0b1001010100100101010001,
  0b1001011100100101010111
};

/**
 * Private functions
 * ============================================================================
 */

static void renderPlayArea() {
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

static void renderTitle(bool isAlive) {
  if (isAlive) {
     gfx_drawFontString(TITLE_X, SIZE_PADDING, MSG_PLAYNOTRIS, 0);
  } else {
     gfx_drawFontString(TITLE_X, SIZE_PADDING, MSG_GAMEOVER, 0);
  }
  gfx_drawFontString(TITLE_X, Y_POS(1), MSG_DIVIDER, 0);
}

static void renderControls(bool isAlive) {
  MAIN_TEXT(8, MSG_CONTROLS);
  MAIN_TEXT(9, MSG_DIVIDER);

  if (isAlive) {
    MAIN_TEXT(10, MSG_CTRL_MOVE);
    MAIN_TEXT(11, MSG_CTRL_ROT);
    MAIN_TEXT(12, MSG_CTRL_DROP);
  } else {
    MAIN_TEXT(11, MSG_RESTART);
  }
}

static void renderScores(int lines, int score) {
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

static void renderKredits() {
  MAIN_TEXT(16, MSG_KREDITS);
  MAIN_TEXT(17, MSG_PSNOOB);
}

static BlockNames getTitleXColour(int x) {
  return
      x < 4 ? BLOCK_S :
      x < 8 ? BLOCK_L :
      x < 12 ? BLOCK_T :
      x < 16 ? BLOCK_J :
      x < 19 ? BLOCK_I :
      BLOCK_O;
}

/**
 * Public functions
 * ============================================================================
 */

void ui_renderBlock(int u, int v, BlockNames block) {
  XYWH coords = {
    .x = SIZE_PADDING + (u * BLOCK_SIZE),
    .y = SIZE_PADDING + (v * BLOCK_SIZE),
    .w = BLOCK_SIZE,
    .h = BLOCK_SIZE,
  };

  ColourPalette* p_colours = colours_getBlockColours(block);
  gfx_drawBlock(coords, p_colours->main, p_colours->light, p_colours->dark);
}

void ui_render(GameState* p_gameState) {
  bool isAlive = p_gameState->playState == PLAY_PLAYING;
  int score = p_gameState->clearedLines;

  renderPlayArea();
  renderTitle(isAlive);
  renderScores(score, score * 12);
  renderControls(isAlive);
  renderKredits();
}

void ui_renderTitleScreen() {
  static int32_t titleTimer = 0;
  
  // Increments but repeats mid way
  titleTimer++;
  if (titleTimer > 1200) {
    titleTimer = 600;
  }

  // Flashing text comes in
  if (titleTimer > 300 && (titleTimer % 60) < 30) {
    gfx_drawFontString(30, 150, MSG_START, 0);
  }

  // Determines how quickly we move through the animation
  int32_t ticks = titleTimer / 2;

  // Dissolve-in the title blocks
  for (int y = 0; y < 5; y++) {
    for (int x = 0; x < 22; x++) {
      int matrixPosition = (y * 22) + x;
      if (matrixPosition > ticks) {
        break; // because this 'pixel' of the display is not to be displayed yet
      }

      int32_t titleLine = titlePattern[y];
      int32_t bitMask = titleMask >> x;
      if (titleLine & bitMask) { // there is a 'pixel' at this location to show
        BlockNames colour = getTitleXColour(x);
        ui_renderBlock(
          x + 6, // roughly center marquee
          y + 8,
          colour
        );
      }
    }
  }
}
