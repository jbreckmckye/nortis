# Notris: a block puzzle game for PSOne

Notris is a familiar (but legally distinct!) block puzzle game for the PlayStation 1. It's written in C using PSnoobSDK.

This repo contains builds for three platforms:

- ✅ a JavaScript prototype using `<canvas>`
- ✅ a MacOS C port using SDL
- ⏳ a PlayStation 1 game

# Notes (WIP)

The following are rough notes that will be written up into a blog post

## Motives and background

Last year I managed to get my hands on a very rare, black PlayStation 1. This is the Net Yaroze and it allowed ordinary
consumers to write their own, basic PSOne games. Yaroze games were a little limited, they had to fit completely within
system RAM, but there sprung up a small yet dedicated fandom of indie developers writing their own PSOne games in the 
1990s.

But now I had my own, and it got me thinking - what does it take to write a PlayStation game?

This is about how I wrote my own, simple Tetris clone for the PSX, using open-source versions of the PSOne SDK but
still running on original hardware and written in 90s-style C. The game is simple, just a 2D puzzle game without any
audio, but I'm proud I made something real and running on an actual PlayStation. [Note - this is written from a future
perspective : D]

## How did they do it in the 90s?

The Sony PlayStation devkit was basically an expansion card you slotted into your PC. It used the old ISA standard and
the software all ran on Windows 95. All the SDKs (software development kits) were based around the C programming language;
almost every PSOne game was written in C.

Now I came into this project not having ever written any C, and being slightly intimidated by it. Most of my professional
programming experience has been in much higher level languages like JavaScript and Haskell. I'd done a little C++ but
modern C++ is so far from 90s C that they're practically different languages. How would I bridge this gap in my skills?

## Breaking down the problem

I figured I could make this project work by breaking into three smaller problems.

First, I would write a prototype of my game in a language I knew. In this case JavaScript with HTML5 canvas. This would
give me a skeleton of the eventual code.

Second, I would rewrite the game in C, as a MacOS native app. This would be how I'd learn C.

Finally, I would port that game to the PlayStation. I would only need to rewrite the parts specific to the PlayStation,
and not get distracted by the new programming language.

## Part 1: JavaScript with HTML5 canvas

So the first step was a prototype in a high level language. Working in JavaScript means we can move quickly and build
an idea of the overall approach. JS is a much more forgiving language than C.

I'd never actually written a Tetris game and it turns out there is some nuance to it. Tetris jargon is full of terms
like 'wall kicks', 't spins' and other mechanics that all needed to be implemented by my port.

Although I was working in JavaScript, I tried to avoid leaning on any higher level language features, that I knew I
would no longer have working in C. That meant no object orientation, no closures, no functional programming - just 
a very simple and procedural style.

(You can run the prototype in this project with `yarn dev-web`)

## Part 2: MacOS with C and SDL2

I came into this project rather intimidated by C. I'd heard horror stories of dangling pointers, misaligned bytes,
segfaulting reads and Heisenbugs so complex even Dijkstra himself wouldn't untangle them. I really had nothing to
fear. Learning C was incredible fun.

It's a bit of an exaggeration to say that C "teaches you how the computer works" (link: Your Computer Is Not a
Fast PDP-11), but there is definitely a simplicity to its mental model, that I found really compelling.

For example, for my Tetronimos, I needed an easy way to define a grid of blocks for each shape. In JavaScript I
did this with dynamic arrays, but C isn't kind to dynamic programming. But a different way to think about a grid
is as a set of zeroes and ones. And a way to store zeroes and ones in C is by declaring numbers. That meant I
could use numbers to declare the pattern of blocks inside a Tetronimo.

