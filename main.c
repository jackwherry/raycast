#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <SDL.h>

#define PROJECT_NAME "Raycast"
#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 256

#define PI 3.14159265359f
#define TAU (2.0f * PI)
#define PI_2 (PI / 2.0f)
#define PI_4 (PI / 4.0f)

// fake <stdbool.h>
typedef _Bool bool;

#define false 0
#define true 1

#define SECTOR_NONE 0

#define NUMSECTORS_MAX 1024
#define NUMWALLS_MAX 512

#define DEG2RAD(_d) ((_d) * (PI / 180.0f))
#define RAD2DEG(_d) ((_d) * (180.0f / PI))

#define EYE_Z 1.65f
#define HFOV DEG2RAD(90.0f)
#define VFOV 0.5f

#define ZNEAR 0.0001f
#define ZFAR 128.0f

#define ifnan(_x, _alt) ({ __typeof__(_x) __x = (_x); isnan(__x) ? (_alt) : __x; })

// -1 right, 0 on, 1 left
#define pointSide(_p, _a, _b) ({						\
	__typeof__(_p) __p = (_p), __a = (_a), __b = (_b);	\
	-(((__p.x - __a.x) * (__b.y - __a.y))				\
	- ((__p.y - __a.y) * (__b.x - __a.x)));				\
})


typedef struct vect2_s {
	float x, y;
} vect2;

typedef struct vect2i_s {
	int32_t x, y;
} vect2i;

struct wall {
	vect2i a, b;
	int portal; // 0 for not a portal, otherwise the sector it's a portal to
};

struct sector {
	int id;
	size_t numwalls;
	float zfloor, zceil;
	struct wall walls[NUMWALLS_MAX];
};

// global state object
struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	uint32_t *pixels;

	bool slomo;
	bool noclip;

	struct {
		struct sector arr[NUMSECTORS_MAX]; size_t n;
	} sectors;

	struct {
		vect2 pos;
		float angle, anglecos, anglesin;
		int sector;
	} camera;

	vect2 positionBeforeWorldExit; // the player's final position before exiting the world
	int sectorBeforeWorldExit;

	bool quit; // set to 1 when it's time to quit
} state;

vect2i vect2ToVect2i(vect2 v) {
	return (vect2i) { v.x, v.y };
}
vect2 vect2iToVect2(vect2i v) {
	return (vect2) { v.x, v.y };
}

// dot product of float vectors
float dot(vect2 v0, vect2 v1) {
	return v0.x * v1.x + v0.y * v1.y;
}

// length of a float vector
float length(vect2 v) {
	return sqrtf(dot(v, v));
}

// normalize a float vector (make it a unit vector)
vect2 normalizeVector(vect2 v) {
	int l = length(v);
	return (vect2) { v.x / l, v.y / l};
}

// max and min of two ints
int maxi(int a, int b) {
	return a > b ? a : b;
}

int mini(int a, int b) {
	return a < b ? a : b;
}

// clamp function on ints
int clampi (int mid, int lower, int upper) {
	return mini(maxi(mid, lower), upper);
}

// sign of a signed float
float sign(float i) {
	if (i < 0) return -1;
	else if (i > 0) return 1;
	else return 0;
}

// trigonometry magic to convert an angle [-(HFOV / 2) .. (HFOV / 2)]
//	to an X coordinate
int screenAngleToX(float angle) {
	return ((int) (SCREEN_WIDTH / 2))
		* (1.0f - tan(((angle + (HFOV / 2.0)) / HFOV) * PI_2 - PI_4));
}

// see: https://en.wikipedia.org/wiki/Line–line_intersection
// compute intersection of two line segments, returns (NAN, NAN) if there is
// no intersection
vect2 intersectSegs(vect2 a0, vect2 a1, vect2 b0, vect2 b1) {
	const float d =
		((a0.x - a1.x) * (b0.y - b1.y))
		- ((a0.y - a1.y) * (b0.x - b1.x));

	if (fabsf(d) < 0.000001f) { return (vect2) { NAN, NAN }; }

	const float
		t = (((a0.x - b0.x) * (b0.y - b1.y))
			- ((a0.y - b0.y) * (b0.x - b1.x))) / d,
		u = (((a0.x - b0.x) * (a0.y - a1.y))
			- ((a0.y - b0.y) * (a0.x - a1.x))) / d;
	return (t >= 0 && t <= 1 && u >= 0 && u <= 1) ?
		((vect2) {
			a0.x + (t * (a1.x - a0.x)),
			a0.y + (t * (a1.y - a0.y)) })
		: ((vect2) { NAN, NAN });
}

