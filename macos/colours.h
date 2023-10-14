#include "SDL2/SDL.h"
#include "defs.h"

struct ColourPalette {
  SDL_Color main;
  SDL_Color light;
  SDL_Color dark;
} typedef ColourPalette;

SDL_Color black;

ColourPalette paletteBlue;
ColourPalette paletteGreen;
ColourPalette paletteRed;
ColourPalette paletteOrange;
ColourPalette palettePurple;
ColourPalette paletteTurquoise;
ColourPalette paletteYellow;

SDL_Color blockColourMain (BlockNames block);
