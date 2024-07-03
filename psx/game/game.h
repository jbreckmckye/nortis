#include <stdint.h>

#include "../defs.h"

/**
 * GAME.H
 * ############################################################################
 * Provides function for executing the game.
 * Expects to receive events via *action functions.
 * Exposes DrawField (graphics state) and GameState (game state) readonly ptrs
 */

extern DrawField* game_p_drawField;
extern GameState* game_p_state;

/**
 * Informs caller how often to call (level speed)
 */
int32_t game_getSpeed();

/**
 * Updates draw-state, call before render
 */
void game_updateDrawState();

/**
 * ACTIONS
 * - hard drop (press X)
 * - move left/right (pad)
 * - restart (when dead)
 * - rotate (circle)
 * - soft drop (due to ticks)
 */

void game_actionRestart();

void game_actionHardDrop();

void game_actionSoftDrop();

void game_actionMovement(GameMovements movement);

void game_actionRotate();
