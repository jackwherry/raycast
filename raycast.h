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
vect2 dot(vect2 v0, vect2 v1);

// length of a float vector
float length(vect2 v);

// normalize a float vector (make it a unit vector)
vect2 normalize(vect2 v);

// max and min of two ints (TODO: consider making this a macro or _Generic)
int maxi(int a, int b);
int mini(int a, int b);

// sign of a signed inteer
int sign(int i);
