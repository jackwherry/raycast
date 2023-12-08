# Determine operating system
UNAME_S = $(shell uname -s)

# Use system clang
CC=clang

# Compiler flags
CFLAGS = -I. -O2 -g -std=gnu17 -Wall -Wextra -Wfloat-equal -Wundef 
CFLAGS += -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes
CFLAGS += -Wwrite-strings -Waggregate-return -Wcast-qual $(CCINCLUDES)
CFLAGS += -fsanitize=address # may impact performance slightly, significantly increases memory footprint

# macOS library stuff
ifeq ($(UNAME_S), Darwin)
	# Outside includes (separate from LDFLAGS to avoid an unused flag warning)
	CCINCLUDES = -F/Library/Frameworks -I/Library/Frameworks/SDL2.framework/Headers

	# Linker flags
	#	(note: you must have SDL2 installed to /Library/Frameworks)
	LDFLAGS = -framework SDL2 -F/Library/Frameworks -I/Library/Frameworks/SDL2.framework/Headers
endif

# Linux library stuff (untested!)
ifeq ($(UNAME_S), Linux)
	# Outside includes (separate from LDFLAGS to avoid an unused flag warning)
	CCINCLUDES = -I/usr/include/SDL2 -D_REENTRANT

	# Linker flags
	#	(note: you must have SDL2 installed to /Library/Frameworks)
	LDFLAGS = -lSDL2
endif

# .h files go here
INCLUDES = config.h nuklear.h nuklear_sdl_renderer.h cJSON.h

# .o files go here
OBJ = main.o cJSON.o

# Generate all the .o files
%.o: %.c $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

# Link and create the ./raycast executable
raycast: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

# Don't do weird stuff if there's a file called clean
.PHONY: clean

clean:
	rm *.o raycast
