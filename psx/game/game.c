#include <stdbool.h>
#include <assert.h>

#include "../defs.h"
#include "blocks.h"

/**
 * GAME.C
 * ############################################################################
 * Provides function for executing the game.
 * Expects to receive events via *action functions.
 * Exposes DrawField (graphics state) and GameState (game state) readonly ptrs
 */

/**
 * Globals
 * ============================================================================
 */

static Field g_field = { 0 };
static DrawField g_drawField = { 0 };
static GameState g_gameState = { 0 };

/**
 * Private functions
 * ============================================================================
 * - Only 'mutation' functions should alter the shared state
 * -
 */

static ShapeBits getCurrentShape() {
  return blocks_getBlockShape(
    g_gameState.blockName,
    g_gameState.blockRotation
  );
}

/**
 * Get drop/spawn collisions for given shape and x/y values
 */
static GameCollisions getDropCollision(ShapeBits shape, int x, int y) {
  // Scan bottom-top left-to-right  
  for (int row = 3; row >= 0; row--) {
    for (int col = 0; col <= 3; col++) {
      int bit = blocks_getShapeBit(shape, row, col);
      // Is there something in the shape to collide with? Check if it would overlap anything
      if (bit) {
        int projectedY = y + row;
        int projectedX = x + col;

        // Check out of bounds
        if (projectedY >= HEIGHT) return COLLIDE_BOTTOMWALL;
          
        // Check overlap
        if (g_field[projectedY][projectedX]) return COLLIDE_CELL;
      }
    }
  }
  return COLLIDE_NONE;
}

/**
 * Get all collisions for for given shape and x/y values.
 * This covers more cases than getDropCollision() but takes more steps, so it's more 
 * for validating rotations
 */
static GameCollisions getCollisions(ShapeBits shape, int x, int y) {
  for (int row = 0; row <= 3; row++) {
    for (int col = 0; col <= 3; col++) {
      int bit = blocks_getShapeBit(shape, row, col);
      // Is there something in the shape to collide with? Check if it would overlap anything
      if (bit) {
        int projectedY = y + row;
        int projectedX = x + col;

        // Check out of bounds
        if (projectedX < 0) return COLLIDE_LEFTWALL;
        if (projectedX >= WIDTH) return COLLIDE_RIGHTWALL;
        if (projectedY >= HEIGHT) return COLLIDE_BOTTOMWALL;

        // Check overlap
        if (g_field[projectedY][projectedX]) return COLLIDE_CELL;
      }
    }
  }
  return COLLIDE_NONE;
}

/**
 * Clear the field grid
 */
static void mutateField_clear() {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      g_field[y][x] = 0;
    }
  }
}

/**
 * Insert a block (shape + colours) into the field of static bricks
 */
static void mutateField_insertBlock(BlockNames blockType, ShapeBits shape, int x, int y) {
  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4; row++) {
      int bit = blocks_getShapeBit(shape, col, row);
      if (bit) {
        int projectedX = x + row;
        int projectedY = y + col;
        g_field[projectedY][projectedX] = blockType;
      }
    }
  }
}

/**
 * Update state by spawning a new block
 */
static GameCollisions mutateState_spawn() {
  g_gameState.blockName = blocks_randomBlock();
  g_gameState.blockRotation = 0;
  g_gameState.positionX = 4;
  g_gameState.positionY = 0;

  // Shunt initial position based on block type
  switch (g_gameState.blockName) {
    case BLOCK_I:
      // Spawns horizontally, just above visible area
      g_gameState.positionX = 3;
      g_gameState.positionY = 1;
      break;
    case BLOCK_T:
      // Spawns in T shape, with bottom half visible
      g_gameState.positionY = 1;
      break;
    case BLOCK_J:
    case BLOCK_L:
    case BLOCK_O:
    case BLOCK_S:
    case BLOCK_Z:
      g_gameState.positionY = 2;
      break;
    default:
      // Should never happen
      assert(g_gameState.blockName != 0);
  }

  ShapeBits shape = getCurrentShape();
  // Does this 'drop' (spawning) create a collision? Triggers game over if so
  return getDropCollision(shape, g_gameState.positionX, g_gameState.positionY);
}

static void mutateState_resetGame() {
  g_gameState.clearedLines = 0;
  g_gameState.points = 0;
  g_gameState.playState = PLAY_PLAYING;
  mutateState_spawn();
}

static void mutateState_gameOver() {
  g_gameState.playState = PLAY_GAMEOVER;
}

static void mutateState_setRotation(int rotation) {
  g_gameState.blockRotation = rotation;
}

static void mutateState_setX(int nextX) {
  g_gameState.positionX = nextX;
}

static void mutateState_setY(int nextY) {
  g_gameState.positionY = nextY;
}

/**
 * Attempts to move the piece down 1 space
 * Success - updates positionY
 * Fail    - returns collision
 */
static GameCollisions downOne() {
  int nextX = g_gameState.positionX;
  int nextY = g_gameState.positionY + 1;
  ShapeBits shape = getCurrentShape();

  GameCollisions collision = getDropCollision(shape, nextX, nextY);
  if (collision == COLLIDE_NONE) {
    mutateState_setY(nextY);
  }

  return collision;
}

