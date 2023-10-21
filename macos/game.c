#include "game.h"
#include "blocks.h"
#include "defs.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static Field g_field = { 0 };
static DrawField g_drawField = { 0 };
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
  g_gameState.blockName = randomBlock();
  g_gameState.blockRotation = 0;
  g_gameState.positionX = 4;
  g_gameState.positionY = 0;

  switch (g_gameState.blockName) {
    case BLOCK_I:
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
  for (int y = 0; y < DRAW_HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      g_drawField[y][x] = g_field[y + HIDDEN_ROWS][x];
    }
  }

  shapeHex shape = getBlockShape(g_gameState.blockName, g_gameState.blockRotation);
  BlockNames block = g_gameState.blockName;

  printf("Block drawn is %d , pos %d %d \n", block, g_gameState.positionY, g_gameState.positionX);

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
// void game_actionSoftDrop();
