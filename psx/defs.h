#pragma once

#include <psxgpu.h>

#define OT_SIZE 16
#define PACKETS_SIZE 8192
#define SCREEN_W 320
#define SCREEN_H 240
#define FONT_ROW_LENGTH 16
#define FONT_GLYPH_SIZE 8
#define BLOCK_SIZE 8
#define WIDTH 10
#define HEIGHT 26
#define SIZE_PADDING 20

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
