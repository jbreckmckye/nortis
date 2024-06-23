#include <assert.h>
#include <psxapi.h>
#include <stdlib.h>

#include "blocks.h"

/**
 * BLOCKS.C
 * ################################################################################################
 * Blocks are stored using a hex number scheme described in https://stackoverflow.com/a/38596291,
 * of 16 bits (4 by 4 rows).  This allows us to very compactly define custom rotation systems.
 * 
 * Bits can be addressed by masking from 0x8000 (GRID_BIT_OFFSET) for the first bit, then shifting
 * right for subsequent bits.
 * 
 * Example:
 * 
 * T block translated into bits
 * 
 * .X.. -> 0100
 * XXX. -> 1110
 * .... -> 0000
 * .... -> 0000
 * 
 * binary= 0b0100111000000000
 * hexdml= 0x4E00
 * 
 * To get first item (x0,y0), we need to AND by
 * binary: 0b1000000000000000  
 * hex:    0b8000
 * 
 * We get subsequent rows by >> right shifting by 4, columns by >> right shifting 1
 * 
 * Therefore to get x,y = BITS & (0x8000 >> ((row * 4) + col))
 *
 */

static ShapeBits shapeHexes[8][4] = {
  { 0 },                              // NONE
  { 0x0F00, 0x4444, 0x0F00, 0x4444 }, // I
  { 0xE200, 0x44C0, 0x8E00, 0xC880 }, // J
  { 0xE800, 0xC440, 0x2E00, 0x88C0 }, // L
  { 0xCC00, 0xCC00, 0xCC00, 0xCC00 }, // O
  { 0x6C00, 0x8C40, 0x6C00, 0x8C40 }, // S
  { 0x0E40, 0x4C40, 0x4E00, 0x4640 }, // T
  { 0x4C80, 0xC600, 0x4C80, 0xC600 }, // Z
};

int blocks_getShapeBit(ShapeBits s, int y, int x) {
  int mask = GRID_BIT_OFFSET >> ((y * 4) + x);
  return s & mask;
}

RotationN blocks_getNextRotation(RotationN r) {
  return (r + 1) % 4;
}

ShapeBits blocks_getBlockShape(BlockNames block, RotationN r) {
  assert(block >= 0 && block < 8);
  assert(r >= 0 && r < 4);

  return shapeHexes[block][r];
}

/**
 * Randomisation
 * ================================================================================================
 */

static int random() {
  // Pseudo-random number based on function timing (first piece is deterministic)
  int rootCount = GetRCnt(0);
  srand(rootCount);
  int result = rand();
}

BlockNames blocks_randomBlock() {
  // Not truly random (has skew) but probably fine for a game
  return (random() % 7) + 1;
}
