import { getDrawState, hardDrop, move, rotate } from './notris/game'
import { Movement } from './notris/types'
import { clearBlock, drawBlock } from './notris/gfx'

const canvas = document.getElementById('game') as HTMLCanvasElement
const ctx = canvas.getContext('2d')

/**
 * Binding
 * ============================================================================
 *
 * Sets up render loop, listens to inputs, updates game state on regular ticks
 */

function drawLoop() {
  const rows = getDrawState()
  for (let y = 0; y < rows.length; y++) {
    const row = rows[y]
    for (let x = 0; x < row.length; x++) {
      const cell = row[x]
      if (cell) {
        drawBlock(ctx, x, y, cell)
      } else {
        clearBlock(ctx, x, y)
      }
    }
  }
  window.requestAnimationFrame(drawLoop)
}

window.addEventListener('keydown', (e) => {
  switch (e.code) {
    case 'ArrowUp':
    case 'KeyW':
      rotate()
      break
    case 'ArrowLeft':
    case 'KeyA':
      move(Movement.LEFT)
      break
    case 'ArrowRight':
    case 'KeyD':
      move(Movement.RIGHT)
      break
    case 'ArrowDown':
    case 'KeyS':
      hardDrop()
      break
    default:
  }
})

drawLoop()
