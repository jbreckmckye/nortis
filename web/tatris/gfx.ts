import { bright, Colour, colours, dark } from './colours'

const PX = 24


function clearBlock(ctx: CanvasRenderingContext2D, x: number, y: number) {
  ctx.clearRect(x * PX, y * PX, PX, PX)
}

export function drawBlock(ctx: CanvasRenderingContext2D, x: number, y: number, colour: Colour) {
  const x0 = x * PX
  const x1 = x0 + PX - 1
  const y0 = y * PX
  const y1 = y0 + PX - 1


  ctx.fillStyle = colours[colour]
  ctx.fillRect(x0, y0, PX, PX)

  ctx.fillStyle = bright[colour]
  ctx.fillRect(x0, y0, 1, PX - 1)
  ctx.fillRect(x0, y0, PX - 1, 1)

  ctx.fillStyle = dark[colour]
  ctx.fillRect(x1, y0, 1, PX)
  ctx.fillRect(x0, y1, PX, 1)
}

