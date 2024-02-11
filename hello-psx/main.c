#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <psxgpu.h>
#include <psxapi.h>
#include <psxpad.h>

#define BOX_RADIUS 16
#define OT_LENGTH 16
#define BUFFER_LENGTH 32768

// Consult https://psx.arthus.net/sdk/Psy-Q/DOCS/Devrefs/Libref.pdf and 
// https://psx.arthus.net/sdk/Psy-Q/DOCS/Devrefs/Libovr.pdf for Psy-Q functions,
// many are equivalent to PSn00bSDK

// GLOBAL STATE (GFX BUFFERS)
// ------------------------------------------------------------------------------------------------------

char* helloMsg = "Hello PlayStation 1 !!!";

// Set up dual buffer rendering
// "The display area is a rectangular section of the frame buffer used to display the video image"
// "The drawing environment contains general information related to two-dimensional primitive drawing, such
//  as the position of the drawing area and the drawing offset."
// Shapes --[drawn into]--> Drawing area --[switched into]--> Display area --[blitted out]--> Video
// We set up two of each to switch back and forth between frames using PutDispEnv and PutDrawEnv
DISPENV display_env[2];
DRAWENV draw_env[2];
int buffer_id = 0;

// Set up ordering table and packet buffer
// Two of each for dual buffer rendering
uint32_t ordering_table[2][OT_LENGTH];
uint8_t packets[2][BUFFER_LENGTH];
uint8_t* p_nextPacket;

// Buffer for controller input. 2 controller ports, 34 bytes each to support 4 controllers in a multitap
char joypads[2][34] = { 0xFF };

