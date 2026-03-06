#ifndef GAMEMODE_1_H
#define GAMEMODE_1_H

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define TICK_MS 500

typedef struct {
    int board[BOARD_WIDTH][BOARD_HEIGHT];

    int piece_x;
    int piece_y;
    int piece_type;
    int piece_rotation;

    int score;
    int lines_cleared;
    int paused;
    int game_over;
} GameState;

int gamemode_1(GameState *state);

#endif