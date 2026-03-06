#include <stdio.h>
#include <stdlib.h>
#include "gamemode_1.h"
#include "gamemode_2.h"

void quit_game(void);

int main(void) {
    char exit_key[4];
    char game_option[4];

    printf("Pepris (DEV)\n\n");
    printf("Welcome to Pepris\n");
    printf("What type of game do you want?\n");
    printf("Normal Tetris (1)\n  Manual Drop with S keybind\n");
    printf("Auto Drop Tetris (2)\n  (S keybind still enabled)\n");
    printf("To exit program, press Q at any time\n\n");
    printf("Enter option: ");
    fgets(game_option, sizeof(game_option), stdin);

    if (game_option[0] == 'Q' || game_option[0] == 'q') {
        quit_game();
        return 0;
    }

    if (game_option[0] == '1') {
        // Game type 1
    } else if (game_option[0] == '2') {
        // Game type 2
    }

    return 0;
}

// Quiting function
void quit_game(void) {
    printf("\n\nExiting game\n");
}