// rotate vector v by angle a
vect2 rotate(vect2 v, float a) {
	return (vect2) {
		(v.x * cos(a)) - (v.y * sin(a)),
		(v.x * sin(a)) + (v.y * cos(a)),
	};
}

// normalize angle to +/-pi
float normalizeAngle(float angle) {
	return angle - (TAU * floor((angle + PI) / TAU));
}

// point is in sector if it is on the left side of all the sector's walls
bool pointInSector(const struct sector *sector, vect2 p) {
	for (size_t i = 0; i < sector->numwalls; i++) {
		const struct wall *wall = &sector->walls[i];
	
		if (pointSide(p, vect2iToVect2(wall->a), vect2iToVect2(wall->b)) > 0) {
			return false;
		}
	}
	return true;
}

uint32_t colorMult(uint32_t color, uint32_t a) {
	const uint32_t blueRed = ((color & 0xFF00FF) * a) >> 8;
	const uint32_t green   = ((color & 0x00FF00) * a) >> 8;

	return 0xFF000000 | (blueRed & 0xFF00FF) | (green & 0x00FF00);
}

// translate and rotate world space to camera space
vect2 worldPosToCamera(vect2 p) {
	const vect2 u = { p.x - state.camera.pos.x, p.y - state.camera.pos.y };
	return (vect2) {
		u.x * state.camera.anglesin - u.y * state.camera.anglecos,
		u.x * state.camera.anglecos + u.y * state.camera.anglesin
	};
}

int loadSectors(void) {
	state.sectors.n = 1;

	struct sector initialSectors[] = {
		{
			.id = 1, .zfloor = 0.0f, .zceil = 5.0f, .numwalls = 8,
			.walls = {
				{{4, 1}, {2, 1}, 0}, {{5, 2}, {4, 1}, 0}, {{5, 3}, {5, 2}, 0},
				{{4, 4}, {5, 3}, 3}, {{2, 4}, {4, 4}, 0}, {{1, 3}, {2, 4}, 2},
				{{1, 2}, {1, 3}, 0}, {{2, 1}, {1, 2}, 0}
			}
		},
		{
			.id = 2, .zfloor = 1.0f, .zceil = 4.0f, .numwalls = 3,
			.walls = {
				{{2, 4}, {1, 3}, 1}, {{1, 5}, {2, 4}, 0}, {{1, 3}, {1, 5}, 0}
			}
		},
		{
			.id = 3, .zfloor = 0.2f, .zceil = 6.0f, .numwalls = 4,
			.walls = {
				{{5, 3}, {4, 4}, 1}, {{6, 5}, {5, 3}, 0}, {{6, 7}, {6, 5}, 4},
				{{4, 4}, {6, 7}, 0}
			}
		},
		{
			.id = 4, .zfloor = 0.0f, .zceil = 3.0f, .numwalls = 5,
			.walls = {
				{{7, 4}, {6, 5}, 0}, {{8, 5}, {7, 4}, 0}, {{8, 7}, {8, 5}, 0},
				{{6, 7}, {8, 7}, 0}, {{6, 5}, {6, 7}, 3}
			}
		}
	};

	int numToLoad = sizeof(initialSectors) / sizeof(struct sector);

	for (int i = 0; i < numToLoad; i++) {
		int id = initialSectors[i].id;

		if (id >= NUMSECTORS_MAX) return -18;

		state.sectors.arr[id] = initialSectors[i];
		state.sectors.n++;
	}

    return 0;
}

void newSector(void) {
	if (state.sectors.n + 1 < NUMSECTORS_MAX) {
		struct sector *sector = &state.sectors.arr[state.sectors.n++];
		sector->numwalls = 0; sector->zfloor = 0.0f; sector->zceil = 5.0f;
	} 
} 

void newWall(struct sector *sector) {
	if (sector->numwalls + 1 < NUMWALLS_MAX) {
		struct wall *wall = &sector->walls[sector->numwalls++];
		vect2i a = { 0, 0 }, b = { 0, 0 };
		wall->a = a; wall->b = b; wall->portal = 0;
	} 
}

// you can't delete a sector because we don't want to change the ids of every other 
//	sector, since that would cause existing portals to break---instead you should 
//	delete all the walls in a sector or remove it from the level file

void deleteWall(struct sector *sector, int index) {
	if (sector->numwalls > 0) {
		if (sector->numwalls > 1) {
			// shift everything to the left, erasing the wall at the given index
			//	only required if the sector has two or more walls
			for (size_t i = index; i < sector->numwalls - 2; i++) {
				sector->walls[i] = sector->walls[i + 1];
			}
		}

		// decrement the size by one
		sector->numwalls--;
	}
}

