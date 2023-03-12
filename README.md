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
Modify the Makefile to use the correct compiler and linker flags. Put it behind an `ifeq` and submit a pull request if you're so inclined :)

### Windows
The Makefile might work with MinGW or something similarly to how it does on Linux. You could also just ignore it and use the Visual Studio build system instead. 

## Possible TODOs
- Collision handling
- Texturing
- Better lighting
- Level editing
- More assertions/robustness improvements
	- ✅ add error when the number of walls doesn't make sense with respect to the info in the segments section
	- ✅ make sure that the total number of walls and sectors don't exceed the size of the arrays
		- or switch to a custom arraylist implementation here instead
	- ✅ cross platform support
- Changing EYE_Z based on floor height
- Sprites
- Mobs
- HUD
	- x/y coords displayed on screen
