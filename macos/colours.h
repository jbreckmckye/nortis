#include "SDL2/SDL.h"
#include "defs.h"

struct ColourPalette {
  SDL_Color main;
  SDL_Color light;
  SDL_Color dark;
} typedef ColourPalette;

SDL_Color colours_black;
SDL_Color colours_offWhite;

ColourPalette colours_blue;
ColourPalette colours_green;
ColourPalette colours_red;
ColourPalette colours_orange;
ColourPalette colours_purple;
ColourPalette colours_turquoise;
ColourPalette colours_yellow;

ColourPalette* colours_blockPalette (BlockNames blockKey);
