#include <psxgpu.h>
#include <stdio.h>

#include "../defs.h"
#include "gfx.h"

/**
 * GFX.C
 * ############################################################################
 * Functions for graphics primitives, drawing shapes & text
 */

static RenderContext ctx = {0};

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

  int u = (charIndex % FONT_ROW_LENGTH) * FONT_GLYPH_SIZE;
  int v = (charIndex / (int)FONT_ROW_LENGTH) * FONT_GLYPH_SIZE;

  setSprt(p_sprite);
  setXY0(p_sprite, x, y);
  setWH(p_sprite, FONT_GLYPH_SIZE, FONT_GLYPH_SIZE);
  setUV0(p_sprite, u, v);
  setClut(p_sprite, g_font.crect->x, g_font.crect->y);
  setRGB0(p_sprite, 128, 128, 128);

  addPrim(p_orderTable + zIndex, p_sprite);
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
  setRGB0(p_drawEnv0, 0x02, 0x02, 0x02);
  setRGB0(p_drawEnv1, 0x02, 0x02, 0x02);
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

void gfx_drawFontString(int x, int y, const char* string, int zIndex) {
  int i = 0;
  char next = string[i];
  while (next) {
    int charX = x + (i * FONT_GLYPH_SIZE);
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

void gfx_drawBlock(XYWH coords, RGB fill, RGB accentLight, RGB accentDark) {
  uint32_t* p_orderingTable = ctx.buffers[ctx.bufferID].orderingTable;

  // A block has two triangles for the light / dark accents, then a tile in the middle

  Vector2 northWest = { coords.x, coords.y };
  Vector2 northEast = { coords.x + coords.w, coords.y };
  Vector2 southEast = { coords.x + coords.w, coords.y + coords.h };
  Vector2 southWest = { coords.x, coords.y + coords.h };

  // Accent top-left
  POLY_F3* p_upperLeft = (POLY_F3*)ctx.p_primitive;
  ctx.p_primitive += sizeof(POLY_F3);
  setPolyF3(p_upperLeft);
  setXY3(p_upperLeft, northEast.x, northEast.y, northWest.x, northWest.y, southWest.x, southWest.y);
  setRGB0(p_upperLeft, accentLight.r, accentLight.g, accentLight.b);

  // Accent bottom-right
  POLY_F3* p_bottomRight = (POLY_F3*)ctx.p_primitive;
  ctx.p_primitive += sizeof(POLY_F3);
  setPolyF3(p_bottomRight);
  setXY3(p_bottomRight, northEast.x, northEast.y, northWest.x, northWest.y, southWest.x, southWest.y);
  setRGB0(p_bottomRight, accentLight.r, accentLight.g, accentLight.b);

  // Tile
  TILE* p_tile = (TILE*)ctx.p_primitive;
  ctx.p_primitive += sizeof(TILE);
  setTile(p_tile);
  setXY0(p_tile, coords.x + 1, coords.y + 1);
  setWH(p_tile, coords.w - 2, coords.h - 2);
  setRGB0(p_tile, fill.r, fill.g, fill.b);

  // Add primitives to ordering table
  // Triangles at back
  addPrim(p_orderingTable, p_tile);
  addPrim(p_orderingTable + 1, p_upperLeft);
  addPrim(p_orderingTable + 1, p_bottomRight);
}

void gfx_drawCornerColouredBox(XYWH coords, RGB rgbNW, RGB rgbNE, RGB rgbSE, RGB rgbSW) {
  /**
   * Create primitives
   * lineTop, lineRight, lineBtm, lineLeft
   */

  LINE_G2* p_lineTop = (LINE_G2*)ctx.p_primitive;
  ctx.p_primitive += sizeof(LINE_G2);
  setLineG2(p_lineTop);

  LINE_G2* p_lineRight = (LINE_G2*)ctx.p_primitive;
  ctx.p_primitive += sizeof(LINE_G2);
  setLineG2(p_lineRight);

  LINE_G2* p_lineBtm = (LINE_G2*)ctx.p_primitive;
  ctx.p_primitive += sizeof(LINE_G2);
  setLineG2(p_lineBtm);

  LINE_G2* p_lineLeft = (LINE_G2*)ctx.p_primitive;
  ctx.p_primitive += sizeof(LINE_G2);
  setLineG2(p_lineLeft);

  /*
   * Insert into ordering table 
   */
  uint32_t* p_orderTable = ctx.buffers[ctx.bufferID].orderingTable;
  addPrim(p_orderTable, p_lineTop);
  addPrim(p_orderTable, p_lineRight);
  addPrim(p_orderTable, p_lineBtm);
  addPrim(p_orderTable, p_lineLeft);

  /*
   * Configure line primitives
   */

  int x0 = coords.x;
  int x1 = coords.x + coords.w;
  int y0 = coords.y;
  int y1 = coords.y + coords.h;

  setXY2(p_lineTop, x0, y0, x1, y0);
  setXY2(p_lineRight, x1, y0, x1, y1);
  setXY2(p_lineBtm, x1, y1, x0, y1);
  setXY2(p_lineLeft, x0, y1, x0, y0);

  setRGB0(p_lineTop, rgbNW.r, rgbNW.g, rgbNW.b);
  setRGB1(p_lineTop, rgbNE.r, rgbNE.g, rgbNE.b);

  setRGB0(p_lineRight, rgbNE.r, rgbNE.g, rgbNE.b);
  setRGB1(p_lineRight, rgbSE.r, rgbSE.g, rgbSE.b);

  setRGB0(p_lineBtm, rgbSE.r, rgbSE.g, rgbSE.b);
  setRGB1(p_lineBtm, rgbSW.r, rgbSW.g, rgbSW.b);

  setRGB0(p_lineLeft, rgbSW.r, rgbSW.g, rgbSW.b);
  setRGB1(p_lineLeft, rgbNW.r, rgbNW.g, rgbNW.b);
}
