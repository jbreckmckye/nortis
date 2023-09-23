import { colourTable } from './colours'
import { Colours } from './types'

/**
 * Graphics code
 * ============================================================================
 */

const PX = 24

export function clearBlock(ctx: CanvasRenderingContext2D, x: number, y: number) {
  ctx.clearRect(x * PX, y * PX, PX, PX)
}

/***
 * x = 0..9
 * y = 0..29
 */
export function drawBlock(ctx: CanvasRenderingContext2D, x: number, y: number, colour: Colours) {
  const x0 = x * PX
  const x1 = x0 + PX - 1
  const y0 = y * PX
  const y1 = y0 + PX - 1

  ctx.fillStyle = colourTable[colour].main
  ctx.fillRect(x0, y0, PX, PX)

  ctx.fillStyle = colourTable[colour].light
  ctx.fillRect(x0, y0, 1, PX - 1)
  ctx.fillRect(x0, y0, PX - 1, 1)

  ctx.fillStyle = colourTable[colour].dark
  ctx.fillRect(x1, y0, 1, PX)
  ctx.fillRect(x0, y1, PX, 1)
}
