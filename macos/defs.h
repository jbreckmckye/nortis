#include <stdbool.h>

#define WIDTH 10
#define HEIGHT 26
#define BLOCK_SIZE 24
#define HIDDEN_ROWS 2
#define SIZE_PADDING 32
#define SIZE_BORDER 4

#define GRID_BIT_OFFSET 0x8000

typedef enum BlockNames {
  BLOCK_I,
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

typedef enum PlayMovements {
  MOVELEFT,
  MOVERIGHT
} PlayMovements;

enum GameCollisions {
  COLLIDE_NONE,
  COLLIDE_LEFTWALL,
  COLLIDE_RIGHTWALL,
  COLLIDE_BOTTOMWALL,
  COLLIDE_CELL
};

typedef bool Field[HEIGHT][WIDTH];

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