/**
 * Move the piece down as many spaces as possible
 */
static void downMany() {
  GameCollisions collision = COLLIDE_NONE;
  while (collision == COLLIDE_NONE) {
    collision = downOne();
  }
}

/**
 * Is line at y full?
 */
static bool isLineComplete(int y) {
  BlockNames* line = g_field[y];
  for (int x = 0; x < WIDTH; x++) {
    if (!line[x]) return false;
  }
  return true;
}

/**
 * Clear the line at y, drop the lines above
 */
static void mutateField_clearLine(int row) {
  // Copy from lines above, except top line
  for (int y = row; y > 0; y--) {
    BlockNames* line = g_field[y];
    BlockNames* lineAbove = g_field[y - 1];
    for (int x = 0; x < WIDTH; x++) {
      line[x] = lineAbove[x];
    }
  }
  // Refresh top line
  for (int x = 0; x < WIDTH; x++) {
    g_field[0][x] = BLOCK_NONE;
  }
}

/**
 * Clear as many lines as possible from the field (e.g. after piece settled)
 */
static int mutateField_clearLines() {
  int cleared = 0;
  for (int y = 0; y < HEIGHT; y++) {
    bool shouldClear = isLineComplete(y);
    if (shouldClear) {
      cleared++;
      mutateField_clearLine(y);
    }
  }
  return cleared;
}

/**
 * Piece has come to a stop; insert into field, check lines, respawn, check game over condition
 */
static void mutate_commitPiece() {
  // Insert landed piece
  mutateField_insertBlock(
    g_gameState.blockName,
    getCurrentShape(),
    g_gameState.positionX,
    g_gameState.positionY
  );

  // Clear lines
  int cleared = mutateField_clearLines();

  // Update score
  if (cleared) {
    g_gameState.clearedLines += cleared;
  }

  // Respawn, check game over
  GameCollisions spawnCollision = mutateState_spawn();
  if (spawnCollision) {
    mutateState_gameOver();
  }
}

/**
 * Public values (see header)
 * ============================================================================
 */

DrawField* game_p_drawField = &g_drawField;
GameState* game_p_gameState = &g_gameState;

/**
 * Public functions
 * ============================================================================
 * - 'Actions' are events from outside the game engine, that trigger changes
 */

void game_actionRestart() {
  mutateField_clear();
  mutateState_resetGame();
}

/**
 * Is the interval between pieces falling, measured in frames
 */
int32_t game_getSpeed() {
  int setsCleared = g_gameState.clearedLines / 5;
  int level = MAX(MIN(1, setsCleared), 10);
  return 60 - (level * 3);
}

/**
 * Copies settled pieces and active piece into the DrawField. Call this just before rendering.
 */
void game_updateDrawState() {
  for (int y = 0; y < DRAW_HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      // Transpose from field, ignoring the topmost two hidden rows
      g_drawField[y][x] = g_field[y + HIDDEN_ROWS][x];
    }
  }

  ShapeBits shape = getCurrentShape();
  BlockNames block = g_gameState.blockName;

  for (int y = 0; y <= 3; y++) {
    for (int x = 0; x <= 3; x++) {
      int bit = blocks_getShapeBit(shape, y, x);
      // Skip if empty bit
      if (bit == 0) continue;

      // Get projections, bound to field limits (else we will overflow the arrays!)
      int fieldY = g_gameState.positionY + y;
      int fieldX = g_gameState.positionX + x;

      if (fieldY < HIDDEN_ROWS) continue;
      if (fieldY >= HEIGHT) continue;
      if (fieldX < 0) continue;
      if (fieldX >= WIDTH) continue;

      g_drawField[fieldY - HIDDEN_ROWS][fieldX] = block;
    }
  }
}

/**
 * I slam that piece down!
 */
void game_actionHardDrop() {
  downMany();
  mutate_commitPiece();
}

/**
 * Gravity pulls the piece down gradually
 */
void game_actionSoftDrop() {
  GameCollisions collision = downOne();
  if (collision != COLLIDE_NONE) {
    mutate_commitPiece();
  }
}

/**
 * I move the piece left or right by +/- 1
 */
void game_actionMovement(GameMovements movement) {
  int nextX = g_gameState.positionX + movement;

  // Check out of bounds
  // (don't constrain on left, as the left edge of a block's 4x4 grid could be empty. We do a collide check anyway)
  if (nextX >= WIDTH) return;

  // Check collisions
  GameCollisions moveCollision = getCollisions(
    getCurrentShape(),
    nextX,
    g_gameState.positionY
  );
  if (moveCollision != COLLIDE_NONE) return;

  // Otherwise, commit change
  mutateState_setX(nextX);
}

/**
 * I rotate the piece clockwise
 */
void game_actionRotate() {
  int nextRotation = blocks_getNextRotation(g_gameState.blockRotation);

  // Check collisions
  GameCollisions rotationCollision = getCollisions(
    blocks_getBlockShape(g_gameState.blockName, nextRotation),
    g_gameState.positionX,
    g_gameState.positionY
  );
  if (rotationCollision != COLLIDE_NONE) return;

  // Otherwise, commit change
  mutateState_setRotation(nextRotation);
}
