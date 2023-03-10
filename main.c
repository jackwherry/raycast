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
		int i = (y * SCREEN_WIDTH) + x;

		// force a crash before writing outside array bounds
		assert(i >= 0 && i < SCREEN_WIDTH * SCREEN_HEIGHT);

		state.pixels[i] = color;
	}
}

void render(void) {
	// everything in raycasting is in terms of vertical lines, so we start by 
	//	iterating across the width of the pixel array
	for (int x = 0; x < SCREEN_WIDTH; x++) {
		// x coordinate in space from -1 to 1
		const float xcam = (2 * (x / (float) SCREEN_WIDTH)) - 1;

		// ray direction through this column
		const vect2 dir = {
			state.dir.x + state.plane.x * xcam,
			state.dir.y + state.plane.y * xcam
		};

		vect2 pos = state.pos;
		vect2i ipos = {
			(int) state.pos.x, (int) state.pos.y
		};

		// distance ray must travel from one x/y side to the next
		const vect2 deltadist = {
			fabsf(dir.x) < 1e-20 ? 1e30 : fabsf(1.0f / dir.x),
			fabsf(dir.y) < 1e-20 ? 1e30 : fabsf(1.0f / dir.y)
		};

		// distance from start position to first x/y side
		vect2 sidedist = {
			deltadist.x * (dir.x < 0 ? (pos.x - ipos.x) : (ipos.x + 1 - pos.x)),
			deltadist.y * (dir.y < 0 ? (pos.y - ipos.y) : (ipos.y + 1 - pos.y))
		};

		// integer step direction for x/y, calculated from overall diff
		const vect2i step = { (int) sign(dir.x), (int) sign(dir.y) };

		// DDA hit
		struct { int val, side; vect2 pos; } hit = { 0, 0, { 0.0f, 0.0f } };

		while (!hit.val) {
			if (sidedist.x < sidedist.y) {
				sidedist.x += deltadist.x;
				ipos.x += step.x;
				hit.side = 0;
			} else {
				sidedist.y += deltadist.y;
				ipos.y += step.y;
				hit.side = 1;
			}

			// crash if the DDA is out of bounds
			assert(ipos.x >= 0 && ipos.x < MAP_SIZE && ipos.y >= 0 && ipos.y < MAP_SIZE);
			hit.val = MAPDATA[ipos.y * MAP_SIZE + ipos.x];
		}

		uint32_t color;
		if (hit.val == 1) color = 0xFF0000FF;
		else if (hit.val == 2) color = 0xFF00FF00;
		else if (hit.val == 3) color = 0xFFFF0000;
		else if (hit.val == 4) color = 0xFFFF00FF;

		// darken colors on y sides
		if (hit.side == 1) {
			const uint32_t br = ((color & 0xFF00FF) * 0xC0) >> 8;
			const uint32_t g = ((color & 0x00FF00) * 0xC0) >> 8;
			color = 0xFF000000 | (br & 0xFF00FF) | (g & 0x00FF00);
		}

		hit.pos = (vect2) { pos.x + sidedist.x, pos.y + sidedist.y };

		// distance to hit
		const float dperp = hit.side == 0 ? 
			(sidedist.x - deltadist.x) 
			: (sidedist.y - deltadist.y);

		// perform perspective division, calculate line height relative to screen center
		const int h = (int) (SCREEN_HEIGHT / dperp);
		const int y0 = maxi((SCREEN_HEIGHT / 2) - (h / 2), 0);
		const int y1 = mini((SCREEN_HEIGHT / 2) + (h / 2), SCREEN_HEIGHT - 1);

		// draw the vertical lines
		vertline(x, 0, y0, 0xFF202020); // ground
		vertline(x, y0, y1, color); // wall
		vertline(x, y1, SCREEN_HEIGHT - 1, 0xFF505050); // sky
	}
}

void rotate(float rot) {
	const vect2 d = state.dir;
	const vect2 p = state.plane;
	state.dir.x = d.x * cos(rot) - d.y * sin(rot);
	state.dir.y = d.x * sin(rot) + d.y * cos(rot);
	state.plane.x = p.x * cos(rot) - p.y * sin(rot);
	state.plane.y = p.x * sin(rot) + p.y * cos(rot);
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

	// these constants all come from 
	// 	https://github.com/jdah/doomenstein-3d/blob/faef6e5604062c50b96774faeeb229d04522cca6/src/main_wolf.c#L203
	state.pos = (vect2) { 2, 2 };
	state.dir = normalize((vect2) { -1.0f, 0.1f });
	state.plane = (vect2) { 0.0f, 0.66f };

	state.quit = false;
	while (!state.quit) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				state.quit = true;
			}
		}

		const float rotspeed = 3.0f * 0.016f;
		const float movespeed = 3.0f * 0.016f;

		const uint8_t *keystate = SDL_GetKeyboardState(NULL);
		if (keystate[SDL_SCANCODE_LEFT]) {
			rotate(rotspeed);
		}

		if (keystate[SDL_SCANCODE_RIGHT]) {
			rotate(-rotspeed);
		}

		if (keystate[SDL_SCANCODE_UP]) {
			state.pos.x += state.dir.x * movespeed;
			state.pos.y += state.dir.y * movespeed;
		}

		if (keystate[SDL_SCANCODE_DOWN]) {
			state.pos.x -= state.dir.x * movespeed;
			state.pos.y -= state.dir.y * movespeed;
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
