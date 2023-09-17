import { Colours } from './types'

export const colourTable = {
  [Colours.BLUE]: colours('#003dcd'),
  [Colours.GREEN]: colours('#00cd37'),
  [Colours.RED]: colours('#cd0030'),
  [Colours.ORANGE]: colours('#df8500'),
  [Colours.PURPLE]: colours('#8900cd'),
  [Colours.TURQUOISE]: colours('#00cdc3'),
  [Colours.YELLOW]: colours('#d8d200')
}

function colours(hexColour: string) {
  return {
    main: hexColour,
    light: alter(hexColour, 32),
    dark: alter(hexColour, -32)
  }
}

function hexToRGB(hexColour: string) {
  const r = parseInt(hexColour.slice(1, 3), 16)
  const g = parseInt(hexColour.slice(3, 5), 16)
  const b = parseInt(hexColour.slice(5, 7), 16)
  return { r, g, b }
}

function alter(hexColour: string, amount: number) {
  const { r, g, b } = hexToRGB(hexColour)
  return `rgba(${addColour(r, amount)}, ${addColour(g, amount)}, ${addColour(b, amount)}, 255)`
}

function addColour(value: number, amount: number) {
  return Math.max(0, Math.min(value + amount, 255))
}
