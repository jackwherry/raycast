# Raycast
Walk around a 3D world rendered with raycasting

![Game screenshot](/screenshot.png?raw=true "debug and map editor windows open")

Features:
* Arbitrary wall geometry; not restricted to boxes
* Sector and portal-based rendering with arbitrary floor and ceiling heights
* Simple wall collision detection
* Level files specified in JSON
* Immediate mode GUI overlay (using [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear))
* Level/map editor; modify map geometry while the game is running
* Visual effects with color and gradients
* Runs at 60 frames per second using software rendering (even on a 12-year-old ThinkPad X220)

Inspired by https://www.youtube.com/watch?v=fSjc8vLMg8c and https://www.youtube.com/watch?v=HQYsFshbkYw. Some renderer code and general structure modified from/insipired by [this demo](https://github.com/jdah/doomenstein-3d/blob/main/src/main_doom.c).

## Building and running
### macOS
Install SDL2 by copying the most recent DMG from [here](https://github.com/libsdl-org/SDL/releases) to /Library/Frameworks.

```sh
$ make
$ ./raycast
```

You may be able to use the Xcode build system if you'd prefer.

### Linux
Install SDL2 with your distribution's package manager (or from source) and run `make`. You may need to modify the include or link flags in the Makefile.

### Windows
The Makefile might work with MinGW similarly to how it does on Linux. You could also use the Visual Studio build system instead.

## Third-party libraries used
- [SDL](https://www.libsdl.org)
- [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) and [nuklear_sdl_renderer](https://github.com/Immediate-Mode-UI/Nuklear/blob/master/demo/sdl_renderer/nuklear_sdl_renderer.h)
- [cJSON](https://github.com/DaveGamble/cJSON)

I chose to include Nuklear and cJSON alongside my source code in this repository, so you won't need to download those in order to build this project. SDL is an external dependency. 

<!--
## Possible next steps
- 🌓 Collision handling
	- ✅ Basic wall-player collisions
	- Arbitrary collisions
- Texturing
- Better lighting
- 🌗 Level editing
	- ✅ Nuklear immediate-mode GUI library
	- ✅ x/y coords displayed on screen (or integrate in level editor)
	- ✅ Edit properties of existing walls/sectors
	- ✅ Add new walls/sectors
	- Save new level files to disk
	- Edit levels visually (top-down view?)
- More assertions/robustness improvements
	- ✅ make sure that the total number of walls and sectors don't exceed the size of the arrays
		- or switch to a custom arraylist implementation here instead
	- ✅ cross platform support
- Changing EYE_Z based on floor height
	- the naive solution doesn't work here
- Billboard sprites
- Wall decals
- Mobs
- HUD
- Procedural generation
-->
