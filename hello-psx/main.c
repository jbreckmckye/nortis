#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <psxgpu.h>

// Consult https://psx.arthus.net/sdk/Psy-Q/DOCS/Devrefs/Libref.pdf and 
// https://psx.arthus.net/sdk/Psy-Q/DOCS/Devrefs/Libovr.pdf for Psy-Q functions,
// many are equivalent to PSn00bSDK

// Set up dual buffer rendering
// "The display area is a rectangular section of the frame buffer used to display the video image"
// "The drawing environment contains general information related to two-dimensional primitive drawing, such
//  as the position of the drawing area and the drawing offset."
// We set up two of each
DISPENV display_env[2];
DRAWENV draw_env[2];
int buffer_id = 0;

int main(int argc, const char **argv) {
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
  // index for setting both
  PutDispEnv(&display_env[0]);
  PutDrawEnv(&draw_env[0]);

  // Load the debug font texture into VRAM at x400,y0 and CLUT (colour lookup table) +128 px below
  FntLoad(960, 0);

  // Create a print stream. Coords are view-env relative. Returns id in case multiple streams required (max 8)
  int printStream = FntOpen(
    20, 20,           // int x, int y
    300, 220,         // int w, int h
    0,                // int isbg - clears background behind print stream rect
    80                // int n - max chars
  );

  // Remove the display mask that was set by ResetGraph()
  SetDispMask(1);

  // Render loop
  while (1) {
    // Draw text into stream rect. Text is always printed in UPPERCASE
    FntPrint(printStream, "Hello PlayStation!!!");
    // Flush stream rect into draw env
    FntFlush(printStream);

    // Wait for the GPU to finish any pending work before sending to video
    DrawSync(0);
    // Wait for video signal to reach end of frame (Vertical Sync)
    VSync(0);

    // Swap buffers
    buffer_id = !buffer_id;

    // Apply display and draw envs for frame
    PutDispEnv(&display_env[buffer_id]);
    PutDrawEnv(&draw_env[buffer_id]);
  }

  return 0;
}
