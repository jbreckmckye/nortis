#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include "colours.h"

static const int FIELD_WIDTH = WIDTH * BLOCK_SIZE;
static const int FIELD_HEIGHT = HEIGHT * (BLOCK_SIZE - HIDDEN_ROWS);
// static const int BORDER_WIDTH = 4;

static const int WINDOW_HEIGHT = FIELD_HEIGHT + (2 * SIZE_PADDING);
static const int WINDOW_WIDTH = (3 * FIELD_WIDTH) + (2 * SIZE_PADDING);

static const char* FONT_PATH = "IBMPlexMono-SemiBold.ttf";

static SDL_Window* p_window = NULL;
static SDL_Renderer* p_renderer = NULL;
static TTF_Font* p_font = NULL;

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
  SDL_RenderPresent(p_renderer);
}
