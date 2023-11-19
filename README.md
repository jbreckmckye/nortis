# Notris, a game for web, MacOS and PSOne

Tetris-like game for multiple platforms.

Progress:

- ✅ Web with `<canvas>`
- ✅ MacOS with C and SDL2
- ⏳ PlayStation 1 with C

## JavaScript with canvas

`yarn dev-web`

The first implementation is a prototype in a high-level language. Working in JavaScript means we can quickly build an
idea of the overall approach, before moving to C and the more involved SDKs.

Although we're working in JS, we avoid availing ourselves of many high level language features like closures, higher
order functions, or object orientation. The code is meant to resemble the C code we will write later for MacOS and
PlayStation.

## MacOS with C and SDL2

`yarn build-macos && yarn run-macos`

This should compile for windows / Linux too, but I haven't tried it.

We're using SDL2 with the TTF font extension. These need to be installed on the system:

```shell
brew install sdl2 sdl2_ttf
```

Build command is 

```shell
gcc hello.c `sdl2-config --libs` -lm -lSDL2_ttf
```

- subshell for `sdl2-config --libs` generates handy linker arguments `-L/usr/local/lib -lSDL2`
- `-lm` links the math library
- `lX` links a library by name

Set `MallocStackLogging=1` to use `leaks` cmd

## PlayStation 1

Currently work in progress; setting up a PSX dev environment