```c
/**
 * blocks.c
 * ================================================================================================
 * Blocks are stored using a hex number scheme described in https://stackoverflow.com/a/38596291,
 * of 16 bits (4 by 4 rows).  This allows us to very compactly define custom rotation systems.
 * 
 * Bits can be addressed by masking from 0x8000 (GRID_BIT_OFFSET) for the first bit, then shifting
 * right for subsequent bits.
 * 
 * Example:
 * 
 * A 2x2 block translated into bits
 * 
 * XX.. -> 1100
 * XX.. -> 1100
 * .... -> 0000
 * .... -> 0000
 * 
 * binary= 0b1100110000000000
 * hexdml= 0xCC00
 * 
 * To get row 1 = 0xCC00 & (0x0F00)
 *        row 2 = 0xCC00 & (0x0F00 >> 1)
 *        etc.
 * 
 * Or to get x/y = 0xCC00 & (0x8000 >> ((y * 4) + x))
 * ================================================================================================
 */

static shapeHex shapeHexes[8][4] = {
  { 0 },                              // EMPTY   
  { 0x0F00, 0x4444, 0x0F00, 0x4444 }, // I
  { 0xE200, 0x44C0, 0x8E00, 0xC880 }, // J
  { 0xE800, 0xC440, 0x2E00, 0x88C0 }, // L
  { 0xCC00, 0xCC00, 0xCC00, 0xCC00 }, // O
  { 0x6C00, 0x8C40, 0x6C00, 0x8C40 }, // S
  { 0x0E40, 0x4C40, 0x4E00, 0x4640 }, // T
  { 0x4C80, 0xC600, 0x4C80, 0xC600 }, // Z
};
```

I liked this aspect of C a lot. You think about ranges of bytes and very basic computer state. From there you
layer up and layer up abstractions until you've built an entire application, right from bare primitives

### Running it yourself

`yarn build-macos && yarn run-macos`

We're using SDL2 with the TTF font extension. These need to be installed on the system:

```shell
brew install sdl2 sdl2_ttf
```

Build command is 

```shell
gcc hello.c `sdl2-config --libs` -lm -lSDL2_ttf
```

Explainer for the gcc flags (as I had to learn)

- subshell for `sdl2-config --libs` generates handy linker arguments `-L/usr/local/lib -lSDL2`
- `-lm` links the math library
- `lX` links a library by name

Set `MallocStackLogging=1` to use `leaks` cmd

Dev notes

- generating tetronimos using numbers
- generating colours using JS

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

### Drawing

Setup and text http://lameguy64.net/tutorials/pstutorials/chapter1/1-display.html
Drawing and gfx http://lameguy64.net/tutorials/pstutorials/chapter1/2-graphics.html

Basic idea is that the 3D maths, projections etc are handled by a coprocessor called the Geometry Transform Engine, before
the CPU sends "packets" to the GPU to actually go and render / rasterise primitive shapes at particular locations.

Packets are arranged in RAM using an ordering table. This initialises to a simple linked list of n pointers. Primitives are added by inserting structs into the linked list

```
IN ARRAY:

[Front of screen] --> 0 --> 1 --> 2 --> [Back of screen]

LINKED:

0 --                -- 1 <--                                -- 2 <-- BEGINS FROM END 
    \- [Shape 1] <-/         \- [Shape 2] <-- [Shape 3] <--/
```

The ordering table is a reverse linked list. In memory it starts with the packets to display at the front of the screen, and ends with those furthest back, so that the
index in the list is analogous to a z-index. However it is processed back-to-front to implement the "painter's algorithm". When the ordering table is ready it is processed
with `DrawOTag()` passing the _last_ element.

Because primitives need to live longer than the functions that declare them, they need to be initialised in a global buffer instead of the stack. (Otherwise functions returning / entering would overwrite the stack memory). The PSX BIOS and SDKs support heap allocations via `malloc` but are buggy and almost never used.

Each primitive can be between 3 to 13 words (each word is 4 bytes as the PSX is 32 bit). Generally the primitive buffers can just be declared as arrays of `char` and then the programmer uses `sizeof` and pointer arithmetic to work with the memory. The SDK I'm using provides several macros to help with setting up primitives.

Generally two ordering tables and primitive buffers are kept in memory, for the same reason two graphics buffers support performance. One set primitives / linked list can be drawn whilst the other is being prepared for the next frame.

### Displaying text

There is a basic debug text utility embedded in the SDK, but it looks very rough and basic. Instead I opted to create a TIM format texture from some PNG pixel art. I wasn't completely happy with how this looked, but I didn't want to focus all my energy on just displaying text.

You can see the debug and TIM fonts in the image below:

![screenshot](psx-font.png)

### Displaying a play piece

todo

### Controller input

todo

### Integraton