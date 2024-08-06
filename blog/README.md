# Notris: a Tetris clone for the PlayStation 1

Notris is a modern homebrew PSX game written in C using modern tooling. It's fully playable on original hardware and is
powered by [PSNoobSDK](https://github.com/Lameguy64/PSn00bSDK).

(picture)

- [Skip introduction / history]
- [Go to code]

## Why write a PSX game in 2024?

Last year I got my hands on a rare, black PlayStation 1. This is called a Net Yaroze and is a special console that can
play homebrew games as well as ordinary PSX titles. It was part of a special Sony project to get hobbyists and students
into the games industry.

<p align="center">
  <img src="yaroze.jpg" width=50% height=50% alt="a Net Yaroze console">
</p>

Yaroze games were very limited, as Sony didn't want bedroom coders competing with commercial developers. They could only
be played on other Yarozes or on [special demo discs](https://www.breck-mckye.com/blog/2016/11/net-yaroze/). They had to
fit entirely within system RAM without access to the CD-ROM. Despite these limitations, the Yaroze fostered a passionate
community of indie developers.

And now I had my own. It got me thinking: what was it actually like, to write a PlayStation game?

This is about how I wrote a simple homebrew PSX game myself, using an open-source version of the libraries but still
running on original hardware and written in classic C.

## PlayStation development in the 90s

PSX games were typically written in C on Windows 9X workstations. The official devkit was a pair of ISA expansion cards
that slotted into a common IBM PC motherboard and contained the entire PSX system chipset, video out, and extra RAM (8mb
instead of 2mb). This provided TTY and debugger output to the host machine.

![DTL devkit](devkit.jpg)

You might have heard about blue PlayStations. These were for QA rather than development and are identical to retail
units except that they can play burned CD-ROMs. However, at least one company sold a special addon to convert them into
devkits:

![Blue devkit](devkit-blue.jpg)

The design was very developer-friendly. You could play your game on CRT with normal controllers whilst stepping through
GDB breakpoints on your Windows 95 PC, leafing through a thick textbook of C SDK functions.

In principle, a PSX developer could work entirely in C. The SDK comprised a set of C libraries called PSY-Q, and
included a compiler program `ccpsx` that was really just a frontend over GCC. This supported a range of optimisations, 
like code inlining and loop unrolling, although performance critical sections still warranted hand-optimised assembly.

(You can read about those optimisation in [these SCEE conference slides](https://psx.arthus.net/sdk/Psy-Q/DOCS/CONF/SCEE/96April/optimize.pdf)).

![Example optimisations](optimise.png)

C++ was supported by `ccpsx` but had a reputation for generating 'bloated' code, as well as slower compile times.
Really, C was the lingua franca of PSX development, but some projects made use of dynamic scripting languages on top of 
a base engine. _Metal Gear Solid_ uses TCL for level scripting, and the _Final Fantasy_ games go a step further and
actually have their own bytecode languages for battles, field and minigame systems. (Learn more about this [here](https://youtu.be/S-8PVydb9CM?si=oU0Rqy6bsd0EVq_F)).

_Further reading: https://www.retroreversing.com/official-playStation-devkit_

## Writing my own PSX game

But I came to this from a very different perspective: a software engineer in 2024 who mostly worked on web applications.
My professional experience had almost exclusively been in high level languages like JavaScript and Haskell; I'd done a
little OpenGL work and C++, but modern C++ is practically a different language these days.

I knew PSX SDKs existed for languages like Rust, but I wanted to experience the flavour of 'real' PSX programming, the 
way it had been done back in the 90s. So it would be modern toolchains and open source libraries, but C all the way
through.

I decided to break the project down into three steps:

1. Create a prototype in a high level language
2. Port the code into C for a desktop game
3. Pick a PSX toolchain, and port to PlayStation

The game needed to be something 2D that could be prototyped in a couple of days. I settled for a Tetris clone - I 
figured that would be complex _enough_ to experience what I wanted to experience.

## Prototyping in a high level language

(Picture)

Prototyping in a familiar language would allow me to quickly iterate my design. I chose JavaScript: it's simple, easy 
to debug, and has the HTML5 `<canvas>` graphics API.

The game came together fast and I had fun tweaking its mechanics. At the same time, I was wary that I'd struggle to port
the code into C if I used too many high-level language features like object orientation or functional programming. So
I restricted myself to a very simple, procedural style that would carry across well.

## Learning C

I really had an ulterior motive taking on this project: it was an excuse to finally learn C. The language loomed
large in my mind and I'd begun to develop an inferiority complex over not knowing it.

C has an intimidating reputation and I feared horror stories of dangling pointers, misaligned reads and the dreaded
`segmentation fault`. More precisely: I was worried that if I tried to learn C, and failed, I'd discover
that I wasn't actually a very good programmer after all.

To keep things simple I decided I'd use [SDL2](http://www.libsdl.org/) to handle the input and graphics, and compile for
MacOS to keep debugging as quick as possible. MacOS ships with a workable C toolchain and memory leak analysis.

Despite my fears, I found C incredibly fun. Very quickly it 'clicked' for me. You start from very simple primitives - 
structs, chars, functions - and build them up into layers of abstraction to eventually find yourself in an entire 
working system. Learning C made me rethink a lot of my values as a programmer, coming from a more functional
background.

(Picture)

"Notris" only took a few days to port, and I was very satisfied with my first C project. Plus, I hadn't had a single
segfault!

SDL had been a pleasure to work with, but there were a few aspects that required me to allocate memory dynamically.
This would be a no-no on the PlayStation, where the `malloc` provided by the PSX kernel doesn't work properly. And the 
graphics pipeline would be an even bigger leap...

## Hello PSX!

When it comes to PlayStation homebrew there are two major choices for your SDK. Either:

1. The original **Psy-Q** library with a modernised toolchain ("Nugget")
2. The new **PSNoobSDK** which is open source

There are a couple of other options like the C++ **Psy-Qo**, and you can even forgo any SDK just to do memory-mapped I/O
yourself - but that would be quite ambitious for a first project.

The biggest issue with Psy-Q is that it's still Sony proprietary code, even 30 years later. Legally any homebrew built
with it is at risk. This is what sunk the [Portal64](https://github.com/Valkirie/portal64) demake: it statically linked `libultra`, which is Nintendo's
proprietary N64 SDK.

But to be honest the main reason I chose [PSNoobSDK](https://github.com/Lameguy64/PSn00bSDK) was that it's very well documented and has an almost identical
API anyway. For some functions I was actually able to just use the reference manuals that came with my Yaroze. If me
using a non-authentic SDK offends the PSX purist in you, feel free to quit reading in disgust.

![PSNoobSDK](psnoob.png)

My first task was a kind of hello world: two squares on a coloured background. Sounds simple, right?

### A primer on PSX graphics

(*Some of this is simplified. For a more authoritative guide hit the [PSNoobSDK tutorial](http://lameguy64.net/tutorials/pstutorials/chapter1/1-display.html))

Think of the PSX VRAM as a big 1024 by 512 canvas of 16-bit pixels. Altogether it makes 1 megabyte of memory shared by
framebuffers and textures. We can choose the resolution of the output framebuffer - even 640x480 pixels if we desire -
but more resolution = fewer textures.

![The PSX VRAM model](psx-vram.png)

Most PSOne games (and games generally) have a notion of dual-buffered rendering: whilst one frame is being prepared, the
other one is sent to screen. So we need to allocate two frame buffers:

![PSX VRAM allocation](psx-vram-allocation.png)

(Now you can see why 640x480 isn't practical - there isn't enough space for 2x480p buffers. But this mode CAN be used
by things like the PSX startup logo, which doesn't need any animation)

The buffers (referred to alternately as display and draw environments) are swapped every frame. Most PSX games target 
30fps (in North America) but the actual VSync interrupt comes at 60hz. (Tekken 3, rather impressively, runs at full
60fps for an authentic arcade experience)

![Buffer swapping](buffer-swap.png)

But how does the drawing process work? This is done by the GPU, but the PSX GPU works very differently to a modern
graphics card. Essentially, every frame the DMA transfers **an entire linked list** from RAM that contains a table of
"packets" or commands describing 2D primitives to be drawn.

The GPU does not do 3D transformations; that is the job of the GTE (Geometry Transform Engine) coprocessor. It just
processes these tables of primitives, called 'ordering tables' for reasons we'll get into.

That means the path of a PSX pixel goes as follows:

1. The program on the CPU creates the primitive;
2. (Optionally) the GTE is triggered to do some 3D stuff and write back to the coprocessor registers;
3. These primitives are inserted into a linked list in RAM
4. The program signals the DMA to transfer the linked list to the GPU
5. The GPU gets busy turning these primitives into VRAM pixels (rasterization)
6. The framebuffers are swapped and the raster is to be displayed
7. Video output hardware scans the lines into a video signal

So in pseudocode the PSX frame loop goes like this

```
FrameBuffer 0, 1
OrderingTable 0, 1

id = 1 // flips every frame

loop {
  // Game logic, check controllers, etc.

  // Construct the graphics
  MakeGraphicsCommands(OrderingTable[id])
  
  // Wait for last draw to finish; wait for vertical blank
  DrawSync()
  VSync()
  
  // Other frame has finished drawing in background, display it
  SetDisplay(Framebuffer[!id])
  
  // Start drawing current frame
  SetDrawing(Framebuffer[id])
  StartDrawing(OrderingTable[id])

  // Flip
  id = !id
}
```

You can see from this that whilst frame 1 is on-screen, frame 2 is still being painted, and frame 3 is potentially still
being 'constructed' by the program itself. Then after DrawSync / VSync we send frame 2 to the TV, and get the GPU
drawing frame 3.

![Graphics cycle](gfx-cycle.png)

### Ordering tables and z-indexes

The GPU is a completely 2D piece of hardware, it doesn't know about z-coordinates in 3D space. There is no z-buffer to
describe occlusion - i.e. which objects are in front of others. Instead, the GPU relies on the table being **ordered**
so that objects furthest from the camera are at the end. Then it traverses the linked list in reverse order to implement
the **painter's algorithm**.

[!Diagram of ordering table traversal](ordering-table-traversal.png)

This approach isn't perfect and sometimes PSX geometry shows weird clipping, because each poly can only be at a single
'z index' in screen space, but it works well enough for most games. And this is why it's called an **ordering table**.

### Taking control

SOME JOYPAD IMAGE


how pads work
no input filtering

### Back to our hello world

Okay. So our first task was just a simple program: two squares on a screen. One moves around automatically, the other is
controlled by joypad. Here's what the end result looks like - not much of a 'game', but it puts together graphics,
logic and controls.

![My hello world game](hello-world.png)

The code for this lives in [`hello-psx/main.c`](../hello-psx/main.c). There are a lot of comments but do read through
them if you're looking for a deeper understanding of how things fit together.


## Making a UI...

I had all key pieces prototyped. now I just needed to combine them to implement my tetris clone

first step is the text and overall UI. this had been easy in SDL where I could just load in a front. the psx would be
much more hands on

there is a debug font, but it's not very good. instead I created a texture using the TIM format and timtool. a few
details about this

(Picture)

I also wanted a 'frame' for the play pieces to fall into. I wanted this to be more colourful than a plain white square
the psx supports nice color gradients. I made each corner the colour of a face button

(Picture)

I then started drawing the Tetronimos. These are made of multiple triangles and a quad. On an upscaling emulator they
look great. The actual rotations are hardcoded, where I treat 16 bit uints like a grid of 4x4 pixels. Then 4 uints 
encode all the rotations

on an upscaling emulator they look fantastic

(Picture, 1x resolution and 4x resolution)

## Something breaks

Testing tetronimos... black screen

(Picture)

Break down the problem - only when drawing many tetronimos
hunch: could this be to do with the primitives buffer?

(visualise how this would fail)

testing with printfs
fixing

(picture)

## Putting it together

I had the graphics and io, what about the logic
this was very easy to port. able to use a modern compiler with the same pragmas. not using vlas etc

one difference to know, is there is no malloc. psx malloc is broken. not much impact for me, as I'd deliberately
avoided dynamic memory

### Adding the SCEA logo

license data

(Picture)

### Emulator check

runs on emulator, 60fps even with speed restrictions. looks great with upscaling on

(Picture)

What would happen on an actual console?

## The moment of truth

this started with the yaroze purchase, but I wouldn't actually be playing this on yaroze
I wanted to actually compile a full, retail-equivalent playstation game, and run it on a grey console
Having lost my original PSX many years ago I scoured eBay for a model with the ability to play backup discs

burning PSX games is quite tricky these days. oxides, reflectivity, drives. It took some jiggery pokery to get my 
grey playstation finally booting a backup disc. The only way I could get the console to play my games was if I held it
on its side. Good enough

...
