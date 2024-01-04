#include <psxgpu.h>
#include <stdio.h>

#include "../defs.h"
#include "gfx.h"

static RenderContext ctx = {0};

// Special characters in font:
// $ = happy emoji, % = sad emoji, () = left/right arrow, & = up arrow, ' = down arrow
static const char* TITLE = "PLAY NOTRIS $";
static const char* GAMEOVER = "GAME OVER %";
static const char* CONTROL_MOVE = "() MOVE, & ROTATE";
static const char* CONTROL_DROP = "' FAST DROP";
static const char* SCORE = "SCORE";
static const char* RESTART = "[START] TO RESTART";
static const char* KREDITS = "2024 JBRECKMCKYE + PSNOOBSDK ^_^"

extern const uint32_t tim_font[];
extern const size_t tim_font_size;

static TIM_IMAGE g_font;

static void loadTexture(const uint32_t* p_data, TIM_IMAGE* p_parsedTim) {
  GetTimInfo(p_data, p_parsedTim);

  // Upload pixels (pixels-rect, pixels-addr)
  LoadImage(p_parsedTim->prect, p_parsedTim->paddr);
  DrawSync(0);

  // Upload CLUT if present (check for bit 4 aka 0x8)
  if (p_parsedTim->mode & 0x8) {
    // Upload CLUT (clut-rect, clut-addr)
    LoadImage(p_parsedTim->crect, p_parsedTim->caddr);
    DrawSync(0);
  }
}

void gfx_init() {
  ResetGraph(0);

  RenderBuffer* p_buffer0 = &(ctx.buffers[0]);
  RenderBuffer* p_buffer1 = &(ctx.buffers[1]);

  DISPENV* p_dispEnv0 = &(p_buffer0->displayEnv);
  DISPENV* p_dispEnv1 = &(p_buffer1->displayEnv);

  DRAWENV* p_drawEnv0 = &(p_buffer0->drawEnv);
  DRAWENV* p_drawEnv1 = &(p_buffer1->drawEnv);

  // Arrange two framebuffers vertically. Display and draw environments are flipped.
  SetDefDispEnv(p_dispEnv0, 0, 0, SCREEN_W, SCREEN_H);
  SetDefDispEnv(p_dispEnv1, 0, SCREEN_H, SCREEN_W, SCREEN_H);
  SetDefDrawEnv(p_drawEnv0, 0, SCREEN_H, SCREEN_W, SCREEN_H);
  SetDefDrawEnv(p_drawEnv1, 0, 0, SCREEN_W, SCREEN_H);

  // Autoclear every frame
  setRGB0(p_drawEnv0, 0x03, 0x0A, 0x12);
  setRGB0(p_drawEnv1, 0x03, 0x0A, 0x12);
  p_drawEnv0->isbg = 1;
  p_drawEnv1->isbg = 1;

  // Initialise first renderbuffer
  ctx.bufferID = 0;
  ctx.p_primitive = p_buffer0->primitivesBuffer;
  ClearOTagR(p_buffer0->orderingTable, OT_SIZE);

  // Unmask video output
  SetDispMask(1);
}

void gfx_loadFontTexture() {
  loadTexture(tim_font, &g_font);
}

static void cmdTexture(TIM_IMAGE* p_tim, int zIndex) {
  uint32_t* p_orderTable = ctx.buffers[ctx.bufferID].orderingTable;

  // Some complex math thing macro to get texture page coordinates
  uint16_t tPage = getTPage(
    p_tim->mode & 0x3, // 00,01,10,11 -> sets bpp mode
    0,
    p_tim->prect->x, // pixel data rect
    p_tim->prect->y  // pixel data rect
  );

  // Chomp from primitive buffer
  DR_TPAGE* p_setTexture = (DR_TPAGE*)ctx.p_primitive;
  ctx.p_primitive += sizeof(DR_TPAGE);

  setDrawTPage(p_setTexture, 0, 0, tPage);
  addPrim(p_orderTable + zIndex, p_setTexture);
}

