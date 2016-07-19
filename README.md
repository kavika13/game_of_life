#Conway's Game of Life

Implemented for an interview, later augmented for fun :)

##Build

###For Visual Studio

- Install Visual C++ 2015 (must be 64 bit version - not sure if that's default?)
- Download SFML 2.3.2, extract it, and drop it in build folder as SFML-2.3.2
- Open solution
- Pick X64 build target
- Build the solution

###For XCode

- Install XCode 7
- Download SFML 2.3.2, and extract it. The location doesn't matter for XCode builds, since we're linking to Frameworks
- Copy the contents of Frameworks and extlibs to /Library/Frameworks (See: http://www.sfml-dev.org/tutorials/2.3/start-osx.php)
- Open the project
- Build the project

(Note: I plan to get a true cross-platform/cross-target build system working once I've researched and picked one)

##License

Do not copy or distribute.
