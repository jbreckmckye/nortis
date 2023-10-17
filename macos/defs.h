// Once-only wrapper
#ifndef DEFS_H_SEEN
#define DEFS_H_SEEN

#include <stdbool.h>

#define WIDTH 10
#define HEIGHT 26
#define BLOCK_SIZE 24
#define HIDDEN_ROWS 2
#define SIZE_PADDING 32
#define SIZE_BORDER 4

#define GRID_BIT_OFFSET 0x8000

typedef enum BlockNames {
  BLOCK_I = 1,
  BLOCK_J,
  BLOCK_L,
  BLOCK_O,
  BLOCK_S,
  BLOCK_T,
  BLOCK_Z
} BlockNames;

typedef enum PlayStates {
  PLAYING,
  GAMEOVER
} PlayStates;

typedef enum GameMovements {
  MOVELEFT,
  MOVERIGHT
} GameMovements;

enum GameCollisions {
  COLLIDE_NONE,
  COLLIDE_LEFTWALL,
  COLLIDE_RIGHTWALL,
  COLLIDE_BOTTOMWALL,
  COLLIDE_CELL
};

typedef int Field[HEIGHT][WIDTH];

struct GameState {
  BlockNames blockName;
  int blockRotation;
  int clearedLines;
  int dropDelay;
  Field *p_field;
  int points;
  int positionX;
  int positionY;
  PlayStates playState;
} typedef GameState;

typedef enum BorderFlags {
  BORDER_TOP = 1,
  BORDER_LEFT = 2,
  BORDER_BOTTOM = 4,
  BORDER_RIGHT = 8,
  BORDER_ALL = 15
} BorderFlags;

// Once-only wrapper
#endif // DEFS_H_SEEN
