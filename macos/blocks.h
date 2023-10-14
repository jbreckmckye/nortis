#include "defs.h"

typedef int shapeHex;
typedef int rotationIndex;

int getShapeBit(shapeHex s, int y, int x);

rotationIndex getNextRotation(int r);

shapeHex getBlockShape(BlockNames key, rotationIndex r);

BlockNames randomBlock();
