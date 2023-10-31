#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <stdio.h>
#include <assert.h> // might remove these as we import more into main

#include "colours.h"

/*
 * Constants
 * ============================================================================ 
 */

static const int FIELD_WIDTH = WIDTH * BLOCK_SIZE;
static const int FIELD_HEIGHT = HEIGHT * (BLOCK_SIZE - HIDDEN_ROWS);

static const int WINDOW_HEIGHT = FIELD_HEIGHT + (2 * SIZE_PADDING);
static const int WINDOW_WIDTH = (2 * FIELD_WIDTH) + (2 * SIZE_PADDING);

static const char* FONT_PATH = "IBMPlexMono-SemiBold.ttf";

static const char* TITLE = "Play NOTRIS!";
static const char* TITLE_GAMEOVER = "Game over!";
static const int TITLE_S = 24;
static const int TITLE_X = (1.5 * FIELD_WIDTH) - 32;
static const int TITLE_Y = SIZE_PADDING + 8;

static const int SCORE_S = 18;
static const char* SCORE_TEXT = "CLEARED";
static const int SCORE_X = SIZE_PADDING + FIELD_WIDTH + 20;
static const int SCORE_Y = TITLE_Y + 66;
static const int SCORE_Y2 = SCORE_Y + 24;

static const char* CONTROLS = "Use WASD to play";
static const char* CONTROLS_GAMEOVER = "Press R to restart";
static const int CONTROLS_Y = SCORE_Y2 + 66;

/*
 * Shared references
 * ============================================================================ 
 */

static SDL_Window* p_window = NULL;
static SDL_Renderer* p_renderer = NULL;
static TTF_Font* p_font = NULL;

/*
 * Initialisation
 * ============================================================================ 
 */

static bool initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL_Init failed, error: %s\n", SDL_GetError());
    return false;
  }

  p_window = SDL_CreateWindow(
    "Notris", // title
    SDL_WINDOWPOS_UNDEFINED, // x
    SDL_WINDOWPOS_UNDEFINED, // y
    WINDOW_WIDTH, // w
    WINDOW_HEIGHT, // h
    SDL_WINDOW_SHOWN // visibility
  );

  if (p_window == NULL) {
    printf("SDL_CreateWindow failed, error: %s\n", SDL_GetError());
    return false;
  }

  p_renderer = SDL_CreateRenderer(p_window, -1, SDL_RENDERER_ACCELERATED);
  if (p_renderer == NULL) {
    printf("SDL_CreateRenderer failed, error: %s\n", SDL_GetError());
    return false;
  }

  return true;
}

static bool initFont() {
  if (TTF_Init() < 0) {
    printf("TTF_Init failed, error: %s\n", SDL_GetError());
    return false;
  }

  p_font = TTF_OpenFont(FONT_PATH, 24);
  if (p_font == NULL) {
    printf("TTF_OpenFont failed, error: %s\n", SDL_GetError());
    return false;
  }

  TTF_SetFontHinting(p_font, TTF_HINTING_NORMAL);

  return true;
}

/*
 * Drawing
 * ============================================================================ 
 */

static void drawBorder(SDL_Color *p_colour, int x1, int y1, int x2, int y2, int thickness, BorderFlags borders) {
  SDL_SetRenderDrawColor(
    p_renderer,
    p_colour->r,
    p_colour->g,
    p_colour->b,
    p_colour->a
  );

  bool isOuter = thickness < 0;
  int size = isOuter ? (thickness * -1) : thickness;

  int boxWidth = x2 - x1;
  int boxHeight = y2 - y1;

  // Horizontal line - top
  SDL_Rect hline = { // x,y,w,h
    isOuter ? x1 - size : x1,
    isOuter ? y1 - size : y1,
    isOuter ? boxWidth + (size * 2) : boxWidth,
    size
  };
  if (borders & BORDER_TOP) SDL_RenderFillRect(p_renderer, &hline);

  // Horizontal line - bottom
  hline.y = isOuter ? y2 : (y2 - size);
  if (borders & BORDER_BOTTOM) SDL_RenderFillRect(p_renderer, &hline);

  // Vertical line - left
  SDL_Rect vline = { //x,y,w,h
    isOuter ? x1 - size : x1,
    isOuter ? y1 - size : y1,
    size,
    isOuter ? boxHeight + (size * 2) : boxHeight
  };
  if (borders & BORDER_LEFT) SDL_RenderFillRect(p_renderer, &vline);

  // Vertical line - right
  vline.x = isOuter ? x2 : (x2 - size);
  if (borders & BORDER_RIGHT) SDL_RenderFillRect(p_renderer, &vline);
}

