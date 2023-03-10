#include "raycast.h"

// Global state object
struct {
	SDL_Window *window;
	bool quit;
} state;

int main(int argc, char* argv[]) {
	// Print debugging messages about the user's computer
	printSetupMessages();

	assert(SDL_Init(SDL_INIT_VIDEO) == 0);

	state.window = SDL_CreateWindow(PROJECT_NAME, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		680, 480,
		0);
	assert(state.window);

	SDL_Surface *window_surface = SDL_GetWindowSurface(state.window);
	assert(window_surface);

	SDL_UpdateWindowSurface(state.window);

	SDL_Event e;
	while (!state.quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				state.quit = true;
			}
			if (e.type == SDL_KEYDOWN) {
				state.quit = true;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				state.quit = true;
			}
		}
	}

	SDL_Quit();
	return 0;
}