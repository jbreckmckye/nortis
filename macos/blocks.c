#include <stdlib.h>
#include <assert.h>

#include "blocks.h"

/**
 * blocks.c
 * ================================================================================================
 * Blocks are stored using a hex number scheme described in https://stackoverflow.com/a/38596291,
 * of 16 bits (4 by 4 rows).  This allows us to very compactly define custom rotation systems.
 * 
 * Bits can be addressed by masking from 0x8000 (GRID_BIT_OFFSET) for the first bit, then shifting
 * right for subsequent bits.
 * 
 * Example:
 * 
 * A 2x2 block translated into bits
 * 
 * XX.. -> 1100
 * XX.. -> 1100
 * .... -> 0000
 * .... -> 0000
 * 
 * binary= 0b1100110000000000
 * hexdml= 0xCC00
 * 
 * To get row 1 = 0xCC00 & (0x0F00)
 *        row 2 = 0xCC00 & (0x0F00 >> 1)
 *        etc.
 * 
 * Or to get x/y = 0xCC00 & (0x8000 >> ((y * 4) + x))
 * ================================================================================================
 */

static shapeHex shapeHexes[8][4] = {
  { 0 },                              // EMPTY   
  { 0x0F00, 0x4444, 0x0F00, 0x4444 }, // I
  { 0xE200, 0x44C0, 0x8E00, 0xC880 }, // J
  { 0xE800, 0xC440, 0x2E00, 0x88C0 }, // L
  { 0xCC00, 0xCC00, 0xCC00, 0xCC00 }, // O
  { 0x6C00, 0x8C40, 0x6C00, 0x8C40 }, // S
  { 0x0E40, 0x4C40, 0x4E00, 0x4640 }, // T
  { 0x4C80, 0xC600, 0x4C80, 0xC600 }, // Z
};

int getShapeBit(shapeHex s, int y, int x) {
  int mask = GRID_BIT_OFFSET >> ((y * 4) + x);
  return s & mask;
}

rotationIndex getNextRotation(int r) {
  return (r + 1) % 4;
}

shapeHex getBlockShape(BlockNames key, rotationIndex r) {
  assert(key >= 0 && key < 8);
  assert(r >= 0 && r < 4);

  return shapeHexes[key][r];
}

/**
 * Randomisation
 * ================================================================================================
 */

BlockNames randomBlock() {
  // Not truly random (has skew) but probably fine for a game
  return (rand() % 7) + 1;
}
