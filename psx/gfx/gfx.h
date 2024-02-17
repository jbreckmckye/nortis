#pragma once

#include <psxgpu.h>
#include "../defs.h"

/* Initialise PSX graphics systems; set up global memory buffers for graphics primitives */
void gfx_init();

/* Complete work for displaying a frame; switch over buffers in dual-buffer setup; dispatch work to GPU */
void gfx_endFrame();

/* Load the font */
void gfx_loadFontTexture();

/* Temporary: show the font */
void gfx_showFontTexture();

/* Renders text */
void gfx_drawFontString(int x, int y, char* string, int zIndex);

void gfx_drawRect(int x, int y, int w, int h, RGB rgb);
