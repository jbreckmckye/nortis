import { Collisions, Colours, Field, Movement, Phase, PlayState, Shape } from './types'
import { randomBlock } from './blocks'

/**
 * Game state
 * ============================================================================
 *
 * This code is a prototype for the SDL and PlayStation versions we will
 * write in C. As such we avoid using too many high-level language features,
 * and stick to a much more procedural style that will port easily to C.
 */

// First 2 rows are hidden to provide space for pieces to spawn in
const HIDDEN_ROWS = 2
const HEIGHT = 26
const WIDTH = 10

/**
 * Global state
 * ============================================================================
 */

const field: Field = createField(HEIGHT, WIDTH)
const playState: PlayState = {
  block: null as any,
  phase: Phase.PLAYING,
  rotation: 0,
  score: 0,
  lines: 0,
  x: 0,
  y: 0
}

;(window as any).DEBUG = {
  field,
  playState
}

spawn()

/**
 * Outputs
 * ============================================================================
 */

export function getDrawState() {
  // This function is not very C-y, but the C implementation of this will probably be different
  let fieldCopy = field
    .slice()
    .map(row => row.slice())

  const drawShape = getRotationShape()
    .slice()
    .map(row => row.map(
        cell => cell ? playState.block.colour : Colours.NIL
      )
    )

  // Copy shape into fieldCopy
  drawShape.forEach((row, shapeY) => {
    row.forEach((cell, shapeX) => {
      const fieldX = playState.x + shapeX
      const fieldY = playState.y + shapeY
      if (cell) fieldCopy[fieldY][fieldX] = cell
    })
  })

  return fieldCopy.slice(HIDDEN_ROWS)
}

export function getState() {
  return playState as Readonly<PlayState>
}

export function getLevelSpeed() {
  const level = Math.max(playState.lines / 10, 10)
  return 1000 - (level * 77)
}

/**
 * Inputs
 * ============================================================================
 */

export function actionRestart() {
  playState.phase = Phase.PLAYING
  playState.score = 0
  playState.lines = 0
  mutateClearField()
  spawn()
}

/**
 * Rotate the item. Is debounced but can happen multiple times per tick.
 * We will 'kick' if we touch a sideways wall.
 * Mutates PlayState
 */
export function actionRotate() {
  const { block, x, y } = playState
  const { key } = block

  if (key === 'O') return

  const nextShape = getNextRotationShape()
  const collision = getCollisions(nextShape, x, y)

  if (collision === Collisions.NONE) {
    mutateNextRotation()
    return
  }

  // For wall collisions, attempt to "kick" left/right
  if (collision === Collisions.LEFT || collision === Collisions.RIGHT) {
    const direction = collision === Collisions.LEFT ? 1 : -1
    const kickedX = x + direction
    if (getCollisions(nextShape, kickedX, y) === Collisions.NONE) {
      mutateNextRotation()
      mutateX(kickedX)
      return
    }
  }

  // Collision - not valid
  return
}

/**
 * Typical gravity behaviour. Moves piece down until it collides, at which point it's committed.
 * Mutates PlayState
 */
export function actionSoftDrop() {
  const collision = downOne()
  if (collision !== Collisions.NONE) {
    commitPiece(false)
  }
}

/**
 * Forced gravity. Pushes piece down N times until it can be committed.
 * Mutates PlayState.
 */
export function actionHardDrop() {
  downMany()
  commitPiece(true)
}

/**
 * Move left/right
 * Updates PlayState
 */
export function actionMoveHorizontal(movement: Movement) {
  const projectedX = playState.x + (movement === Movement.LEFT ? -1 : 1)

  if (projectedX >= WIDTH) return

  const shape = getRotationShape()
  const collision = getCollisions(shape, projectedX, playState.y)
  if (collision === Collisions.NONE) {
    mutateX(projectedX)
  }
}

/**
 * Internal state updates
 * ============================================================================
 */

function commitPiece(isHardDrop: boolean) {
  const shape = getRotationShape()
  mutateField(playState.x, playState.y, shape)

  const clearedLines = clearLines()
  if (clearedLines) {
    mutateLineScore(clearedLines, isHardDrop)
  }

  const spawnCollision = spawn()
  if (spawnCollision !== Collisions.NONE) {
    mutateGameover()
  }
}

function spawn(): Collisions {
  const block = randomBlock()
  const shape = block.rotations[0]

  const x = 4
  let y: number

  // Spawn height varies with block type, so it always spawns in-view but has room to rotate
  switch (block.key) {
    case 'I':
      y = 0
      break

    case 'J':
    case 'L':
    case 'O':
    case 'S':
    case 'T':
    case 'Z':
      y = 2
      break
  }

  playState.block = block
  playState.rotation = 0
  playState.x = x
  playState.y = y

  return getDropCollision(shape, x, y)
}

