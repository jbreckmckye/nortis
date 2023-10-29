#include "game.h"
#include "blocks.h"
#include "defs.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static Field g_field = { 0 };
static DrawField g_drawField = { 0 };
static GameState g_gameState;

static shapeHex getCurrentShape() {
  return getBlockShape(g_gameState.blockName, g_gameState.blockRotation);
}

/**
 * Get collisions for proposed (drop/spawn) shape, x, y values
 */
static GameCollisions getDropCollision(shapeHex shape, int x, int y) {
  for (int row = 3; row >= 0; row--) {
    for (int col = 0; col <= 3; col++) {
      int bit = getShapeBit(shape, row, col);
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
 * Get collisions for proposed (rotation) shape, x, y values
 */
static GameCollisions getCollisions(shapeHex shape, int x, int y) {
  for (int row = 0; row <= 3; row++) {
    for (int col = 0; col <= 3; col++) {
      int bit = getShapeBit(shape, row, col);
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

static void mutateField_insertBlock(BlockNames blockType, shapeHex shape, int x, int y) {
  for (int col = 0; col < 4; col++) {
    for (int row = 0; row < 4; row++) {
      int bit = getShapeBit(shape, col, row);
      if (bit) {
        int projectedX = x + row;
        int projectedY = y + col;
        g_field[projectedY][projectedX] = blockType;
      }
    }
  }
}

/**
 * Update state for a new block spawn
 */
static GameCollisions mutateState_spawn() {
  g_gameState.blockName = randomBlock();
  g_gameState.blockRotation = 0;
  g_gameState.positionX = 4;
  g_gameState.positionY = 0;

  switch (g_gameState.blockName) {
    case BLOCK_I:
      g_gameState.positionX = 3;
      g_gameState.positionY = 1;
      break;
    case BLOCK_T:
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
      assert(g_gameState.blockName != 0);
  }

  shapeHex shape = getCurrentShape();
  return getDropCollision(shape, g_gameState.positionX, g_gameState.positionY);
}

/**
 * New game
 */
static void mutateState_resetGame() {
  g_gameState.clearedLines = 0;
  g_gameState.dropDelay = 500;
  g_gameState.points = 0;
  g_gameState.playState = PLAY_PLAYING;
  mutateState_spawn();
}

static void mutateState_gameOver() {
  g_gameState.playState = PLAY_GAMEOVER;
}

static void mutateState_setX(int nextX) {
  g_gameState.positionX = nextX;
}

static void mutateState_setY(int nextY) {
  g_gameState.positionY = nextY;
}

static GameCollisions downOne() {
  int nextX = g_gameState.positionX;
  int nextY = g_gameState.positionY + 1;
  shapeHex shape = getCurrentShape();

  GameCollisions collision = getDropCollision(shape, nextX, nextY);
  if (collision == COLLIDE_NONE) {
    mutateState_setY(nextY);
  }

  return collision;
}

static void downMany() {
  GameCollisions collision = COLLIDE_NONE;
  while (collision == COLLIDE_NONE) {
    collision = downOne();
  }
}

static bool isLineComplete(int y) {
  BlockNames* line = g_field[y];
  for (int x = 0; x < WIDTH; x++) {
    if (!line[x]) return false;
  }
  return true;
}

static void mutateField_clearLine(int y) {
  // Copy from lines above, except top line
  for (int y = HEIGHT - 1; y > 0; y--) {
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

static void action_commitPiece() {
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
 * Public values (docs in header)
 * ============================================================================
 */

/**
 * Public accessor for drawField as a const 
 */
const DrawField* game_p_drawField = &g_drawField;

/**
 * Public functions
 * ============================================================================
 */

void game_actionRestart() {
  mutateField_clear();
  mutateState_resetGame();
}

PlayStates game_getPlayState() {
  return g_gameState.playState;
}

uint64_t game_getSpeed() {
  int setsCleared = g_gameState.clearedLines / 4;
  int level = fmax(fmin(1, setsCleared), 10);
  return 500 - (level * 15);
}

/**
 * Copies field + piece items into a field grid
 */
void game_updateDrawState() {
  for (int y = 0; y < DRAW_HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      g_drawField[y][x] = g_field[y + HIDDEN_ROWS][x];
    }
  }

  shapeHex shape = getCurrentShape();
  BlockNames block = g_gameState.blockName;

  for (int y = 0; y <= 3; y++) {
    for (int x = 0; x <= 3; x++) {
      int bit = getShapeBit(shape, y, x);
      // Skip if empty bit
      if (bit == 0) continue;

      // Get projections, bound to field limits
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

// void game_actionHardDrop();
// void game_actionMovement(GameMovements);
// void game_actionRestart();
// void game_actionRotate();

void game_actionHardDrop() {
  downMany();
  action_commitPiece();
}

void game_actionSoftDrop() {
  GameCollisions collision = downOne();
  if (collision != COLLIDE_NONE) {
    action_commitPiece();
  }
}

void game_actionMovement(GameMovements movement) {
  int nextX = g_gameState.positionX + movement;

  // Check out of bounds
  if (nextX < 0 || nextX >= WIDTH) return;

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