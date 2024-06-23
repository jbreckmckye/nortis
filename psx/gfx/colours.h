#pragma once

#include <psxgpu.h>

#include "../defs.h"

/**
 * COLOURS.H
 * ############################################################################
 * Colour values & helpers
 */

extern RGB colours_bkg;

extern ColourPalette colours_blue;
extern ColourPalette colours_green;
extern ColourPalette colours_red;
extern ColourPalette colours_orange;
extern ColourPalette colours_purple;
extern ColourPalette colours_turquoise;
extern ColourPalette colours_yellow;

ColourPalette* colours_getBlockColours (BlockNames blockKey);
