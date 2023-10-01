#include <stdlib.h>
#include "defs.h"

struct Rotation {
  int cells[4][2]; // x,y of each cell. Easier than variable length arrays
} typedef Rotation;

struct PlayBlock {
  Game_Colours colour;
  Rotation rotations[4];
} typedef PlayBlock;

/**
 * Shape T
 * ======================================================== 
 */

static Rotation shape_t_cells_rot1 = {{
          {1, 0},
  {0, 1}, {1, 1}, {2, 1} 
}};

static Rotation shape_t_cells_rot2 = {{
  {0, 0},
  {0, 1}, {1, 1},
  {0, 2}
}};

static Rotation shape_t_cells_rot3 = {{
  {0, 0}, {1, 0}, {2, 0},
          {1, 1}
}};

static Rotation shape_t_cells_rot4 = {{
          {1, 0},
  {0, 1}, {1, 1},
          {1, 2}
}};

const PlayBlock block_T = {
  BLUE,
  {
    shape_t_cells_rot1,
    shape_t_cells_rot2,
    shape_t_cells_rot3,
    shape_t_cells_rot4
  }
};

/**
 * Shape S
 * ======================================================== 
 */

static Rotation shape_s_cells_rot1 = {{
          {1, 0}, {2, 0},
  {0, 1}, {1, 1}
}};

static Rotation shape_s_cells_rot2 = {{
  {0, 0},
  {0, 1}, {1, 1},
          {1, 2}
}};

const PlayBlock block_S = {
  GREEN,
  {
    shape_s_cells_rot1,
    shape_s_cells_rot2,
    shape_s_cells_rot1,
    shape_s_cells_rot2
  }
};

/**
 * Shape Z
 * ======================================================== 
 */

static Rotation shape_z_cells_rot1 = {{
  {0, 0}, {1, 0},
          {1, 1}, {2, 1}
}};

static Rotation shape_z_cells_rot2 = {{
          {1, 0},
  {0, 1}, {1, 1},
  {0, 2}
}};

const PlayBlock block_Z = {
  RED,
  {
    shape_z_cells_rot1,
    shape_z_cells_rot2,
    shape_z_cells_rot1,
    shape_z_cells_rot2
  }
};

/**
 * Shape O
 * ======================================================== 
 */

static Rotation shape_o_cells_rot1 = {{
  {0, 0}, {1, 0},
  {0, 1}, {1, 1}
}};

const PlayBlock block_O = {
  YELLOW,
  {
    shape_o_cells_rot1,
    shape_o_cells_rot1,
    shape_o_cells_rot1,
    shape_o_cells_rot1
  }
};

/**
 * Shape J
 * ======================================================== 
 */

static Rotation shape_j_cells_rot1 = {{
  {0, 0},
  {0, 1}, {1, 1}, {2, 1}
}};

static Rotation shape_j_cells_rot2 = {{
  {0, 0}, {1, 0},
  {0, 1},
  {0, 2}
}};

static Rotation shape_j_cells_rot3 = {{
  {0, 0}, {1, 0}, {2, 0},
                  {2, 1}
}};

static Rotation shape_j_cells_rot4 = {{
          {1, 0},
          {1, 1},
  {0, 2}, {1, 2}
}};

const PlayBlock block_J = {
  PURPLE,
  {
    shape_j_cells_rot1,
    shape_j_cells_rot2,
    shape_j_cells_rot3,
    shape_j_cells_rot4
  }
};

/**
 * Shape L
 * ======================================================== 
 */

static Rotation shape_l_cells_rot1 = {{
                  {2, 0},
  {0, 1}, {1, 1}, {2, 1}
}};

static Rotation shape_l_cells_rot2 = {{
  {0, 0},
  {0, 1},
  {0, 2}, {1, 2}
}};

static Rotation shape_l_cells_rot3 = {{
  {0, 0}, {1, 0}, {2, 0},
  {0, 1}
}};

static Rotation shape_l_cells_rot4 = {{
  {0, 0}, {1, 0},
  {0, 1},
  {0, 2}
}};

const PlayBlock block_L = {
  AQUA,
  {
    shape_l_cells_rot1,
    shape_l_cells_rot2,
    shape_l_cells_rot3,
    shape_l_cells_rot4
  }
};

/**
 * Shape I
 * ======================================================== 
 */

static Rotation shape_i_cells_rot1 = {{
  {0, 0}, {1, 0}, {2, 0}, {3, 0}
}};

static Rotation shape_i_cells_rot2 = {{
  {0, 0},
  {0, 1},
  {0, 2},
  {0, 3}
}};

const PlayBlock block_I = {
  ORANGE,
  {
    shape_i_cells_rot1,
    shape_i_cells_rot2,
    shape_i_cells_rot1,
    shape_i_cells_rot2
  }
};
