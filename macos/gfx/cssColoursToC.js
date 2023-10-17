// Helps with generating hardcoded SDL_Color structs
// Run on https://npm.runkit.com/color

var color = require("color")

const cols = {
  BLUE: '#003dcd',
  GREEN: '#00cd37',
  RED: '#cd0030',
  ORANGE: '#df8500',
  PURPLE: '#8900cd',
  TURQUOISE: '#00cdc3',
  YELLOW: '#d8d200'
}

const asc = (hex) => `{0x${
  hex.slice(1, 3).toUpperCase()
}, 0x${
  hex.slice(3, 5).toUpperCase()
}, 0x${
  hex.slice(5, 7).toUpperCase()
}, 0xFF}`;

for (const COLOR of Object.keys(cols)) {
  let hex = cols[COLOR];
  let main = color(hex).hex();
  let lite = color(hex).lighten(0.3).hex();
  let dark = color(hex).darken(0.3).hex();

  console.log('-- COLOR --' + COLOR)
  console.log('MAIN: ' + asc(main))
  console.log('LITE: ' + asc(lite))
  console.log('DARK: ' + asc(dark))
  console.log('-----------')
}