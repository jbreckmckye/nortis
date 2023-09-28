# Notris

Tetris-like game for multiple platforms.

TODO

- web
- MacOS with C
- PlayStation with C

## JavaScript with canvas

The first implementation is a prototype in a high-level language. Working in JavaScript means we can quickly build an
idea of the overall approach, before moving to C and the more involved SDKs.

Although we're working in JS, we avoid availing ourselves of many high level language features like closures, higher
order functions, or object orientation. The code is meant to resemble the C code we will write later for MacOS and
PlayStation.

## C with SDL

We're using SDL2 with the TTF font extension. These need to be installed on the system:

```shell
brew install sdl2 sdl2_ttf
```

Notes

I discovered a near-identical project already existed - https://github.com/olzhasar/sdl-tetris. This was a boost, though
I didn't want to rely on it too much.

https://lazyfoo.net/tutorials/SDL/01_hello_SDL/index.php

Build command is 

```shell
gcc hello.c `sdl2-config --libs` -lm -lSDL2_ttf
```

- subshell for `sdl2-config --libs` generates handy linker arguments `-L/usr/local/lib -lSDL2`
- `-lm` links the math library
- `lX` links a library by name
