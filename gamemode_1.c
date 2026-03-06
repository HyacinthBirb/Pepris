#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10
#define TICK_MS 500

// this is game type 1
// Manaul Drop with no auto drop

struct termios orig;

int gamemode_1(void) {
	int running  = 1;
	int piece_x  = BOARD_WIDTH  / 2;
	int piece_y  = BOARD_HEIGHT / 2;
	int dx = 0, dy = 0;

	struct timespec last, now;
	clock_gettime(CLOCK_MONOTONIC, &last);

	// Basic Game control
	while (running) {
		char c;
		if (read(STDIN_FILENO, &c, 1) == 1) {
			switch (c) {
				case 'w': dy = -1; dx =  0; break;
				case 's': dy =  1; dx =  0; break;
				case 'a': dx = -1; dy =  0; break;
				case 'd': dx =  1; dy =  0; break;
				case 'q': running = 0;       break;
			}
		}

		clock_gettime(CLOCK_MONOTONIC, &now);
		long elapsed = (now.tv_sec  - last.tv_sec)  * 1000000000L
					 + (now.tv_nsec - last.tv_nsec);
		if (elapsed >= TICK_NS) {
			last = now;

			int new_x = piece_x + dx;
			int new_y = piece_y + dy;

			if (new_x >= 0 && new_x < BOARD_WIDTH)  piece_x = new_x;
			if (new_y >= 0 && new_y < BOARD_HEIGHT)  piece_y = new_y;

			dx = 0;
			dy = 0;

			// Render will be here!
			render();
		}
		usleep(16000);
	}
	return 0;
}
