Run:

`yarn build-macos && yarn run-macos`

We're using SDL2 with the TTF font extension. These need to be installed on the system:

```shell
brew install sdl2 sdl2_ttf
```

Build command is

```shell
gcc hello.c `sdl2-config --libs` -lm -lSDL2_ttf
```

Explainer for the gcc flags

- subshell for `sdl2-config --libs` generates handy linker arguments `-L/usr/local/lib -lSDL2`
- `-lm` links the math library
- `lX` links a library by name

Set `MallocStackLogging=1` to use `leaks` cmd