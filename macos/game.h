#include "defs.h"

/**
 * Pointer to read-only draw state 
 */
const DrawField* game_p_drawField;

/**
 * Updates draw-state, call before render
 */
void game_updateDrawState();

// void game_actionHardDrop();
// void game_actionMovement(GameMovements);
void game_actionRestart();
// void game_actionRotate();
// void game_actionSoftDrop();
