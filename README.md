# Raycast
Project goal: game (or at least game engine) that uses raycasting

Inspired by https://www.youtube.com/watch?v=fSjc8vLMg8c

## Building and running
### macOS
Install SDL2 by copying the most recent DMG from [here](https://github.com/libsdl-org/SDL/releases) to /Library/Frameworks.

```sh
$ make
$ ./raycast
```

You may be able to use the Xcode build system if you'd prefer.

### Linux
Install SDL2 with your distribution's package manager (or from source) and run `make`. You may need to modify the include or link flags.

### Windows
The Makefile might work with MinGW similarly to how it does on Linux. You could also just ignore it and use the Visual Studio build system instead.

## Third-party libraries used
- [SDL](https://www.libsdl.org)
- [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) and [nuklear_sdl_renderer](https://github.com/Immediate-Mode-UI/Nuklear/blob/master/demo/sdl_renderer/nuklear_sdl_renderer.h)
- [cJSON](https://github.com/DaveGamble/cJSON)

I chose to include Nuklear and cJSON in this repository, so you won't need to download those in order to build this project. The SDL wiki doesn't recommend statically linking it because doing so makes it harder to run old games on newer operating systems where a newer SDL version is needed, so that's the only external dependency. 

## Possible TODOs
- 🌓 Collision handling
	- ✅ Basic wall-player collisions
	- Arbitrary collisions
- Texturing
- Better lighting
- 🌗 Level editing
	- ✅ Nuklear immediate-mode GUI library
	- ✅ x/y coords displayed on screen (or integrate in level editor)
	- Edit properties of existing walls/sectors
	- Add new walls/sectors
	- Save new level files to disk
	- Edit levels visually (top-down view?)
- More assertions/robustness improvements
	- ✅ add error when the number of walls doesn't make sense with respect to the info in the segments section
	- ✅ make sure that the total number of walls and sectors don't exceed the size of the arrays
		- or switch to a custom arraylist implementation here instead
	- ✅ cross platform support
- Changing EYE_Z based on floor height
- Billboard sprites
- Wall decals
- Mobs
- HUD
