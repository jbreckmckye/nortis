import { Shape, Colours, Block } from './types'

function rotate(shape: Shape): Shape {
  const size = shape.length
  const next = shape.slice(0).map(row => row.slice(0))

  // Input rows = output columns in reverse (right hand turn)
  for (let inRow = 0; inRow < size; inRow++) {
    for (let inCol = 0; inCol < size; inCol++) {
      next[inCol][size - 1 - inRow] = shape[inRow][inCol]
    }
  }

  return next
}

function rotations(count: number, shape: Shape): Shape[] {
  const list = [shape]
  for (let i = 1; i < count; i++) {
    list[i] = rotate(list[i - 1])
  }
  return list
}

const blockI: Block = {
  colour: Colours.ORANGE,
  key: 'I',
  rotations: rotations(2, [
    [0, 0, 0, 0],
    [0, 0, 0, 0],
    [1, 1, 1, 1],
    [0, 0, 0, 0]
  ]),
  size: 4
}

const blockJ: Block = {
  colour: Colours.TURQUOISE,
  key: 'J',
  rotations: rotations(4, [
    [1, 0, 0],
    [1, 1, 1],
    [0, 0, 0],
  ]),
  size: 3
}

const blockL: Block = {
  colour: Colours.PURPLE,
  key: 'L',
  rotations: rotations(4, [
    [0, 0, 0],
    [0, 0, 1],
    [1, 1, 1],
  ]),
  size: 3
}

const blockO: Block = {
  colour: Colours.YELLOW,
  key: 'O',
  rotations: rotations(1, [
    [1, 1],
    [1, 1]
  ]),
  size: 2
}

const blockS: Block = {
  colour: Colours.GREEN,
  key: 'S',
  rotations: rotations(2, [
    [0, 0, 0],
    [0, 1, 1],
    [1, 1, 0],
  ]),
  size: 3
}

const blockT: Block = {
  colour: Colours.BLUE,
  key: 'T',
  rotations: rotations(4, [
    [0, 0, 0],
    [0, 1, 0],
    [1, 1, 1],
  ]),
  size: 3
}

const blockZ: Block = {
  colour: Colours.RED,
  key: 'Z',
  rotations: rotations(4, [
    [0, 0, 0],
    [1, 1, 0],
    [0, 1, 1],
  ]),
  size: 3
}

const selection = [
  blockI,
  blockJ,
  blockL,
  blockO,
  blockS,
  blockT,
  blockZ
]

export function randomBlock() {
  const index = Math.floor(Math.random() * 7)
  return selection[index]
}
