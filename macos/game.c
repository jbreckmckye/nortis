#include "game.h"
#include "blocks.h"
#include "defs.h"

#include <assert.h>
#include <stdlib.h>

static Field g_field;
static DrawField g_drawField;
static GameState g_gameState;

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
        if (projectedY >= HEIGHT || g_field[projectedY][projectedX]) {
          return COLLIDE_BOTTOMWALL;
        }
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
        if (g_field[projectedY][projectedX]) {
          return COLLIDE_CELL;
        }
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
 * Update state for a new block spawn
 */
static GameCollisions mutateState_spawn() {
  g_gameState.blockName = BLOCK_I; // randomBlock();
  g_gameState.blockRotation = 0;
  g_gameState.positionX = 5;
  g_gameState.positionY = 0;

  shapeHex shape = getBlockShape(g_gameState.blockName, 0);
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

/**
 * Public values
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

/**
 * Copies field + piece items into a field grid
 */
void game_updateDrawField() {
  for (int y = HIDDEN_ROWS; y < HEIGHT; y++) {
    int shiftedY = y - HIDDEN_ROWS;
    for (int x = 0; x < WIDTH; x++) {
      g_drawField[shiftedY][x] = g_field[y][x];
    }
  }

  shapeHex shape = getBlockShape(g_gameState.blockName, g_gameState.blockRotation);
  BlockNames block = g_gameState.blockName;

  const int DRAW_FIELD_HEIGHT = HEIGHT - HIDDEN_ROWS;

  for (int y = 0; y <= 3; y++) {
    for (int x = 0; x <= 3; x++) {
      int bit = getShapeBit(shape, y, x);
      // Skip if empty bit
      if (bit == 0) continue;

      // Get projections, bound to field limits
      int projectedY = g_gameState.positionY + y - HIDDEN_ROWS;
      int projectedX = g_gameState.positionX + x;

      if (projectedY < 0) continue;
      if (projectedY >= DRAW_FIELD_HEIGHT) continue;
      if (projectedX < 0) continue;
      if (projectedX >= DRAW_FIELD_HEIGHT) continue;

      // Copy block into field
      g_drawField[projectedY][projectedX] = block;
    }
  }
}

// void game_actionHardDrop();
// void game_actionMovement(GameMovements);
// void game_actionRestart();
// void game_actionRotate();
// void game_actionSoftDrop();