#include "raycast.h"

int main(int argc, char* argv[]) {
	// Print debugging messages about the user's computer
	printSetupMessages();

	assert(SDL_Init(SDL_INIT_VIDEO) == 0);

	SDL_Window *window = SDL_CreateWindow(PROJECT_NAME, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		680, 480,
		0);
	assert(window);

	SDL_Surface *window_surface = SDL_GetWindowSurface(window);
	assert(window_surface);

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