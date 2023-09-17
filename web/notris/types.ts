export type Flag = 0 | 1
export type ShapeRow = Flag[]
export type Shape = ShapeRow[]

export type Block = Readonly<{
  key: 'I' | 'O' | 'S' | 'Z' | 'L' | 'J' | 'T'
  colour: Colours,
  rotations: Shape[],
  size: number
}>

export enum Colours {
  NIL,
  BLUE,
  GREEN,
  RED,
  ORANGE,
  PURPLE,
  TURQUOISE,
  YELLOW
}

export type Field = Colours[][]

export enum Phase {
  PLAYING,
  GAMEOVER
}

export type PlayState = {
  block: Block,
  lines: number,
  phase: Phase,
  rotation: number,
  score: number,
  x: number,
  y: number
}

export enum Collisions {
  NONE,
  RIGHT, // Right wall
  BOTTOM, // Bottom wall
  LEFT, // Let wall
  CELL// Another piece on grid
}

export enum Movement {
  LEFT,
  RIGHT
}
