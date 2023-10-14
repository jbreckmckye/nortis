#define WIDTH 10
#define HEIGHT 26
#define HIDDEN_ROWS 2

#define GRID_BIT_OFFSET 0x8000

// These are coupled so that each BlockName -> BlockColour
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
  BLOCK_I = 1, // -> ORANGE
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
