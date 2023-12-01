# Notris, a game for web, MacOS and PSOne

Tetris-like game for multiple platforms.

The following notes will be written up into a blog post...

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

Explainer for the gcc flags

- subshell for `sdl2-config --libs` generates handy linker arguments `-L/usr/local/lib -lSDL2`
- `-lm` links the math library
- `lX` links a library by name

Set `MallocStackLogging=1` to use `leaks` cmd

## PlayStation 1

### Setting up dev environment

Compiler etc.
- working on Windows (better emulation options)
- my emulator of choice is Duckstation. It provides some debugging capabilities. The PSX community often recommends no$psx as a dev emulator, but I couldn't get this working
  with my builds.

### `PSn00bSDK`

Rewrite of PSY-Q
Seemed to have the easiest setup & very thorough docs

Requires cmake https://cmake.org/download (I used 3.27.8)
Rest of instructions from https://github.com/Lameguy64/PSn00bSDK
- download binaries for platform, releases here: https://github.com/Lameguy64/PSn00bSDK/releases - I used v0.24
  - downloaded gcc-mipsel-none-elf-12.3.0-windows.zip - this is the MIPS toolchain. Added its /bin to my PATH
  - same dealy with PSn00bSDK-0.24-win32.zip 
  - can check if mkpsxiso (from SDK) and mipsel-none-elf-size (from MIPS toolchain) are recognised on CMD 
- set env var `PSN00BSDK_LIBS` to the libs folder e.g. C:/PSn00bSDK/lib/libpsn00b. Check you can echo `echo %PSN00BSDK_LIBS%` and if not restart shell
- now can invoke compiler with `mipsel-none-elf-gcc`

Then can create project as per https://github.com/Lameguy64/PSn00bSDK/blob/master/doc/installation.md#creating-a-project
Copy template from share\psn00bSDK\template into a new repo
Then call
```
cmake --preset default .
cmake --build ./build
```

To rebuild, clear build dir and re-run commands above

For VSCode, configure the `includePath` in the C/C++ plugin to include `C:\PSn00bSDK/include/libpsn00b` - this enables intellisense for PSX headers

### Showing the PSX logo

Classic PSX games boot in two stages: first they show the Sony Computer Entertainment screen, then the classic PSX logo. But our example project doesn't. Why is that?

Well, the SCEE splash comes from the BIOS, as does the PSX boot sound, but the logo is actually part of the disc license data. It's there to act like a 'seal of authenticity'
in effect.

MKPSXISO supports loading in a license file (i.e. extracted from a retail game via DUMPSXISO), but for the sake of copyright we have to gitignore it.

If we rebuild with a license_data.dat we get the classic splash screen

### Building on windows

Because PSn00bSDK targets windows (and I'm using a windows emulator), I do my scripting for PSX projects with PowerShell. What's the extension of a powershell script?
Why, it's `.ps1`, of course!

There's a `build.ps1` file added to the project that can be executed by right clicking in Explorer
