#include <stdbool.h>

/**
 * UI.H
 * ############################################################################
 * High level functions for drawing the play state
 */

void ui_renderPlayArea();
void ui_renderScores(int score, int lines);
void ui_renderControls(bool isAlive);
void ui_renderKredits();
void ui_renderTitle(bool isAlive);

// These functions will need to be re-orged
void ui_renderBlock(int u, int v);