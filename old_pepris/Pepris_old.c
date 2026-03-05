#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <sys/ioctl.h>
#include <signal.h>

// This is made by the Birb using Vibe coding becous my flight is in 5 minutes
// Since its shit, im making it open source and well, you the internet can fix this completly broken Tetris* (or as I call, Pepris)
// Current version is

// V - 0.1

//People who wasted there time:
// HyacinthBirb - 5 minutes writing a lazy promp
//

// Game dimensions
#define GRID_WIDTH 10
#define GRID_HEIGHT 20
#define DISPLAY_WIDTH (GRID_WIDTH * 2)
#define DISPLAY_HEIGHT (GRID_HEIGHT + 2)

// Tetris pieces (4x4 grid for each piece)
typedef struct {
    int shape[4][4];
    int rotation;
} Piece;

// Game modes
#define GAME_MODE_NORMAL 1
#define GAME_MODE_AUTO_DROP 2

// Game state
typedef struct {
    int grid[GRID_HEIGHT][GRID_WIDTH];
    Piece current_piece;
    int piece_x;
    int piece_y;
    int score;
    int level;
    int lines_cleared;
    int game_over;
    int paused;
    int game_mode;
    int drop_speed_ms;  // Drop speed in milliseconds
} GameState;

// Terminal configuration
struct termios original_termios;

// Function prototypes
void init_terminal(void);
void restore_terminal(void);
void clear_screen(void);
void draw_game(GameState *state);
void move_piece_left(GameState *state);
void move_piece_right(GameState *state);
void rotate_piece(GameState *state);
void move_piece_down(GameState *state);
void handle_input(GameState *state, char input);
int check_collision(GameState *state, int dx, int dy, int rotation);
void lock_piece(GameState *state);
void clear_lines(GameState *state);
void init_game(GameState *state, int game_mode, int drop_speed_ms);
void game_loop(GameState *state);
void show_menu(void);
int check_game_over(GameState *state);
void signal_handler(int sig);

