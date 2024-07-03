#include <stdio.h>
#include <psxapi.h>
#include <psxpad.h>
#include <stdbool.h>
#include <limits.h>

#include "../defs.h"
#include "pad.h"

/**
 * PAD.C
 * ############################################################################
 * Helpers for polling joypad state
 */

// Control input buffer - 2 controllers with 34 bytes each
uint8_t pads[2][34];

// Input event change polling
uint16_t lastBtn = 0;
uint32_t btnHeldFor = 0;

void pad_init() {
  InitPAD(pads[0], 34, pads[1], 34);
  // Poll every vsync
  StartPAD();
  // PSX bugfix: StartPad initially enables VSync interrupts, causing first vsync to timeout
  ChangeClearPAD(1);
}

// Returns a 16 bit number mask of button states
// Normally input values are inverted (1=unpressed), this inverts so we 
// can do bitwise ops with PAD_UP etc
static uint16_t pad_buttons1() {
  PADTYPE* pad = (PADTYPE*)pads[0];

  // Exit if: not connected
  if (pad->stat != 0) return 0;

  // Exit if: not digital
  if (pad->type != PAD_ID_DIGITAL) return 0;

  return ~ pad->btn;
}

GameInputs pad_getInput() {
  uint16_t btn = pad_buttons1();

  // Watching for input changes
  if (btn == lastBtn) {
    btnHeldFor++;
  } else {
    lastBtn = btn;
    btnHeldFor = 0;
  }

  if (btnHeldFor == INT32_MAX) {
    btnHeldFor = 0;
  }

  // Actions that don't repeat until the button is re-pressed
  if (btnHeldFor == 0) {
    if (btn & PAD_CIRCLE) return INPUT_ROTATE;
    if (btn & PAD_CROSS) return INPUT_DROP;
    if (btn & PAD_START) return INPUT_RESTART;
  }

  // Continuous actions that repeat quickly as the key is held
  if (btnHeldFor % 20 == 0) {
    if (btn & PAD_LEFT) return INPUT_LEFT;
    if (btn & PAD_RIGHT) return INPUT_RIGHT;
  }

 return INPUT_NONE;
}

// printf selected buttons e.g "up, x, L1 pressed"
void pad_debug() {
  uint16_t btn = pad_buttons1();
  if (btn & PAD_UP) printf("up, ");
  if (btn & PAD_LEFT) printf("left, ");
  if (btn & PAD_DOWN) printf("down, ");
  if (btn & PAD_RIGHT) printf("right, ");
  if (btn & PAD_TRIANGLE) printf("tri, ");
  if (btn & PAD_CIRCLE) printf("circ, ");
  if (btn & PAD_CROSS) printf("x, ");
  if (btn & PAD_SQUARE) printf("squ, ");
  if (btn & PAD_L1) printf("L1, ");
  if (btn & PAD_R1) printf("R1, ");
  if (btn & PAD_L2) printf("L2, ");
  if (btn & PAD_R2) printf("R2, ");
  if (btn & PAD_SELECT) printf("sel, ");
  if (btn & PAD_START) printf("start, ");
  if (btn) printf("pressed\n");

  GameInputs parsed = pad_getInput();
  printf("Parsed input val = %d\n", parsed);
}
