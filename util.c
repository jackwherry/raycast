#include "raycast.h"

void printSetupMessages() {
	printf("Starting " PROJECT_NAME "... \n");
}

vect2 dot(vect2 v0, vect2 v1) {
	return v0.x * v1.x + v0.y * v1.y;
}

float length(vect2 v) {
	return sqrtf(dot(v, v));
}

vect2 normalize(vect2 v) {
	int l = length(v);
	return (vect2) { v.x / l, v.y / l};
}

int maxi(int a, int b) {
	return a > b ? a : b;
}

int mini(int a, int b) {
	return a < b ? a : b;
}

int sign(int i) {
	if (i < 0) return -1;
	else if (i > 0) return 1;
	else return 0;
}
