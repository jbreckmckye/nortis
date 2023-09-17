export const colours = {
  blue: '#003dcd',
  green: '#00cd37',
  red: '#cd0030',
  orange: '#cd8500',
  purple: '#8900cd',
  turquoise: '#00cdc3',
  grey: '#adadad'
}

export type Colour = keyof typeof colours

export const bright = Object.entries(colours).reduce(
  (acc, [key, value]) => {
    acc[key] = alter(colours[key], 32)
    console.log('brighter', key, value, acc[key])
    return acc
  },
  {} as Record<Colour, string>
)

export const dark = Object.entries(colours).reduce(
  (acc, [key, value]) => {
    acc[key] = alter(colours[key], -32)
    return acc
  },
  {} as Record<Colour, string>
)

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
