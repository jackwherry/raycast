// PROJECT-WIDE INCLUDES 
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <SDL.h>

// CONSTANTS
#define PROJECT_NAME "Raycast"
#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 256

#define MAP_SIZE 8
static uint8_t MAPDATA[MAP_SIZE * MAP_SIZE] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 3, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 2, 0, 4, 4, 0, 1,
	1, 0, 0, 0, 4, 0, 0, 1,
	1, 0, 3, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
};

// TYPEDEFS
typedef struct vect2_s {
	float x, y;
} vect2;

typedef struct vect2i_s {
	int32_t x, y;
} vect2i;

// FUNCTION SIGNATURES
// print a welcome message
void printSetupMessages(void);

// dot product of float vectors
float dot(vect2 v0, vect2 v1);

// length of a float vector
float length(vect2 v);

// normalize a float vector (make it a unit vector)
vect2 normalize(vect2 v);

// max and min of two ints (TODO: consider making this a macro or _Generic)
int maxi(int a, int b);
int mini(int a, int b);

// sign of a signed float
float sign(float i);
