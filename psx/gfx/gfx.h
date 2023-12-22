#include <psxgpu.h>

/* Initialise PSX graphics systems; set up global memory buffers for graphics primitives */
void gfx_init();

/* Complete work for displaying a frame; switch over buffers in dual-buffer setup; dispatch work to GPU */
void gfx_endFrame();

/* WIP: Render font texture */
void gfx_loadFontTexture(TIM_IMAGE* p_tim);
void gfx_showFontTexture(TIM_IMAGE* p_tim);