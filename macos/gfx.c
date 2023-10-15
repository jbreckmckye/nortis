#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

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

static void drawFrame() {
  SDL_SetRenderDrawColor(
    p_renderer,
    colours_offWhite.r,
    colours_offWhite.g,
    colours_offWhite.b,
    colours_offWhite.a
  );

  int hlineWidth = FIELD_WIDTH + (2 * SIZE_BORDER);
  int vlineHeight = FIELD_HEIGHT + (2 * SIZE_BORDER);
  int fieldBottom = SIZE_PADDING + SIZE_BORDER + FIELD_HEIGHT;
  int fieldRight = SIZE_PADDING + SIZE_BORDER + FIELD_WIDTH;

  SDL_Rect topLine = {
    SIZE_PADDING, SIZE_PADDING, // x/y
    hlineWidth,   SIZE_BORDER,  // w/h
  };
  SDL_RenderFillRect(p_renderer, &topLine);

  SDL_Rect leftLine = {
    SIZE_PADDING, SIZE_PADDING, // x/y
    SIZE_BORDER,  vlineHeight   // w/h
  };
  SDL_RenderFillRect(p_renderer, &leftLine);

  SDL_Rect bottomLine = {
    SIZE_PADDING, fieldBottom, // x/y
    hlineWidth,   SIZE_BORDER  // w/h
  };
  SDL_RenderFillRect(p_renderer, &bottomLine);

  SDL_Rect rightLine = {
    fieldRight,  SIZE_PADDING, // x/y
    SIZE_BORDER, vlineHeight   // w/h
  };
  SDL_RenderFillRect(p_renderer, &rightLine);
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

void gfx_drawDebug() {
  SDL_Color bkg = colours_blue.dark;
  SDL_SetRenderDrawColor(p_renderer, bkg.r, bkg.g, bkg.b, bkg.a);
  SDL_RenderFillRect(p_renderer, NULL);
  drawFrame();
  SDL_RenderPresent(p_renderer);
}