// Args aren't typically set. At least they aren't set by the PSX kernel. Though they could be set by crt0 (runtime code prepended by linker)
int main(int argc, const char **argv) {
  // INITIALISING GRAPHICS
  // ------------------------------------------------------------------------------------------------------

  // Resets the graphics system, applies a mask
  // This also enables interrupts (needed for e.g. calling VSync())
  // Has three modes; see RunTimeLibrary overview: https://psx.arthus.net/sdk/Psy-Q/DOCS/Devrefs/Libovr.pdf
  ResetGraph(0);

  // Initialise draw buffers in 320x240 mode
  // Vertical arrangement is most common
  SetDefDispEnv(
    &display_env[0], // DISPENV* env
    0, 0,            // int x, int y
    320, 240         // int w, int h
  );
  SetDefDispEnv(
    &display_env[1],
    0, 240,
    320, 240
  );

  // Initialise draw environments
  // These should generally be aligned to the display environments, but in the opposite order
  SetDefDrawEnv(
    &draw_env[0],    // DRAWENV* env
    0, 240,          // int x, int y
    320, 240         // int w, int h
  );
  SetDefDrawEnv(
    &draw_env[1],
    0, 0,
    320, 240
  );

  // We want to enable background clearing for the draw environments. This prevents
  // 'hall of mirror' effects. We have to set the clear colour too
  setRGB0(&draw_env[0], 0, 0x48, 0x89);
  setRGB0(&draw_env[1], 0, 0x48, 0x89);
  draw_env[0].isbg = 1;
  draw_env[1].isbg = 1;

  // Applies the display and draw envs to the GPU
  // Because the display and draw env y positions are inverted, we can use the same
  // index for setting both :)
  PutDispEnv(&display_env[0]);
  PutDrawEnv(&draw_env[0]);

  // PREPARING DEBUG TEXT
  // ------------------------------------------------------------------------------------------------------

  // Load the debug font texture into VRAM at x400,y0 and CLUT (colour lookup table) +128 px below
  FntLoad(960, 0);

  // Create a print stream. Coords are view-env relative. Returns id in case multiple streams required (max 8)
  int printStream = FntOpen(
    20, 20,           // int x, int y
    300, 220,         // int w, int h
    0,                // int isbg - clears background behind print stream rect
    80                // int n - max chars
  );

  // INITIALISE JOYPADS
  // ------------------------------------------------------------------------------------------------------
  InitPAD(joypads[0], 34, joypads[1], 34);
  StartPAD(); // begins polling every vsync
  ChangeClearPAD(1); // resolves issue where StartPad initially enables VSync interrupts, which causes the first VSync to time out

  // 16 bit map of current joypad state
  PADTYPE* p_pad;

  // RENDER LOOP
  // ------------------------------------------------------------------------------------------------------

  // Remove the display mask that was set by ResetGraph() (shows the screen)
  SetDispMask(1);

  // Tile position (from top left origin)
  int x = 120 - BOX_RADIUS;
  int y = 120 - BOX_RADIUS;
  int width = BOX_RADIUS * 2;
  int dx = 1;
  int dy = 1;

  // Player tile position
  int playerX = x;
  int playerY = y;

  // Render loop
  while (1) {
    // UPDATE RED BLOCK LOCATION
    // ------------------------------------------------------------------------------------------------------

    if (x <= 0) {
      dx = 1;
    } else if (x + width >= 320) {
      dx = -1;
    }
    x += dx;

    if (y <= 0) {
      dy = 1;
    } else if (y + width >= 240) {
      dy = -1;
    }
    y += dy;

    // DRAWING A PRIMITIVE (RED BLOCK)
    // ------------------------------------------------------------------------------------------------------

    // Reset nextPrimitive to point to start of the *current* primitive buffer
    p_nextPacket = packets[buffer_id];

    // Reset ordering table
    ClearOTagR(ordering_table[buffer_id], OT_LENGTH);

    // Create a tile primitive in the primitive buffer
    // The buffer is a contiguous array of chars, and primitives can be any n chars
    // We cast p_nextPrimitive as a TILE*, so that its char used as the head of the TILE struct
    TILE* p_tile = (TILE*)p_nextPacket;
    setTile(p_tile);
    setXY0 (p_tile, x, y);
    setWH  (p_tile, width, width);
    setRGB0(p_tile, 252, 32, 3);
  

    // Link into ordering table (z level 2)
    int z = 2;
    addPrim(ordering_table[buffer_id] + z, p_nextPacket);

    // Then advance buffer
    p_nextPacket += sizeof(TILE);

    // READ CONTROLLER AND UPDATE YELLOW BLOCK LOCATION
    // ------------------------------------------------------------------------------------------------------
    // Pull from first pad in slot 1
    p_pad = (PADTYPE *)joypads[0];
    if (p_pad->stat == 0) { // check joy1 connected
      if (p_pad->type == 0x04) {
        uint16_t buttons = ~(p_pad->btn);
        if (buttons & PAD_UP || buttons & PAD_TRIANGLE) {
          playerY -= 2;
          if (playerY < 0) playerY = 0;
        }
        if (buttons & PAD_DOWN || buttons & PAD_CROSS) {
          playerY += 2;
          if (playerY + width > 240) playerY = 240;
        }
        if (buttons & PAD_LEFT || buttons & PAD_SQUARE) {
          playerX -= 2;
          if (playerX < 0) playerX = 0;
        }
        if (buttons & PAD_RIGHT || buttons & PAD_CIRCLE) {
          playerX += 2;
          if (playerX + width > 320) playerX = 320;
        }
      } else {
        printf("Joypad 1 not connected or not digital pad mode\n");
      }
    }

    // ANOTHER PRIMITIVE (YELLOW BLOCK, IN FRONT OF THE OTHER) (PLAYER CONTROLLED)
    // ------------------------------------------------------------------------------------------------------

    TILE* p_tile2 = (TILE*)p_nextPacket;
    setTile(p_tile2);
    setXY0 (p_tile2, playerX, playerY); // moves in reverse direction to tile 1
    setWH  (p_tile2, width, width);
    setRGB0(p_tile2, 252, 186, 3);

    // Link into ordering table, in front of tile 1 (z-level 1)
    // If we re-used z=2 this would go behind (as it is put lastmost in that OT index, which means it's processed first)
    addPrim(ordering_table[buffer_id] + z - 1, p_nextPacket);

    // Again increment buffer watermark
    p_nextPacket += sizeof(TILE);

    // DRAWING DEBUG TEXT
    // ------------------------------------------------------------------------------------------------------

    // Draw text into stream rect. Text is always printed in UPPERCASE
    FntPrint(printStream, helloMsg);
    // Flush stream rect into draw env
    FntFlush(printStream);

    // VSYNC, DRAW, BUFFER SWAP
    // ------------------------------------------------------------------------------------------------------

    // Wait for the GPU to finish any pending work before sending to video
    DrawSync(0);
    // Wait for video signal to reach end of frame (Vertical Sync)
    VSync(0);

    // Apply display and draw envs for frame
    PutDispEnv(&display_env[buffer_id]);
    PutDrawEnv(&draw_env[buffer_id]);

    // Send ordering table to GPU via DMA. Have to start with final element in reverse-linked-list
    DrawOTagEnv(&ordering_table[buffer_id][OT_LENGTH - 1], &draw_env[buffer_id]);

    // Swap buffers for next frame
    buffer_id = !buffer_id;
  }

  return 0;
}