// Tetromino definitions (I, O, T, S, Z, J, L)
Piece tetrominos[7] = {
    // I piece
    {{{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}, 0},
    // O piece
    {{{1,1,0,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}}, 0},
    // T piece
    {{{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}, 0},
    // S piece
    {{{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}}, 0},
    // Z piece
    {{{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}, 0},
    // J piece
    {{{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}, 0},
    // L piece
    {{{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}, 0}
};

void signal_handler(int sig) {
    restore_terminal();
    exit(0);
}

void init_terminal(void) {
    struct termios new_termios;
    
    // Save original terminal settings
    tcgetattr(STDIN_FILENO, &original_termios);
    new_termios = original_termios;
    
    // Disable canonical mode and echo
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0;
    new_termios.c_cc[VTIME] = 0;
    
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    
    // Set up signal handler for clean exit
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
}

void restore_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    clear_screen();
    printf("\033[?25h"); // Show cursor
    fflush(stdout);
}

void clear_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void hide_cursor(void) {
    printf("\033[?25l");
    fflush(stdout);
}

void show_cursor(void) {
    printf("\033[?25h");
    fflush(stdout);
}

void init_game(GameState *state, int game_mode, int drop_speed_ms) {
    memset(state->grid, 0, sizeof(state->grid));
    state->current_piece = tetrominos[rand() % 7];
    state->piece_x = GRID_WIDTH / 2 - 2;
    state->piece_y = 0;
    state->score = 0;
    state->level = 1;
    state->lines_cleared = 0;
    state->game_over = 0;
    state->paused = 0;
    state->game_mode = game_mode;
    state->drop_speed_ms = drop_speed_ms;
}

void draw_game(GameState *state) {
    clear_screen();
    
    printf("╔");
    for (int i = 0; i < GRID_WIDTH; i++) printf("══");
    printf("╗\n");
    
    // Draw grid with current piece
    for (int y = 0; y < GRID_HEIGHT; y++) {
        printf("║");
        for (int x = 0; x < GRID_WIDTH; x++) {
            int block = state->grid[y][x];
            
            // Check if current piece occupies this space
            if (state->piece_y <= y && y < state->piece_y + 4 &&
                state->piece_x <= x && x < state->piece_x + 4) {
                int py = y - state->piece_y;
                int px = x - state->piece_x;
                if (state->current_piece.shape[py][px]) {
                    block = 1;
                }
            }
            
            if (block) {
                printf("█ ");
            } else {
                printf("  ");
            }
        }
        printf("║\n");
    }
    
    printf("╚");
    for (int i = 0; i < GRID_WIDTH; i++) printf("══");
    printf("╝\n\n");
    
    printf("Score: %d\n", state->score);
    printf("Level: %d\n", state->level);
    printf("Lines: %d\n", state->lines_cleared);
    printf("\nControls: A/D (Move) | W (Rotate) | P (Pauses Game) | S (Drop)\n");
    if (state->paused) printf("\n*** PAUSED ***\n");
    
    fflush(stdout);
}

int get_drop_speed(void) {
    clear_screen();
    printf("╔════════════════════════════════╗\n");
    printf("║   DROP SPEED CONFIGURATION      ║\n");
    printf("╠════════════════════════════════╣\n");
    printf("║  Enter drop speed (millisecs):  ║\n");
    printf("║                                 ║\n");
    printf("║  1. Slow (1000ms)               ║\n");
    printf("║  2. Normal (700ms) [default]    ║\n");
    printf("║  3. Fast (500ms)                ║\n");
    printf("║  4. Very Fast (300ms)           ║\n");
    printf("║  5. Custom (enter value)        ║\n");
    printf("║                                 ║\n");
    printf("╚════════════════════════════════╝\n");
    printf("\nSelect (1-5, or press Enter): ");
    fflush(stdout);
    
    char input[10];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 700; // default
    }
    
    if (input[0] == '\n' || input[0] == '\0') {
        return 700; // default
    }
    
    switch (input[0]) {
        case '1':
            return 1000;
        case '2':
            return 700;
        case '3':
            return 500;
        case '4':
            return 300;
        case '5': {
            printf("Enter custom drop speed (milliseconds): ");
            fflush(stdout);
            int custom;
            if (scanf("%d", &custom) == 1 && custom > 0 && custom < 5000) {
                return custom;
            }
            return 700; // fallback to default
        }
        default:
            return 700; // default
    }
}

void show_menu(void) {
    clear_screen();
    printf("╔════════════════════════════════╗\n");
    printf("║       PEPRIS GAME               ║\n");
    printf("╠════════════════════════════════╣\n");
    printf("║  Game Mode Selection:           ║\n");
    printf("║                                 ║\n");
    printf("║  1. Normal Tetris               ║\n");
    printf("║     (Manual drop with S)        ║\n");
    printf("║                                 ║\n");
    printf("║  2. Auto Drop Mode              ║\n");
    printf("║     (Faster auto-drop only)     ║\n");
    printf("║                                 ║\n");
    printf("║  3. Exit                        ║\n");
    printf("║                                 ║\n");
    printf("╚════════════════════════════════╝\n");
    printf("            V-0.1.0\n");
    printf("\nSelect mode (1, 2, or 3): ");
    fflush(stdout);
}

int check_collision(GameState *state, int dx, int dy, int rotation) {
    Piece temp_piece = state->current_piece;
    
    // Rotate if needed (simplified rotation)
    if (rotation != state->current_piece.rotation) {
        // For now, we'll skip rotation collision checking
        // A full implementation would have proper rotation matrices
    }
    
    for (int py = 0; py < 4; py++) {
        for (int px = 0; px < 4; px++) {
            if (!temp_piece.shape[py][px]) continue;
            
            int x = state->piece_x + px + dx;
            int y = state->piece_y + py + dy;
            
            if (x < 0 || x >= GRID_WIDTH || y >= GRID_HEIGHT) {
                return 1;
            }
            
            if (y >= 0 && state->grid[y][x]) {
                return 1;
            }
        }
    }
    
    return 0;
}

void move_piece_left(GameState *state) {
    if (!check_collision(state, -1, 0, state->current_piece.rotation)) {
        state->piece_x--;
    }
}

void move_piece_right(GameState *state) {
    if (!check_collision(state, 1, 0, state->current_piece.rotation)) {
        state->piece_x++;
    }
}

void move_piece_down(GameState *state) {
    if (!check_collision(state, 0, 1, state->current_piece.rotation)) {
        state->piece_y++;
    } else {
        lock_piece(state);
    }
}

void rotate_piece(GameState *state) {
    // Simplified rotation - rotate 90 degrees clockwise
    int rotated[4][4] = {0};
    
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            rotated[x][3-y] = state->current_piece.shape[y][x];
        }
    }
    
    // Check if rotation is valid
    Piece backup = state->current_piece;
    memcpy(state->current_piece.shape, rotated, sizeof(rotated));
    
    if (check_collision(state, 0, 0, (state->current_piece.rotation + 1) % 4)) {
        state->current_piece = backup;
    } else {
        state->current_piece.rotation = (state->current_piece.rotation + 1) % 4;
    }
}

void lock_piece(GameState *state) {
    // Lock the piece in the grid
    for (int py = 0; py < 4; py++) {
        for (int px = 0; px < 4; px++) {
            if (!state->current_piece.shape[py][px]) continue;
            
            int y = state->piece_y + py;
            int x = state->piece_x + px;
            
            if (y >= 0 && y < GRID_HEIGHT && x >= 0 && x < GRID_WIDTH) {
                state->grid[y][x] = 1;
            }
        }
    }
    
    clear_lines(state);
    
    // Spawn new piece
    state->current_piece = tetrominos[rand() % 7];
    state->piece_x = GRID_WIDTH / 2 - 2;
    state->piece_y = 0;
    
    // Check for game over
    if (check_collision(state, 0, 0, state->current_piece.rotation)) {
        state->game_over = 1;
    }
}

void clear_lines(GameState *state) {
    int lines_to_clear = 0;
    
    for (int y = GRID_HEIGHT - 1; y >= 0; y--) {
        int full = 1;
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (!state->grid[y][x]) {
                full = 0;
                break;
            }
        }
        
        if (full) {
            lines_to_clear++;
            // Shift rows down
            for (int sy = y; sy > 0; sy--) {
                memcpy(state->grid[sy], state->grid[sy-1], sizeof(state->grid[sy]));
            }
            memset(state->grid[0], 0, sizeof(state->grid[0]));
            y++; // Check this row again
        }
    }
    
    if (lines_to_clear > 0) {
        state->lines_cleared += lines_to_clear;
        state->score += lines_to_clear * 100;
        state->level = 1 + state->lines_cleared / 10;
    }
}

