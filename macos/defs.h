#define WIDTH 10
#define HEIGHT 26
#define HIDDEN_ROWS 2

enum Game_Colours {
  NIL,
  BLUE,
  GREEN,
  RED,
  YELLOW,
  PURPLE,
  AQUA,
  ORANGE 
};

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

struct Game_Piece {
  PlayBlock block;
  int rotation;
  int x;
  int y;
} typedef Game_Piece;

