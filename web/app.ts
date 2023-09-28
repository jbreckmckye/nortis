import { getDrawState, getLevelSpeed, getState, actionHardDrop, actionMoveHorizontal, actionRestart, actionRotate, actionSoftDrop } from './game'
import { Movement, Phase } from './types'
import { clearBlock, drawBlock } from './gfx'

const lines = document.querySelector('#lines')
const score = document.querySelector('#score')
const gameover = document.querySelector('#gameover')

const canvas = document.querySelector('#game') as HTMLCanvasElement
const ctx = canvas.getContext('2d')

/**
 * Binding
 * ============================================================================
 *
 * Sets up render loop, listens to inputs, updates game state on regular ticks
 */

drawLoop()
tick()

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

  const state = getState()
  lines.textContent = state.lines.toString(10)
  score.textContent = state.score.toString(10)

  window.requestAnimationFrame(drawLoop)
}

function tick() {
  if (getState().phase === Phase.PLAYING) {
    actionSoftDrop()
  } else {
    canvas.classList.add('dead')
    gameover.classList.remove('hidden')
  }
  window.setTimeout(tick, getLevelSpeed())
}

window.addEventListener('keydown', (e) => {
  if (getState().phase === Phase.GAMEOVER) {
    switch (e.code) {
      case 'KeyR':
        canvas.classList.remove('dead')
        gameover.classList.add('hidden')
        return actionRestart()
      default:
        return
    }

  } else {
    switch (e.code) {
      case 'ArrowUp':
      case 'KeyW':
        actionRotate()
        break
      case 'ArrowLeft':
      case 'KeyA':
        actionMoveHorizontal(Movement.LEFT)
        break
      case 'ArrowRight':
      case 'KeyD':
        actionMoveHorizontal(Movement.RIGHT)
        break
      case 'ArrowDown':
      case 'KeyS':
        actionHardDrop()
        break
      default:
    }
  }
})


