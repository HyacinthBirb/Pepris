#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include "gamemode_1.h"

// this is game type 1 with manual drop

static struct termios orig;

// function delceration
static void handle_input(GameState *state, char input);
static void show_cursor(void);
static void hide_cursor(void);

static int pieces[7][4][4] = {
    // I
    { {0,0,0,0},
      {1,1,1,1},
      {0,0,0,0},
      {0,0,0,0} },

    // O
    { {0,1,1,0},
      {0,1,1,0},
      {0,0,0,0},
      {0,0,0,0} },

    // T
    { {0,1,0,0},
      {1,1,1,0},
      {0,0,0,0},
      {0,0,0,0} },

    // S
    { {0,1,1,0},
      {1,1,0,0},
      {0,0,0,0},
      {0,0,0,0} },

    // Z
    { {1,1,0,0},
      {0,1,1,0},
      {0,0,0,0},
      {0,0,0,0} },

    // J
    { {1,0,0,0},
      {1,1,1,0},
      {0,0,0,0},
      {0,0,0,0} },

    // L
    { {0,0,1,0},
      {1,1,1,0},
      {0,0,0,0},
      {0,0,0,0} }
};

int gamemode_1(GameState *state) {
    return state->score;
}

static void handle_input(GameState *state, char input) {
    switch (input) {
        case 'a': case 'A': case ',':
            break;
        case 'd': case 'D': case '.':
            break;
        case 'w': case 'W': case ';':
            break;
        case 's': case 'S': case '/':
            break;
        case 'p': case 'P':
            state->paused = !state->paused;
            break;
        case 'q': case 'Q':
            state->game_over = 1;
            break;
    }
}

static void show_cursor(void) {
    printf("\033[?25h");
    fflush(stdout);
}

static void hide_cursor(void) {
    printf("\033[?25l");
    fflush(stdout);
}