# Raycast
Project goal: game (or at least game engine) that uses raycasting

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