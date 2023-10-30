#include "SDL2/SDL.h"
#include "../defs.h"

bool gfx_init();
void gfx_cleanup();

void gfx_draw(const DrawField* drawField, const GameState* p_gameState);
