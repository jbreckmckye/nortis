#pragma once

#include <stdint.h>
#include <psxgpu.h>

#define OT_SIZE 16
#define PACKETS_SIZE 20480
#define SCREEN_W 320
#define SCREEN_H 240
#define FONT_ROW_LENGTH 16
#define FONT_GLYPH_SIZE 8
#define BLOCK_SIZE 8
#define WIDTH 10
#define HEIGHT 26
#define HIDDEN_ROWS 2
#define DRAW_HEIGHT (HEIGHT - HIDDEN_ROWS)
#define SIZE_PADDING 20
#define GRID_BIT_OFFSET 0x8000
#define TITLE_LINE_BITS 22

// Shim for max/min, just don't use with assignments like i++,j++
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

/* The RenderBuffer contains multiple buffers associated with graphics:
 * - The display and draw environments
 * - The primitives ordering table (which is processed end-to-start)
 * - The primitives / commands buffer (which is cleared every frame)
 */
typedef struct {
  DISPENV displayEnv;
  DRAWENV drawEnv;
  uint32_t orderingTable[OT_SIZE];
  uint8_t primitivesBuffer[PACKETS_SIZE];
} RenderBuffer;

/* The RenderContext is a global handle on all the graphics buffers.
 * There are two RenderBuffers to handle dual-buffer rendering; access the current using buffer_id
 * The p_primitive points to the next unused byte in the current primitives buffer
 */
typedef struct {
  int bufferID;
  uint8_t* p_primitive; // next primitive
  RenderBuffer buffers[2];
} RenderContext;

typedef struct {
  int16_t x;
  int16_t y;
} Vector2;

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} RGB;

typedef struct {
  RGB main;
  RGB light;
  RGB dark;
} ColourPalette;

typedef struct {
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
} XYWH;

typedef enum BlockNames {
  BLOCK_NONE = 0,
  BLOCK_I,
  BLOCK_J,
  BLOCK_L,
  BLOCK_O,
  BLOCK_S,
  BLOCK_T,
  BLOCK_Z
} BlockNames;

typedef int16_t ShapeBits;
typedef int RotationN;

typedef enum PlayStates {
  PLAY_PLAYING,
  PLAY_GAMEOVER
} PlayStates;

typedef enum GameInputs {
  INPUT_NONE = 0,
  INPUT_LEFT,
  INPUT_RIGHT,
  INPUT_ROTATE,
  INPUT_DROP,
  INPUT_RESTART,
} GameInputs;

typedef enum GameMovements {
  MOVE_LEFT = -1,
  MOVE_RIGHT = 1,
} GameMovements;

typedef enum GameCollisions {
  COLLIDE_NONE = 0,
  COLLIDE_LEFTWALL,
  COLLIDE_RIGHTWALL,
  COLLIDE_BOTTOMWALL,
  COLLIDE_CELL
} GameCollisions;

// Full field of 'settled' squares. Two hidden rows at the top 'absorb' rotations of items just spawned in
typedef BlockNames Field[HEIGHT][WIDTH];

// Field plus active piece, but minus hidden rows
typedef BlockNames DrawField[HEIGHT - HIDDEN_ROWS][WIDTH];

struct GameState {
  BlockNames blockName;
  int blockRotation;
  int clearedLines;
  int points;
  int positionX;
  int positionY;
  PlayStates playState;
} typedef GameState;

