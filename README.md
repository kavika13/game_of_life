# Conway's Game of Life

A project I built over the course of a week in July 2016. I started it for an interview, and later augmented for fun :)

## What is this thing?

It is sort of like a zero-player video game. Many people have collaborated over the years to learn interesting things from it (and are still doing so).

More accurately, it is a "cellular automaton", invented in 1970 by John Horton Conway. It holds interest in the fields of computer graphics, mathematics, and [Computability Theory](https://en.wikipedia.org/wiki/Computability_theory), as well as other fields.

See these resources for way more information than you ever wanted to know about this:

- https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
- http://www.conwaylife.com/
- http://www.conwaylife.com/wiki/Main_Page

## Program Usage

- Download a build for your platform from https://github.com/kavika13/game_of_life/releases
- Extract the archive to a folder
- Run the app
- Enjoy!

Instructions for Hotkeys are visible when you run the application. To see them again once they disappear, hit the <kbd>H</kbd> or <kbd>?</kbd> key.

To change the initial state of the cells, edit "input.txt". This is a little harder on OSX, since you have to crack open the application bundle.

## Input format

TL;DR - Here's an example:

    # A "glider"
    (0, 0)
    (1, 0)
    (2, 0)
    (2, 1)
    (1, 2) // Cool beans!
    // Note that blank lines are okay, as are a few types of comments

    (5, 5) ; A spinner
    (6, 5)
    (7, 5)

Cells must be input one line at a time, and must have this format:

    (x-value, y-value)

The x value and y value can be anywhere in the range of an unsigned 64-bit integer, and the simulation will deal with it. The simulation will wrap at 64-bit integer boundaries.

If you want the visualization to work properly, constrain these to values that are representable using single-precision floating point numbers (since OpenGL &lt; 4.0 can&apos;t handle anything but this).

You are free to add blank lines, and can add comments after each cell, or on a line by themselves.

Allowed comment characters are: `#`, `//`, `;`

## Build

### For Visual Studio

- Install Visual C++ 2015 (must be 64 bit version - not sure if that's default?)
- Download SFML 2.3.2, extract it, and drop it in build folder as SFML-2.3.2
- Open solution
- Pick X64 build target
- Build the solution

### For XCode

- Install XCode 7
- Download SFML 2.3.2, and extract it. The location doesn't matter for XCode builds, since we're linking to Frameworks
- Copy the contents of Frameworks and extlibs to /Library/Frameworks (See: http://www.sfml-dev.org/tutorials/2.3/start-osx.php)
- Open the project
- Build the project

(Note: I plan to get a true cross-platform/cross-target build system working once I've researched and picked one)

## Techmologiez

- http://www.sfml-dev.org/ - This made the UI portion so painless.

## License

Do not copy or distribute.

Please contact me if you're interested in me getting this license to be more permissive.
