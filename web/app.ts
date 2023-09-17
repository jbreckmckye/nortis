import { drawBlock } from './tatris/gfx'

const canvas = document.getElementById('game') as HTMLCanvasElement
const ctx = canvas.getContext('2d')

drawBlock(ctx, 8, 13, 'red')
drawBlock(ctx, 9, 13, 'red')
drawBlock(ctx, 8, 14, 'red')
drawBlock(ctx, 9, 14, 'red')
