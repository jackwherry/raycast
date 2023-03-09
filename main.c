#include <stdbool.h>
#include <SDL.h>
#include "raycast.h"

int main() {
	// Print debugging messages about the user's computer
	printSetupMessages();

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init Error: %s", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow(PROJECT_NAME, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		680, 480,
		0);

	if (!window) {
		printf("Failed to create window\n");
		return 1;
	}

	SDL_Surface *window_surface = SDL_GetWindowSurface(window);

	if (!window_surface) {
		printf("Failed to create window surface\n");
		return 1;
	}

	SDL_UpdateWindowSurface(window);

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			if (e.type == SDL_KEYDOWN) {
				quit = true;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				quit = true;
			}
		}
	}

	SDL_Quit();
	return 0;
}