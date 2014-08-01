#ifdef BUILD_PC

#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdio.h>

void setup(); // Forward arduino
void loop(); // Forward arduino

int pc_btn_1 = 0; // Button 1
int pc_btn_2 = 0; // Button 2

long long milliseconds = 0; // Elapsed milliseconds

SDL_Surface * screen = 0; // Screen surface

// Return elapsed milliseconds
long long millis() {
	return milliseconds;
}

int main(int argc, char** argv) {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stdout,"SDL_Init fail (%s)\n", SDL_GetError());
		return -1;
	}

	// Create window
	SDL_Window* pWindow = NULL;
	pWindow = SDL_CreateWindow(
		"ponghaum emulation",
		SDL_WINDOWPOS_UNDEFINED, // x
		SDL_WINDOWPOS_UNDEFINED, // y
		640, // w
		200, // h
		SDL_WINDOW_SHOWN // flags
	);
	screen = SDL_GetWindowSurface(pWindow);
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

	// Main loop
	if (pWindow) {
		// Launch arduino setup
		setup();

		// Loop
		SDL_Event event;
		bool run = true;
		int looplimiter = 5;
		struct timespec t;
		pthread_mutex_t my_mutex;
		pthread_cond_t my_cond;
		pthread_mutex_init(&my_mutex, NULL);
		pthread_cond_init(&my_cond, NULL);
		while (run) {
			// Prepare next jump
			clock_gettime (CLOCK_REALTIME, &t);
			t.tv_nsec += 1e6 * looplimiter;
			if (t.tv_nsec >= 1e9) {
				t.tv_sec += 1;
				t.tv_nsec -= 1e9;
			}

			// Treat all events
			while (SDL_PollEvent(&event)) {
				switch(event.type) {
					case SDL_QUIT:
						run = false;
						break;

					case SDL_KEYUP:
						if (event.key.keysym.sym == SDLK_LEFT)
							pc_btn_1 = false;
						else if (event.key.keysym.sym == SDLK_RIGHT)
							pc_btn_2 = false;
						break;

					case SDL_KEYDOWN:
						if (event.key.keysym.sym == SDLK_ESCAPE)
							run = false;
						else if (event.key.keysym.sym == SDLK_LEFT)
							pc_btn_1 = true;
						else if (event.key.keysym.sym == SDLK_RIGHT)
							pc_btn_2 = true;
						break;
				}
			}

			// Run arduino loop function
			loop();

			// Update window
			SDL_UpdateWindowSurface(pWindow);

			// Wait
			pthread_cond_timedwait(&my_cond, &my_mutex, &t);

			// Update time counter
			milliseconds += looplimiter;
		}
		SDL_DestroyWindow(pWindow);
	} else {
		fprintf(stderr,"Erreur de création de la fenêtre: %s\n", SDL_GetError());
	}

	SDL_Quit();
	return 0;
}

#endif
