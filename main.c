#include "raycast.h"

// Global state object
struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
	bool quit; // set to 1 when it's time to quit
} state;

int main(int argc, char* argv[]) {
	// Print debugging messages about the user's computer
	printSetupMessages();

	assert(SDL_Init(SDL_INIT_VIDEO) == 0);

	state.window = SDL_CreateWindow(PROJECT_NAME, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH*2 , SCREEN_HEIGHT*2,
		SDL_WINDOW_ALLOW_HIGHDPI);
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
		}

		const uint8_t *keystate = SDL_GetKeyboardState(NULL);
		if (keystate[SDL_SCANCODE_LEFT]) {

		}

		if (keystate[SDL_SCANCODE_RIGHT]) {

		}

		if (keystate[SDL_SCANCODE_UP]) {

		}

		if (keystate[SDL_SCANCODE_DOWN]) {

		}

		// do rendering here
	}

	SDL_Quit();
	return 0;
}