void vertline(int x, int yStart, int yEnd, uint32_t color) {
	// set an entire vertical line of pixels to the given color
	for (int y = yStart; y <= yEnd; y++) {
		int i = (y * SCREEN_WIDTH) + x;

		// force a crash before writing outside array bounds
		if (!(i >= 0 && i < SCREEN_WIDTH * SCREEN_HEIGHT * 4)) return;

		state.pixels[i] = color;
	}
}

void present(void) {
	void *px;
	int pitch;

	SDL_LockTexture(state.texture, NULL, &px, &pitch); // replace UpdateTexture
	for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
		memcpy(&((uint8_t*) px)[y * pitch], &state.pixels[y * SCREEN_WIDTH], SCREEN_WIDTH * 4);
	}
	SDL_UnlockTexture(state.texture);

	SDL_SetRenderTarget(state.renderer, NULL);
	SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 0xFF);
	SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderClear(state.renderer);

	SDL_RenderCopyEx(state.renderer, state.texture, 
		NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
	SDL_RenderPresent(state.renderer);
}

void render(void) {
	// visible ceiling and floor heights across the screen width
	uint16_t y_lo[SCREEN_WIDTH], y_hi[SCREEN_WIDTH];
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		y_hi[i] = SCREEN_HEIGHT - 1;
		y_lo[i] = 0;
	}

	// track which sectors have been drawn
	bool sectdraw[NUMSECTORS_MAX];
	memset(sectdraw, 0, sizeof(sectdraw));

	// calculate edges of near/far planes, looking down positive y axis
	//	see: https://en.wikipedia.org/wiki/Viewing_frustum
	const vect2
		zdl = rotate(((vect2) { 0.0f, 1.0f }),  (HFOV / 2.0f)),
		zdr = rotate(((vect2) { 0.0f, 1.0f }), -(HFOV / 2.0f)), // direction vectors left and right
		znl = (vect2) { zdl.x * ZNEAR, zdl.y * ZNEAR },
		znr = (vect2) { zdr.x * ZNEAR, zdr.y * ZNEAR }, // left and right sides of near plane
		zfl = (vect2) { zdl.x * ZFAR, zdl.y * ZFAR },
		zfr = (vect2) { zdr.x * ZFAR, zdr.y * ZFAR }; // left and right sides of far plane


	// queue of sectors to render
	enum { QUEUE_MAX = 64 }; // don't render more than 64 sectors
	struct queue_entry { int id, x0, x1; }; // id of sector and left and right bounds of portal window

	// singleton anonymous struct containing an array of queue_entries and a size
	struct { struct queue_entry arr[QUEUE_MAX]; size_t n; } queue = {
		// always start by rendering the sector the camera is in
		{{ state.camera.sector, 0, SCREEN_WIDTH - 1 }}, 1
	};

	while (queue.n != 0) {
		// render the end of the queue first
		struct queue_entry entry = queue.arr[--queue.n];

		if (sectdraw[entry.id]) {
			continue; // if we've already rendered this sector, don't do it again
		}

		sectdraw[entry.id] = true;

		const struct sector *sector = &state.sectors.arr[entry.id];

		for (size_t i = 0; i < sector->numwalls; i++) {
			const struct wall *wall = &sector->walls[i];

			// translate relative to player and rotate points around player's view
			const vect2 
				op0 = worldPosToCamera(vect2iToVect2(wall->a)),
				op1 = worldPosToCamera(vect2iToVect2(wall->b));

			// wall clipped pos (set later)
			vect2 cp0 = op0, cp1 = op1;


			// skip rendering the wall if it's completely behind the player
			if (cp0.y <= 0 && cp1.y <= 0) {
				continue;
			}

			// angle-clip against view frustum
			float 
				ap0 = normalizeAngle(atan2(cp0.y, cp0.x) - PI_2),
				ap1 = normalizeAngle(atan2(cp1.y, cp1.x) - PI_2);

			// clip against view frustum if both angles are not within HFOV
			if (cp0.y < ZNEAR 
				|| cp1.y < ZNEAR 
				|| ap0 >  (HFOV / 2)
				|| ap1 < -(HFOV / 2)) {
				const vect2
					il = intersectSegs(cp0, cp1, znl, zfl),
					ir = intersectSegs(cp0, cp1, znr, zfr);

				// recompute angles if points change
				if (!isnan(il.x)) {
					cp0 = il;
					ap0 = normalizeAngle(atan2(cp0.y, cp0.x) - PI_2);
				}

				if (!isnan(ir.x)) {
					cp1 = ir;
					ap1 = normalizeAngle(atan2(cp1.y, cp1.x) - PI_2);
				}
			}

			if (ap0 < ap1) {
				continue;
			}

			if ((ap0 < -(HFOV / 2) && ap1 < -(HFOV / 2)) 
				|| (ap0 > (HFOV / 2) && ap1 > (HFOV / 2))) {
				continue;
			}

			// true x values before portal clamping
			const int tx0 = screenAngleToX(ap0), tx1 = screenAngleToX(ap1);

			// bounds check against portal window
			if (tx0 > entry.x1) continue;
			if (tx1 < entry.x0) continue;

			// give the illusion of light on walls
			const int wallshade = 16 * (sin(atan2f(wall->b.x - wall->a.x, wall->b.y - wall->a.y)) + 1.0f);

			// clamp to portal boundaries
			const int
				x0 = clampi(tx0, entry.x0, entry.x1),
				x1 = clampi(tx1, entry.x0, entry.x1);

			const float
				z_floor = sector->zfloor,
				z_ceil = sector->zceil,
				nz_floor = 
					wall->portal ? state.sectors.arr[wall->portal].zfloor : 0,
				nz_ceil = 
					wall->portal ? state.sectors.arr[wall->portal].zceil : 0;

			const float
				sy0 = ifnan((VFOV * SCREEN_HEIGHT) / cp0.y, 1e10),
				sy1 = ifnan((VFOV * SCREEN_HEIGHT) / cp1.y, 1e10);

			const int
				yf0  = (SCREEN_HEIGHT / 2) + (int) (( z_floor - EYE_Z) * sy0),
				yc0  = (SCREEN_HEIGHT / 2) + (int) (( z_ceil  - EYE_Z) * sy0),
				yf1  = (SCREEN_HEIGHT / 2) + (int) (( z_floor - EYE_Z) * sy1),
				yc1  = (SCREEN_HEIGHT / 2) + (int) (( z_ceil  - EYE_Z) * sy1),
				nyf0 = (SCREEN_HEIGHT / 2) + (int) ((nz_floor - EYE_Z) * sy0),
				nyc0 = (SCREEN_HEIGHT / 2) + (int) ((nz_ceil  - EYE_Z) * sy0),
				nyf1 = (SCREEN_HEIGHT / 2) + (int) ((nz_floor - EYE_Z) * sy1),
				nyc1 = (SCREEN_HEIGHT / 2) + (int) ((nz_ceil  - EYE_Z) * sy1),
				txd = tx1 - tx0,
				yfd = yf1 - yf0,
				ycd = yc1 - yc0,
				nyfd = nyf1 - nyf0,
				nycd = nyc1 - nyc0;

			for (int x = x0; x <= x1; x++) {
				int shade = 255 - wallshade;

				// calculate progress along x axis via tx{0,1} so that walls
				//	which are partially cut off due to portal edges still have proper heights
				const float xp = ifnan((x - tx0) / (float) txd, 0);

				// get y ceil and floor for this x ("y=mx+b", yo!)
				const int
					tyf = (int) (xp * yfd) + yf0,
					tyc = (int) (xp * ycd) + yc0,
					yf = clampi(tyf, y_lo[x], y_hi[x]),
					yc = clampi(tyc, y_lo[x], y_hi[x]);

				// draw the floor
				if (yf > y_lo[x]) {
					vertline(x, y_lo[x], yf, 0xFFFF0000);
				}

				// draw the ceiling
				if (yc < y_hi[x]) {
					vertline(x, yc, y_hi[x], 0xFF00FFFF);
				}

				// draw walls
				if (wall->portal) {
					const int
						tnyf = (int) (xp * nyfd) + nyf0,
						tnyc = (int) (xp * nycd) + nyc0,
						nyf = clampi(tnyf, y_lo[x], y_hi[x]),
						nyc = clampi(tnyc, y_lo[x], y_hi[x]);

					// step down in the ceiling
					vertline(x, nyc, yc, colorMult(0xFF00FF00, shade));
					// color the face of the step up in the floor
					vertline(x, yf, nyf, colorMult(0xFF0000FF, shade));

					y_hi[x] = clampi(mini(mini(yc, nyc), y_hi[x]), 0, SCREEN_HEIGHT - 1);
					y_lo[x] = clampi(maxi(maxi(yf, nyf), y_lo[x]), 0, SCREEN_HEIGHT - 1);
				} else {
					vertline(x, yf, yc, colorMult(0xFFD0D0D0, shade)); // draw normal walls
				}

				// present now to hide the UI and make slomo smooth
				if (state.slomo) {
					present();
					SDL_Delay(6);
				}
			}

			if (wall->portal) {
				if (!(queue.n < QUEUE_MAX)) return; // make sure we're not out of queue space
				queue.arr[queue.n++] = (struct queue_entry) {
					.id = wall->portal,
					.x0 = x0,
					.x1 = x1
				};
			}
		}
	}
}