static void cmdCharacterSprite(int x, int y, int charCode, int zIndex) {
  uint32_t* p_orderTable = ctx.buffers[ctx.bufferID].orderingTable;

  // Chomp from primitive buffer
  SPRT* p_sprite = (SPRT*)ctx.p_primitive;
  ctx.p_primitive += sizeof(SPRT);

  int charIndex = charCode - 32;
  if (charIndex < 0 || charIndex > 64) {
    printf("Bad character %d\n", charCode);
    charIndex = 0;
  }

  int u = (charIndex % 8) * 8;
  int v = (charIndex / (int)8) * 8;

  setSprt(p_sprite);
  setXY0(p_sprite, x, y);
  setWH0(p_sprite, 8, 8);
  setUV0(p_sprite, u, v);
  setClut(p_sprite, g_font.crect->x, g_font.crect->y);
  setRGB0(p_sprite, 128, 128, 128);

  addPrim(p_orderTable + zIndex, p_sprite);
}

static void gfx_drawFontString(int x, int y, char* string, int zIndex) {
  int i = 0;
  char next = string[i];
  while (next) {
    int charX = x + (i * 8);
    cmdCharacterSprite(charX, y, next, zIndex);
    next = string[++i];
  }

  // Must insert this later than the textured primitives as cmds processed in reverse
  cmdTexture(&g_font, zIndex);
}

void gfx_showFontTexture() {
  uint32_t* orderingTable = ctx.buffers[ctx.bufferID].orderingTable;
  TIM_IMAGE* p_tim = &g_font;

  // Direct the drawenv which texture page to read from.
  // I don't really know how the macro arrives at this magic integer
  uint16_t tPage = getTPage(
    p_tim->mode & 0x3,
    0,
    p_tim->prect->x,
    p_tim->prect->y
  );

  // Create a sprite
  SPRT* sprite = (SPRT*)ctx.p_primitive;
  setSprt(sprite);
  // Set dimensions
  setXY0(sprite, 32, 32);
  setWH(sprite, 256, 64);
  // Set UV
  setUV0(sprite, 0, 0);
  // Apply CLUT
  setClut(sprite, p_tim->crect->x, p_tim->crect->y);
  // Set colour (neutral=128)
  setRGB0(sprite, 128, 128, 128);

  // Insert in ordering table
  addPrim(orderingTable, sprite);

  // Advance primitive pointer
  ctx.p_primitive += sizeof(SPRT);

  // Insert DR_TPAGE to set sprite texture page (after sprite => processed before)
  DR_TPAGE* p_setTexture = (DR_TPAGE*)ctx.p_primitive;
  setDrawTPage(
    p_setTexture,
    0, 0,
    tPage // custom tpage, could be an issue
  );
  addPrim(orderingTable + OT_SIZE - 1, p_setTexture);

  // Advance primitive pointer
  ctx.p_primitive += sizeof(DR_TPAGE);
}

void gfx_endFrame() {
  // Wait for GPU to finish current drawing, then the next vertical blanking interval
  DrawSync(0);
  VSync(0);

  RenderBuffer* p_buffer = &(ctx.buffers[ctx.bufferID]);
  DISPENV* p_dispenv = &(p_buffer->displayEnv);
  DRAWENV* p_drawenv = &(p_buffer->drawEnv);
  uint32_t* p_ordertable = p_buffer->orderingTable;

  // Show next displayEnvironment (ie opposing buffer's drawEnv, which was finalised after DrawSync)
  PutDispEnv(p_dispenv);

  // Start GPU drawing current buffer's drawEnv
  PutDrawEnv(p_drawenv);
  // Send primitive / packet buffer to GPU to start drawing, starting from end of table
  DrawOTagEnv(p_ordertable + (OT_SIZE - 1), &(p_buffer->drawEnv));

  // Swap buffers, prepare for next frame
  ctx.bufferID ^= 1;
  RenderBuffer* p_next_buffer = &(ctx.buffers[ctx.bufferID]);
  // Reset starting primitive / packet
  ctx.p_primitive = p_next_buffer->primitivesBuffer;
  // Clear ordering table
  ClearOTagR(p_next_buffer->orderingTable, OT_SIZE);
}
