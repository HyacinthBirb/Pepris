#include <iostream>
#include <stdlib.h>
#include "gamemode_2.h"
#include "gamemode_1.h"
using namespace std;

void quit_game(void);

int main(int) {
    string exit;
    string game_option;
    cout << "Pepris (DEV)\n\n";
    cout << "Welcome to Pepris \n";
    cout << "What type of game do you want?\n";
    cout << "Normal Tetris (1)\n Manual Drop with S keybind\n";
    cout << "Auto Drop Tetris (2)\n (S keybind still enabled)\n";
    cout << "To exit program, press Q at any time\n";

    // Insure this is always active!!!!
    if (exit == "Q") {
        quit_game();
    }
    if (game_option == "1") {
        // Game type 1
    }
    else if (game_option == "2") {
        // game type 2
    }
}

// quit game function
void quit_game(void) {
    cout << "\n\n Exiting game \n" << endl;
    return 0;
}