static void drawPlayPiece(ColourPalette *p_palette, int posX, int posY) {
  const int borderWidth = 2;

  SDL_SetRenderDrawColor(
    p_renderer,
    p_palette->main.r,
    p_palette->main.g,
    p_palette->main.b,
    p_palette->main.a
  );

  SDL_Rect block = {
    .x = SIZE_PADDING + (posX * BLOCK_SIZE),
    .y = SIZE_PADDING + (posY * BLOCK_SIZE),
    .h = BLOCK_SIZE,
    .w = BLOCK_SIZE
  };

  SDL_RenderFillRect(p_renderer, &block);

  drawBorder(
    &(p_palette->dark),
    block.x,
    block.y,
    block.x + block.w,
    block.y + block.h,
    borderWidth,
    BORDER_BOTTOM + BORDER_RIGHT
  );

  drawBorder(
    &(p_palette->light),
    block.x,
    block.y,
    block.x + block.w,
    block.y + block.h,
    borderWidth,
    BORDER_TOP + BORDER_LEFT
  );
}

static void drawPlayBox() {
  drawBorder(
    &colours_offWhite,
    SIZE_PADDING,
    SIZE_PADDING,
    SIZE_PADDING + FIELD_WIDTH,
    SIZE_PADDING + FIELD_HEIGHT + SIZE_BORDER,
    SIZE_BORDER * -1,
    BORDER_ALL
  );
}

static void drawText(const char* text, int x, int y, int size, SDL_Color colour) {
  int fontSizeSet = TTF_SetFontSize(p_font, size);
  assert(fontSizeSet == 0);

  SDL_Surface *p_surface = TTF_RenderUTF8_Blended(p_font, text, colour);
  SDL_Texture *p_texture = SDL_CreateTextureFromSurface(p_renderer, p_surface);

  SDL_Rect rect = { x, y, .w = p_surface->w, .h = p_surface->h };
  SDL_RenderCopy(p_renderer, p_texture, NULL, &rect);

  SDL_DestroyTexture(p_texture);
  SDL_FreeSurface(p_surface);
}

/**
 * PSX NOTE
 * We'll probably want a more fixed-memory, simplified approach for PSX here,
 * due to its buggy malloc()
 */
char* newIntString(int number) {
  double tens = number == 0 ? 1 : ceil(log10(number)) + 1;
  char* p_string = malloc(tens * sizeof(char));
  assert(p_string);
  sprintf(p_string, "%d", number);
  return p_string;
}

void freeIntString(char* p_string) {
  free(p_string);
}

/*
 * Public
 * ============================================================================ 
 */

bool gfx_init() {
  return initSDL() && initFont();
}

void gfx_cleanup() {
  SDL_DestroyRenderer(p_renderer);
  p_renderer = NULL;

  SDL_DestroyWindow(p_window);
  p_window = NULL;

  TTF_CloseFont(p_font);
  TTF_Quit();

  SDL_Quit();
}

void gfx_draw(const DrawField* p_drawField, const GameState* p_gameState) {
  // Clear the backbuffer
  SDL_RenderClear(p_renderer);

  // Black background
  SDL_Color bkg = colours_black;
  SDL_SetRenderDrawColor(p_renderer, bkg.r, bkg.g, bkg.b, bkg.a);
  SDL_RenderFillRect(p_renderer, NULL);

  // Play box and play pieces

  drawPlayBox();

  for (int y = 0; y < DRAW_HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      int blockKey = (*p_drawField)[y][x];
      if (blockKey) {
        ColourPalette *p_palette = colours_blockPalette(blockKey);
        drawPlayPiece(p_palette, x, y);
      }
    }
  }

  /**
   * UI layout:
   * ----------
   * 
   * # TITLE / GAME OVER
   * 
   * ## Cleared:
   * ## 123
   * 
   * ## Controls / Restart game
   */
  char* p_clearedLines = newIntString(p_gameState->clearedLines);

  if (p_gameState->playState == PLAY_PLAYING) {
    drawText(TITLE, TITLE_X, TITLE_Y, TITLE_S, colours_offWhite);
    drawText(SCORE_TEXT, SCORE_X, SCORE_Y, SCORE_S, colours_offWhite);
    drawText(p_clearedLines, SCORE_X, SCORE_Y2, SCORE_S, colours_offWhite);
    drawText(CONTROLS, SCORE_X, CONTROLS_Y, SCORE_S, colours_offWhite);

  } else {
    drawText(TITLE_GAMEOVER, TITLE_X, TITLE_Y, TITLE_S, colours_red.dark);
    drawText(SCORE_TEXT, SCORE_X, SCORE_Y, SCORE_S, colours_red.dark);
    drawText(p_clearedLines, SCORE_X, SCORE_Y2, SCORE_S, colours_red.dark);
    drawText(CONTROLS_GAMEOVER, SCORE_X, CONTROLS_Y, SCORE_S, colours_offWhite);
  }

  freeIntString(p_clearedLines);
  p_clearedLines = NULL;

  // Flip back buffer
  SDL_RenderPresent(p_renderer);
}
