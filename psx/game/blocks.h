#include "../defs.h"

/**
 * BLOCKS.H
 * ################################################################################################
 * Get play blocks, access shape data
 */

int blocks_getShapeBit(ShapeBits s, int y, int x);

RotationN blocks_getNextRotation(RotationN r);

ShapeBits blocks_getBlockShape(BlockNames block, RotationN r);

BlockNames blocks_randomBlock();
