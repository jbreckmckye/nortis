#include <stdbool.h>

#include "../defs.h"

/**
 * UI.H
 * ############################################################################
 * High level functions for drawing the play state
 */

void ui_render(GameState* p_gameState);

void ui_renderBlock(int u, int v, BlockNames block);

void ui_renderTitle();
