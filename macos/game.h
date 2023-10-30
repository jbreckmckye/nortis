#include "defs.h"
#include <stdint.h>

/**
 * Pointer to read-only draw state 
 */
const DrawField* game_p_drawField;

const GameState* game_p_gameState;

uint64_t game_getSpeed();

/**
 * Updates draw-state, call before render
 */
void game_updateDrawState();

void game_actionHardDrop();

void game_actionMovement(GameMovements movement);

void game_actionRestart();

void game_actionRotate();

/**
 * Gravity-based drop by one row 
 */
void game_actionSoftDrop();
