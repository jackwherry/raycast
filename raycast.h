// PROJECT-WIDE INCLUDES 
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
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
void printSetupMessages(void);