void handle_input(GameState *state, char input) {
    switch (input) {
        case 'a':
        case 'A':
        case ',':
            move_piece_left(state);
            break;
        case 'd':
        case 'D':
        case '.':
            move_piece_right(state);
            break;
        case 'w':
        case 'W':
        case ';':
            rotate_piece(state);
            break;
        case 's':
        case 'S':
        case '/':
            move_piece_down(state);
            break;
        case 'p':
        case 'P':
            state->paused = !state->paused;
            break;
        case 'q':
        case 'Q':
            state->game_over = 1;
            break;
    }
}

void game_loop_normal(GameState *state) {
    hide_cursor();
    struct timespec last_drop, now;
    clock_gettime(CLOCK_MONOTONIC, &last_drop);
    
    while (!state->game_over) {
        draw_game(state);
        
        // Handle input
        char input;
        if (read(STDIN_FILENO, &input, 1) > 0) {
            handle_input(state, input);
        }
        
        // Auto drop piece
        if (!state->paused) {
            clock_gettime(CLOCK_MONOTONIC, &now);
            long elapsed_ms = (now.tv_sec - last_drop.tv_sec) * 1000 + 
                            (now.tv_nsec - last_drop.tv_nsec) / 1000000;
            
            if (elapsed_ms > state->drop_speed_ms) {
                move_piece_down(state);
                clock_gettime(CLOCK_MONOTONIC, &last_drop);
            }
        }
        
        usleep(16667); // ~60 FPS
    }
    
    show_cursor();
}

