/* Compile: gcc -O2 -Wall -o labyrinth labyrinth.c
 * Run:     ./labyrinth
 *
 * Fixed board size keeps things simple for beginners. If you change
 * HEIGHT or LENGTH, everything else still works.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define HEIGHT 10
#define LENGTH 20
#define ROCK_COUNT 10

/* Game state. C cannot return several values from one function easily,
 * so we put everything that might change into one struct and pass it
 * by pointer. Functions that take "State *state" can read AND change it. */
typedef struct {
    int player_row;
    int player_col;
    int flag_row;
    int flag_col;
    int won;   /* 0 = no, 1 = yes */
} State;

/* Forward declarations so we can call functions before they appear below. */
void create_world_board(char board[HEIGHT][LENGTH], State *state);
void place_rocks(char board[HEIGHT][LENGTH], State *state);
int  path_exists(char board[HEIGHT][LENGTH], State *state);
void move_up(char board[HEIGHT][LENGTH], State *state);
void move_down(char board[HEIGHT][LENGTH], State *state);
void move_left(char board[HEIGHT][LENGTH], State *state);
void move_right(char board[HEIGHT][LENGTH], State *state);
void try_move(char board[HEIGHT][LENGTH], int new_row, int new_col, State *state);
void print_world(char board[HEIGHT][LENGTH]);
void print_user_manual(void);
char input_direction(void);
void labyrinth(void);

void create_world_board(char board[HEIGHT][LENGTH], State *state) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < LENGTH; j++) {
            board[i][j] = ' ';
        }
    }

    int player_row = rand() % HEIGHT;
    int player_col = rand() % LENGTH;
    board[player_row][player_col] = '@';
    state->player_row = player_row;
    state->player_col = player_col;

    while (1) {
        int r = rand() % HEIGHT;
        int c = rand() % LENGTH;
        if (board[r][c] == ' ') {
            board[r][c] = '&';
            state->flag_row = r;
            state->flag_col = c;
            break;
        }
    }

    place_rocks(board, state);
}

void place_rocks(char board[HEIGHT][LENGTH], State *state) {
    int placed = 0;
    int attempts = 0;
    while (placed < ROCK_COUNT && attempts < 1000) {
        attempts++;
        int r = rand() % HEIGHT;
        int c = rand() % LENGTH;
        if (board[r][c] != ' ') continue;

        board[r][c] = '#';
        if (path_exists(board, state)) {
            placed++;
        } else {
            board[r][c] = ' ';
        }
    }
}

int path_exists(char board[HEIGHT][LENGTH], State *state) {
    int start_row = state->player_row;
    int start_col = state->player_col;
    int flag_row  = state->flag_row;
    int flag_col  = state->flag_col;

    int visited[HEIGHT][LENGTH];
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < LENGTH; j++)
            visited[i][j] = 0;

    int stack_r[HEIGHT * LENGTH];
    int stack_c[HEIGHT * LENGTH];
    int top = 0;
    stack_r[top] = start_row;
    stack_c[top] = start_col;
    top++;
    visited[start_row][start_col] = 1;

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = { 0, 0,-1, 1};

    while (top > 0) {
        top--;
        int r = stack_r[top];
        int c = stack_c[top];
        if (r == flag_row && c == flag_col) return 1;

        for (int k = 0; k < 4; k++) {
            int nr = r + dr[k];
            int nc = c + dc[k];
            if (nr < 0 || nr >= HEIGHT || nc < 0 || nc >= LENGTH) continue;
            if (visited[nr][nc]) continue;
            if (board[nr][nc] == '#') continue;
            visited[nr][nc] = 1;
            stack_r[top] = nr;
            stack_c[top] = nc;
            top++;
        }
    }
    return 0;
}

void move_up(char board[HEIGHT][LENGTH], State *state) {
    try_move(board, state->player_row - 1, state->player_col, state);
}

void move_down(char board[HEIGHT][LENGTH], State *state) {
    try_move(board, state->player_row + 1, state->player_col, state);
}

void move_left(char board[HEIGHT][LENGTH], State *state) {
    try_move(board, state->player_row, state->player_col - 1, state);
}

void move_right(char board[HEIGHT][LENGTH], State *state) {
    try_move(board, state->player_row, state->player_col + 1, state);
}

void try_move(char board[HEIGHT][LENGTH], int new_row, int new_col, State *state) {
    int player_row = state->player_row;
    int player_col = state->player_col;

    if (new_row < 0 || new_row >= HEIGHT || new_col < 0 || new_col >= LENGTH) {
        printf("Out of bounds. You stay.\n");
        return;
    }

    char target = board[new_row][new_col];
    if (target == '#') {
        printf("A rock blocks your way.\n");
        return;
    }
    if (target == '&') {
        board[player_row][player_col] = ' ';
        board[new_row][new_col] = '@';
        state->player_row = new_row;
        state->player_col = new_col;
        state->won = 1;
        return;
    }

    board[player_row][player_col] = ' ';
    board[new_row][new_col] = '@';
    state->player_row = new_row;
    state->player_col = new_col;
}

void print_world(char board[HEIGHT][LENGTH]) {
    printf("+");
    for (int j = 0; j < LENGTH; j++) printf("-");
    printf("+\n");
    for (int i = 0; i < HEIGHT; i++) {
        printf("|");
        for (int j = 0; j < LENGTH; j++) {
            printf("%c", board[i][j]);
        }
        printf("|\n");
    }
    printf("+");
    for (int j = 0; j < LENGTH; j++) printf("-");
    printf("+\n");
}

void print_user_manual(void) {
    printf("=== LABYRINTH ===\n");
    printf("Symbols:  @ = you   # = rock   & = flag (goal)\n");
    printf("Controls: w = up, a = left, s = down, d = right\n");
    printf("Reach the flag to win. Rocks block you.\n");
    printf("\n");
}

char input_direction(void) {
    char buffer[128];
    while (1) {
        printf("Direction (w/a/s/d): ");
        fflush(stdout);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            /* End of input (Ctrl+D). Exit the program. */
            printf("\nGoodbye.\n");
            exit(0);
        }

        /* Skip leading spaces. */
        int i = 0;
        while (buffer[i] == ' ' || buffer[i] == '\t') i++;

        if (buffer[i] == '\n' || buffer[i] == '\0') {
            printf("Empty input is not allowed.\n");
            continue;
        }

        char ch = (char) tolower((unsigned char) buffer[i]);
        if (ch == 'w' || ch == 'a' || ch == 's' || ch == 'd') {
            return ch;
        }
        printf("Use only w, a, s or d.\n");
    }
}

void labyrinth(void) {
    State state = {0, 0, 0, 0, 0};
    char board[HEIGHT][LENGTH];

    print_user_manual();
    create_world_board(board, &state);
    print_world(board);

    while (state.won == 0) {
        char dir = input_direction();
        if (dir == 'w')      move_up(board, &state);
        else if (dir == 's') move_down(board, &state);
        else if (dir == 'a') move_left(board, &state);
        else if (dir == 'd') move_right(board, &state);

        print_world(board);

        if (state.won == 1) {
            printf("You reached the flag. You win!\n");
        }
    }
}

int main(void) {
    srand((unsigned int) time(NULL));
    labyrinth();
    return 0;
}
