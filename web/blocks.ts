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
  rotations: [
    [
      [0, 0, 0, 0],
      [0, 0, 0, 0],
      [1, 1, 1, 1],
    ],
    [
      [0, 1],
      [0, 1],
      [0, 1],
      [0, 1],
    ],
  ]
}

const blockJ: Block = {
  colour: Colours.TURQUOISE,
  key: 'J',
  rotations: [
    [
      [1, 0, 0],
      [1, 1, 1]
    ],
    [
      [1, 1],
      [1, 0],
      [1, 0],
    ],
    [
      [1, 1, 1],
      [0, 0, 1]
    ],
    [
      [0, 1],
      [0, 1],
      [1, 1],
    ],
  ],
}

const blockL: Block = {
  colour: Colours.PURPLE,
  key: 'L',
  rotations: [
    [
      [0, 0, 1],
      [1, 1, 1]
    ],
    [
      [1, 0],
      [1, 0],
      [1, 1]
    ],
    [
      [1, 1, 1],
      [1, 0, 0]
    ],
    [
      [1, 1],
      [0, 1],
      [0, 1]
    ]
  ]
}

const blockO: Block = {
  colour: Colours.YELLOW,
  key: 'O',
  rotations: [
    [
      [1, 1],
      [1, 1]
    ]
  ]
}

const blockS: Block = {
  colour: Colours.GREEN,
  key: 'S',
  rotations: [
    [
      [0, 1, 1],
      [1, 1, 0]
    ],
    [
      [1, 0],
      [1, 1],
      [0, 1]
    ]
  ]
}

const blockT: Block = {
  colour: Colours.BLUE,
  key: 'T',
  rotations: [
    [
      [0, 1, 0],
      [1, 1, 1]
    ],
    [
      [1, 0],
      [1, 1],
      [1, 0]
    ],
    [
      [1, 1, 1],
      [0, 1, 0]
    ],
    [
      [0, 1],
      [1, 1],
      [0, 1]
    ]
  ]
}

const blockZ: Block = {
  colour: Colours.RED,
  key: 'Z',
  rotations: [
    [
      [1, 1, 0],
      [0, 1, 1]
    ],
    [
      [0, 1],
      [1, 1],
      [1, 0]
    ]
  ]
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