void game_loop_auto_drop(GameState *state) {
    hide_cursor();
    struct timespec last_drop, now;
    clock_gettime(CLOCK_MONOTONIC, &last_drop);
    
    while (!state->game_over) {
        draw_game(state);
        
        // Handle input - only movement allowed, no manual drop
        char input;
        if (read(STDIN_FILENO, &input, 1) > 0) {
            if (input == 'a' || input == 'A') {
                move_piece_left(state);
            } else if (input == 'd' || input == 'D') {
                move_piece_right(state);
            } else if (input == 'w' || input == 'W') {
                rotate_piece(state);
            } else if (input == 'p' || input == 'P') {
                state->paused = !state->paused;
            } else if (input == 'q' || input == 'Q') {
                state->game_over = 1;
            }
        }
        
        // Auto drop piece - no manual drop allowed
        if (!state->paused) {
            clock_gettime(CLOCK_MONOTONIC, &now);
            long elapsed_ms = (now.tv_sec - last_drop.tv_sec) * 1000 + 
                            (now.tv_nsec - last_drop.tv_nsec) / 1000000;
            
            if (elapsed_ms > state->drop_speed_ms) {
                move_piece_down(state);
                clock_gettime(CLOCK_MONOTONIC, &last_drop);
            }
        }
        
        usleep(16667); // ~60 FPS
    }
    
    show_cursor();
}

void game_loop(GameState *state) {
    if (state->game_mode == GAME_MODE_AUTO_DROP) {
        game_loop_auto_drop(state);
    } else {
        game_loop_normal(state);
    }
}

int main(void) {
    srand(time(NULL));
    init_terminal();
    
    int running = 1;
    while (running) {
        show_menu();
        
        char choice;
        if (read(STDIN_FILENO, &choice, 1) <= 0) {
            sleep(1);
            continue;
        }
        
        printf("%c\n", choice);
        
        if (choice == '1') {
            int drop_speed = get_drop_speed();
            clear_screen();
            GameState state;
            init_game(&state, GAME_MODE_NORMAL, drop_speed);
            game_loop(&state);
            
            clear_screen();
            printf("╔════════════════════════════════╗\n");
            printf("║       GAME OVER                 ║\n");
            printf("╠════════════════════════════════╣\n");
            printf("║  Final Score: %-15d  ║\n", state.score);
            printf("║  Level: %-20d      ║\n", state.level);
            printf("║  Lines Cleared: %-12d   ║\n", state.lines_cleared);
            printf("║                                 ║\n");
            printf("║  Press any key to return...    ║\n");
            printf("╚════════════════════════════════╝\n");
            fflush(stdout);
            
            read(STDIN_FILENO, &choice, 1);
        } else if (choice == '2') {
            int drop_speed = get_drop_speed();
            clear_screen();
            GameState state;
            init_game(&state, GAME_MODE_AUTO_DROP, drop_speed);
            game_loop(&state);
            
            clear_screen();
            printf("╔════════════════════════════════╗\n");
            printf("║       GAME OVER                 ║\n");
            printf("╠════════════════════════════════╣\n");
            printf("║  Final Score: %-15d  ║\n", state.score);
            printf("║  Level: %-20d      ║\n", state.level);
            printf("║  Lines Cleared: %-12d   ║\n", state.lines_cleared);
            printf("║                                 ║\n");
            printf("║  Press any key to return...    ║\n");
            printf("╚════════════════════════════════╝\n");
            fflush(stdout);
            
            read(STDIN_FILENO, &choice, 1);
        } else if (choice == '3') {
            running = 0;
        }
    }
    
    restore_terminal();
    printf("Thanks for playing Pepris!, Made by VibeCoding Birb (Yea not proud of that but hey I need to do something on the plane xD\n");
    
    return 0;
}
