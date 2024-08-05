# Notris: a Tetris clone for the PlayStation 1

Notris is a modern homebrew PSX game written in C using modern tooling. It's fully playable on original hardware and is
powered by [PSNoobSDK](https://github.com/Lameguy64/PSn00bSDK).

(picture)

Quick links:
- [Notris code + build instructions]
- [Skip past introduction to "Hello PSX"]

## Why write a PSX game in 2024?

Last year I got my hands on a rare, black PlayStation 1. This is called a Net Yaroze and is a special console that can
play homebrew games as well as ordinary PSX titles. It was part of a special Sony project to get hobbyists and students
into the games industry.

<p align="center">
  <img src="yaroze.jpg" width=50% height=50% alt="a Net Yaroze console">
</p>

Yaroze games were very limited, as Sony didn't want bedroom coders competing with commercial developers. They could only
be played on other Yarozes or as part of special demo discs (info about those
[here](https://www.breck-mckye.com/blog/2016/11/net-yaroze/)). The games had to fit entirely within system RAM without
access to the CD-ROM or memory card. Despite these limitations, the Yaroze fostered a passionate online community of
indie game developers.

And now I finally had my own. It got me thinking: what was it actually like, to write a PlayStation game? Could I write my own?

This is about how I wrote a simple homebrew PSX game myself, using an open-source version of the libraries but still running on original hardware and written in classic C.

## PlayStation development in the 90s

PSX games were typically written in C on Windows 9X workstations. The official devkit was a pair of ISA expansion cards
that slotted into a common IBM PC motherboard and contained the entire PSX system chipset, with extra RAM (8mb instead
of 2mb), and TTY + debugger output to the host machine.

![DTL devkit](devkit.jpg)

You might have heard about blue PlayStations. These were for QA rather than development and their only
major feature is that they can play burned CD-ROMs without anti-piracy checks. However, at
least one company sold a special kit to convert QA systems into devkits:

![Blue devkit](devkit-blue.jpg)

The design was very developer-friendly. You could play your game on CRT with normal controllers whilst stepping through
GDB breakpoints on your Windows 95 PC, leafing through a thick textbook of C SDK functions.

In principle, a PSX developer could work entirely in C. The SDK comprised a set of C libraries called PSY-Q, and
included a compiler program `ccpsx` that was really just a frontend over GCC. This supported a range of optimisations, 
like code inlining and loop unrolling, although performance critical sections still warranted hand-optimised assembly.

(You can read about some of Sony's recommended optimisation in [these SCEE
conference slides](https://psx.arthus.net/sdk/Psy-Q/DOCS/CONF/SCEE/96April/optimize.pdf)).

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

Working in a familiar language would allow me to get to grips with the overall design and logic, independently of learning C. I chose JavaScript: it's simple, easy to debug, and has the HTML5 `<canvas>` graphics API.

The prototype came together fast and I had fun tweaking the mechanics of my game. At the same time, I was wary that if I used high-level language features like object-orientation or functional programming,
I'd struggle to port the code to plain old C. So I restricted myself to a subset of JavaScript: simple structs, plain loops, and basic functions.

## Learning C

I really had an ulterior motive taking on this project: it was an excuse to finally learn C. The language loomed
large in my mind and I'd begun to develop an inferiority complex over not knowing it.

C has an intimidating reputation and I feared horror stories of dangling pointers, misaligned reads and the dreaded
`segmentation fault`. More precisely: I was worried that if I tried to learn C, and failed, I'd discover
that I wasn't actually a very good programmer after all.

Like most software development problems, I just needed to it down into smaller steps:

- with a simple C graphics library, I could focus on the logic
- if I built for desktop, I could iterate fast
- once the desktop version worked, I could tackle the PSX specifics individually

The framework I went for was [SDL2](http://www.libsdl.org/). Maybe there are fancier libraries, or newer
ones, but this seemed _good enough_. I needed to stop procrastinating and write... my first C
program!

Despite my fears, I found C incredibly fun. Very quickly it 'clicked' for me. You start from very simple primitives - 
structs, chars, functions - and build them up into layers of abstraction to eventually find yourself in an entire 
working system. You do need to be disciplined to avoid certain footguns - it's not a language that tolerates distracted
programming - but there is something incredibly empowering, when you feel you built every part of your system top to
bottom.

(Picture)

"Notris" only took a few days to port, and I was very satisfied with my first C project. Plus, I hadn't had a single
segfault!

SDL had been a pleasure to work with, but there were a few aspects that required me to allocate memory dynamically.
This would be a no-no on the PlayStation, where the `malloc` provided by the PSX kernel doesn't work properly. And the 
graphics pipeline would be an even bigger leap...

## Hello world on PSX

When it comes to PlayStation SDKs there are two major options: you can either go with the original Psy-Q library with modernised toolchain ("Nugget"), or the open source PSNoobSDK.

It's actually quite plausible to work without any SDK at all - they are just wrappers for memory-mapped I/O - but that might be a bit ambitious for a first project.

The biggest issue with Psy-Q is that it's Sony proprietary code. Legally that puts any homebrew at risk. This was the same issue the recent [Portal64](https://github.com/Valkirie/portal64) demake fell foul of, by statically linking Nintendo's LibUltra code.

I ended up selecting [PSNoobSDK](https://github.com/Lameguy64/PSn00bSDK): it has a very similar API to PsyQ, but is legally safer and has very approachable documentation. Feel free to quit reading in disgust if that offends the PSX purist in you.

![PSNoobSDK](psnoob.png)

My first task was a kind of hello world: two bouncing squares on a coloured background.

### A primer on PSX graphics

dual buffer & memory mapping
unified memory model

graphics pipeline is quite different

every time you want to draw, you hand the GPU a linked list of pointers to 'primitives' / graphics commands. It traverses
this in reverse order, from the end of the list to the start. Therefore shapes are drawn from highest z coordinate (furthest
away) to lowest, implementing the painters algorithm

![Graphics cycle](gfx-cycle.png)

This work has to be synchronised with screen draws - vsync

The gpu can do this at the same time the CPU is working. therefore psx games generally have two primitives buffers and
two ordering tables, building one whilst drawing the other

### Taking control

how pads work
no input filtering

### Milestone: bouncing squares

getting to point of simple graphics and controls

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
