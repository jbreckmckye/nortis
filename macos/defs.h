#define WIDTH 10
#define HEIGHT 26
#define HIDDEN_ROWS 2

#define GRID_BIT_OFFSET 0x8000

// This MUST map to NIL + an item per BlockName
typedef enum BlockColours {
  NIL,
  ORANGE,
  GREEN,
  PURPLE,
  YELLOW,
  RED,
  BLUE,
  AQUA,
} BlockColours;

typedef enum BlockNames {
  BLOCK_I,
  BLOCK_J,
  BLOCK_L,
  BLOCK_O,
  BLOCK_S,
  BLOCK_T,
  BLOCK_Z
} BlockNames;

enum Game_States {
  PLAYING,
  GAMEOVER
};

enum Game_Movements {
  MOVELEFT,
  MOVERIGHT
};

enum Game_Collisions {
  COLLIDE_NONE,
  COLLIDE_LEFTWALL,
  COLLIDE_RIGHTWALL,
  COLLIDE_BOTTOMWALL,
  COLLIDE_CELL
};