/**
 * Basic piece gravity movement
 */
function downOne(): Collisions {
  const { x, y } = playState

  const shape = getRotationShape()
  const projectedY = y + 1

  const collision = getDropCollision(shape, x, projectedY)
  if (collision === Collisions.NONE) {
    mutateY(projectedY)
  }

  return collision
}

/**
 * Hard drop gravity movement
 */
function downMany() {
  let collision: Collisions = Collisions.NONE
  while (collision === Collisions.NONE) {
    collision = downOne()
  }
}

/**
 * Clear any complete lines, returns no cleared
 */
function clearLines(): number {
  let cleared = 0
  for (let line = 0; line < HEIGHT; line++) {
    if (isLineComplete(line)) {
      mutateDroppedLines(line)
      cleared++
    }
  }

  return cleared
}

/**
 * Get collisions for a proposed drop (or spawn) shape / position
 */
function getDropCollision(shape: Shape, x: number, y: number) {
  for (let row = shape.length - 1; row >= 0; row--) {
    for (let col = 0; col < shape[row].length; col++) {
      if (shape[row][col]) {
        const mappedY = y + row
        const mappedX = x + col
        if (mappedY >= HEIGHT || field[mappedY][mappedX]) {
          return Collisions.BOTTOM
        }
      }
    }
  }
  return Collisions.NONE
}

/**
 * Get collisions for a proposed rotation shape / position
 */
function getCollisions(shape: Shape, x: number, y: number) {
  for (let row = 0; row < shape.length; row++) {
    for (let col = 0; col < shape[row].length; col++) {
      // Check active cells for collisions
      if (shape[row][col]) {
        const mappedX = x + col
        const mappedY = y + row

        // Check out of bounds
        if (mappedX < 0) return Collisions.LEFT
        if (mappedX >= WIDTH) return Collisions.RIGHT
        if (mappedY >= HEIGHT) return Collisions.BOTTOM

        // Check overlap
        const overlap = field[mappedY][mappedX]
        if (overlap !== Colours.NIL) return Collisions.CELL
      }
    }
  }
  return Collisions.NONE
}



/**
 * Field functions
 * ============================================================================
 */

function createField(height: number, width: number): Field {
  return arrayOf(null, height).map(
    _row => arrayOf(Colours.NIL, width)
  )
}

function mutateClearField() {
  for (let y = 0; y < HEIGHT; y++) {
    for (let x = 0; x < WIDTH; x++) {
      field[y][x] = Colours.NIL
    }
  }
}

function mutateField(shapeX: number, shapeY: number, shapeToCopy: Shape) {
  for (let y = 0; y < shapeToCopy.length; y++) {
    const fieldY = shapeY + y
    for (let x = 0; x < shapeToCopy[y].length; x++) {
      if (shapeToCopy[y][x]) {
        const fieldX = shapeX + x
        field[fieldY][fieldX] = playState.block.colour
      }
    }
  }
}

function isLineComplete(y: number) {
  for (const cell of field[y]) {
    if (!cell) return false
  }
  return true
}

/**
 * State getters / setters
 * ============================================================================
 */

function getRotationShape() {
  return playState.block.rotations[playState.rotation]
}

function getNextRotation() {
  return (playState.rotation + 1) % playState.block.rotations.length
}

function getNextRotationShape() {
  return playState.block.rotations[getNextRotation()]
}

function mutateX(x: number) {
  playState.x = x
}

function mutateY(y: number) {
  playState.y = y
}

function mutateNextRotation() {
  playState.rotation = getNextRotation()
}

function mutateLineScore(linesCleared: number, isHardDrop: boolean) {
  playState.lines += linesCleared
  switch (linesCleared) {
    case 1:
      playState.score += isHardDrop ? 2 : 1
      break
    case 2:
      playState.score += isHardDrop ? 4 : 2
      break
    case 3:
      playState.score += isHardDrop ? 8 : 4
      break
    default:
      playState.score += isHardDrop ? 16 : 8
  }
}

function mutateGameover() {
  playState.phase = Phase.GAMEOVER
}

function mutateDroppedLines(droppedLine: number) {
  for (let y = droppedLine; y >= 0; y--) {
    if (y === 0) {
      field[0] = Array(WIDTH).fill(Colours.NIL)
    } else {
      field[y] = field[y - 1].slice()
    }
  }
}

/**
 * Utils
 * ============================================================================
 */

function arrayOf<T>(value: T, size: number) {
  return new Array(size).fill(value)
}
