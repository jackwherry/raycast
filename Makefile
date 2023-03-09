# Use system clang
CC=clang

# Add these flags to compiler commands
CFLAGS = -I. -O2 -g -std=gnu17 -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wwrite-strings -Waggregate-return -Wcast-qual

# Look for third-party libraries in lib/
LDIR=lib

# Include these libraries (for now, the math library)
LIBS=-lm

# .h files written by me go here
INCLUDES = raycast.h

# .o files go here
OBJ = main.o util.o

# Generate all the .o files
%.o: %.c $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS)

# Link and create the ./raycast executable
raycast: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# Don't do weird stuff if there's a file called clean
.PHONY: clean

clean:
	rm *.o raycast