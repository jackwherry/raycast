#include "raycast.h"

// global state object
struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

	vect2 pos, dir, plane;

	bool quit; // set to 1 when it's time to quit
} state;

void vertline(int x, int yStart, int yEnd, uint32_t color) {
	// set an entire vertical line of pixels to the given color
	for (int y = yStart; y <= yEnd; y++) {
		state.pixels[(y * SCREEN_WIDTH) + x] = color;
	}
}

void render() {
	// everything in raycasting is in terms of vertical lines, so we start by 
	//	iterating across the width of the pixel array
	for (int x = 0; x < SCREEN_WIDTH; x++) {
			vertline(x, state.pos.y, state.pos.x, 0xFFFF20AA);
	}
}

int main(int argc, char* argv[]) {
	printSetupMessages();

	assert(SDL_Init(SDL_INIT_VIDEO) == 0);

	state.window = SDL_CreateWindow(PROJECT_NAME, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH*2 , SCREEN_HEIGHT*2,
		SDL_WINDOW_ALLOW_HIGHDPI);
	assert(state.window);

	state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);
	assert(state.renderer);

	state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888, 
		SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	assert(state.texture);

	state.pos = (vect2) { 2, 2 };

	state.quit = false;
	while (!state.quit) {
		SDL_Event e;
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
			state.pos.x++;
		}

		if (keystate[SDL_SCANCODE_DOWN]) {
			state.pos.x--;
		}

		// clear existing pixel array and render to it
		memset(state.pixels, 0, sizeof(state.pixels));
		render();

		// copy the pixel array to the texture, then the renderer, then display everything
		SDL_UpdateTexture(state.texture, NULL, state.pixels, SCREEN_WIDTH * 4);
		SDL_RenderCopyEx(state.renderer, state.texture, 
			NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
		SDL_RenderPresent(state.renderer);
	}

	SDL_DestroyTexture(state.texture);
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
	SDL_Quit();
	return 0;
}