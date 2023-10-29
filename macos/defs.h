// Once-only wrapper
#ifndef DEFS_H_SEEN
#define DEFS_H_SEEN

#include <stdbool.h>

#define WIDTH 10
#define HEIGHT 26
#define HIDDEN_ROWS 2
#define DRAW_HEIGHT (HEIGHT - HIDDEN_ROWS)
#define BLOCK_SIZE 24

#define SIZE_PADDING 32
#define SIZE_BORDER 4

#define GRID_BIT_OFFSET 0x8000

typedef enum BlockNames {
  BLOCK_NONE,
  BLOCK_I,
  BLOCK_J,
  BLOCK_L,
  BLOCK_O,
  BLOCK_S,
  BLOCK_T,
  BLOCK_Z
} BlockNames;

typedef enum PlayStates {
  PLAY_PLAYING,
  PLAY_GAMEOVER
} PlayStates;

typedef enum {
  INPUT_NONE = 0,
  INPUT_LEFT,
  INPUT_RIGHT,
  INPUT_UP,
  INPUT_DOWN,
  INPUT_QUIT
} GameInputs;

typedef enum GameMovements {
  MOVE_LEFT,
  MOVE_RIGHT
} GameMovements;

typedef enum GameCollisions {
  COLLIDE_NONE = 0,
  COLLIDE_LEFTWALL,
  COLLIDE_RIGHTWALL,
  COLLIDE_BOTTOMWALL,
  COLLIDE_CELL
} GameCollisions;

typedef BlockNames Field[HEIGHT][WIDTH];
typedef BlockNames DrawField[HEIGHT - HIDDEN_ROWS][WIDTH];

struct GameState {
  BlockNames blockName;
  int blockRotation;
  int clearedLines;
  int dropDelay;
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
