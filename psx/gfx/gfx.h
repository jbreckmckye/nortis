#pragma once

#include <psxgpu.h>
#include "../defs.h"

/**
 * GFX.H
 * ############################################################################
 * Functions for graphics primitives, drawing shapes & text
 */

/* Exposed for debugging */
extern RenderContext* gfx_p_ctx;

/* Initialise PSX graphics systems; set up global memory buffers for graphics primitives */
void gfx_init();

/* Complete work for displaying a frame; switch over buffers in dual-buffer setup; dispatch work to GPU */
void gfx_endFrame();

/* Load the font */
void gfx_loadFontTexture();

/* Temporary: show the font */
void gfx_showFontTexture();

/* Renders text */
void gfx_drawFontString(int x, int y, const char* string, int zIndex);

void gfx_drawBlock(XYWH coords, RGB fill, RGB accentLight, RGB accentDark);

/* Creates a frame with 1px borders, with colour gradients running from each corner (NE, SE, SW, NW) */
void gfx_drawCornerColouredBox(XYWH coords, RGB rgbNW, RGB rgbNE, RGB rgbSE, RGB rgbSW);