int main(int argc, char* argv[]) {
	state.pixels = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);

	if (!(SDL_Init(SDL_INIT_VIDEO) == 0)) return -1;

	state.window = SDL_CreateWindow(PROJECT_NAME, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH*3 , SCREEN_HEIGHT*3,
		SDL_WINDOW_ALLOW_HIGHDPI);
	if (!(state.window)) return -1;

	state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!(state.renderer)) return -1;

	state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888, 
		SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if (!(state.texture)) return -1;

	state.camera.pos = (vect2) { 2, 2 };
	state.camera.angle = 0.0;
	state.camera.sector = 0;

	state.positionBeforeWorldExit = state.camera.pos;
	state.sectorBeforeWorldExit = 1;

	loadSectors();

	/* scale the renderer output for High-DPI displays */
	{
		int render_w, render_h;
		int window_w, window_h;
		float scale_x, scale_y;
		SDL_GetRendererOutputSize(state.renderer, &render_w, &render_h);
		SDL_GetWindowSize(state.window, &window_w, &window_h);
		scale_x = (float)(render_w) / (float)(window_w);
		scale_y = (float)(render_h) / (float)(window_h);
		SDL_RenderSetScale(state.renderer, scale_x, scale_y);
	}

	state.slomo = false;
	state.noclip = false;

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
		if (keystate[SDLK_LEFT & 0xFFFF]) {
			state.camera.angle += rotspeed;
		}

		if (keystate[SDLK_RIGHT & 0xFFFF]) {
			state.camera.angle -= rotspeed;
		}

		state.camera.anglecos = cos(state.camera.angle);
		state.camera.anglesin = sin(state.camera.angle);

		if (keystate[SDLK_UP & 0xFFFF]) {
			state.camera.pos.x += movespeed * state.camera.anglecos;
			state.camera.pos.y += movespeed * state.camera.anglesin;
		}

		if (keystate[SDLK_DOWN & 0xFFFF]) {
			state.camera.pos.x -= movespeed * state.camera.anglecos;
			state.camera.pos.y -= movespeed * state.camera.anglesin;
		}

		bool outsideWorld = false;

		// update player's sector
		{
			// BFS neighbors in a circular queue because player is likely to be in a neighboring sector
			enum { QUEUE_MAX = 64 };
			int queue[QUEUE_MAX] = { state.camera.sector };
			int i = 0, n = 1, found = SECTOR_NONE;

			while (n != 0) {
				// get front of queue and advance to next
				const int id = queue[i];
				i = (i + 1) % QUEUE_MAX; // wrap around
				n--;

				const struct sector *sector = &state.sectors.arr[id];

				if (pointInSector(sector, state.camera.pos)) {
					found = id;
					break;
				}

				// check neighbors
				for (size_t j = 0; j < sector->numwalls; j++) {
					const struct wall *wall = &sector->walls[j];

					if (wall->portal) {
						if (n == QUEUE_MAX) {
							goto done;
						}
						queue [(i + n) % QUEUE_MAX] = wall->portal;
						n++;
					}
				}
			}
done:
			if (!found) {
				outsideWorld = true;
			} else {
				state.camera.sector = found;
			}
		}

		// check for collisions and update the camera pos accordingly
		if (outsideWorld && !state.noclip) {
			vect2 newPosition = { state.positionBeforeWorldExit.x, state.positionBeforeWorldExit.y };
			state.camera.pos = newPosition;
			state.camera.sector = state.sectorBeforeWorldExit;
		}

		if (!outsideWorld) {
			state.positionBeforeWorldExit = state.camera.pos;
			state.sectorBeforeWorldExit = state.camera.sector;
		}

		// clear existing pixel array and render to it
		memset(state.pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT * 4);
		render();

		if (!state.slomo) present();
		else state.slomo = false; // only one frame 
	}

exit:
	SDL_DestroyTexture(state.texture);
	SDL_DestroyRenderer(state.renderer);
	SDL_DestroyWindow(state.window);
	SDL_Quit();
	exit(0);
}
