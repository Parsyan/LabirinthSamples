// Compile: g++ -O2 -Wall -std=c++17 -o labyrinth_cpp labyrinth.cpp
// Run:     ./labyrinth_cpp
//
// This C++ version uses <vector> and <string> instead of raw arrays,
// which is the usual C++ way and a bit safer than the C version.

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <cctype>
#include <algorithm>

const int HEIGHT = 10;
const int LENGTH = 20;
const int ROCK_COUNT = 10;

using Board = std::vector<std::vector<char>>;

// All values that change during the game live in one struct.
// We pass it by reference (State&) so functions can read and modify it.
struct State {
    int player_row = 0;
    int player_col = 0;
    int flag_row = 0;
    int flag_col = 0;
    bool won = false;
};

// One shared random generator. Created once, reused everywhere.
std::mt19937& rng() {
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

int rand_int(int max) {
    std::uniform_int_distribution<int> dist(0, max - 1);
    return dist(rng());
}

// Forward declarations.
void place_rocks(Board& board, State& state);
bool path_exists(const Board& board, const State& state);
void try_move(Board& board, int new_row, int new_col, State& state);

Board create_world_board(int height, int length, State& state) {
    Board board(height, std::vector<char>(length, ' '));

    int player_row = rand_int(height);
    int player_col = rand_int(length);
    board[player_row][player_col] = '@';
    state.player_row = player_row;
    state.player_col = player_col;

    while (true) {
        int r = rand_int(height);
        int c = rand_int(length);
        if (board[r][c] == ' ') {
            board[r][c] = '&';
            state.flag_row = r;
            state.flag_col = c;
            break;
        }
    }

    place_rocks(board, state);
    return board;
}

void place_rocks(Board& board, State& state) {
    int placed = 0;
    int attempts = 0;
    int height = (int) board.size();
    int length = (int) board[0].size();

    while (placed < ROCK_COUNT && attempts < 1000) {
        attempts++;
        int r = rand_int(height);
        int c = rand_int(length);
        if (board[r][c] != ' ') continue;

        board[r][c] = '#';
        if (path_exists(board, state)) {
            placed++;
        } else {
            board[r][c] = ' ';
        }
    }
}

bool path_exists(const Board& board, const State& state) {
    int height = (int) board.size();
    int length = (int) board[0].size();

    std::vector<std::vector<bool>> visited(height, std::vector<bool>(length, false));
    std::vector<std::pair<int,int>> stack;
    stack.reserve(height * length);
    stack.push_back({state.player_row, state.player_col});
    visited[state.player_row][state.player_col] = true;

    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = { 0, 0,-1, 1};

    while (!stack.empty()) {
        auto [r, c] = stack.back();
        stack.pop_back();
        if (r == state.flag_row && c == state.flag_col) return true;

        for (int k = 0; k < 4; k++) {
            int nr = r + dr[k];
            int nc = c + dc[k];
            if (nr < 0 || nr >= height || nc < 0 || nc >= length) continue;
            if (visited[nr][nc]) continue;
            if (board[nr][nc] == '#') continue;
            visited[nr][nc] = true;
            stack.push_back({nr, nc});
        }
    }
    return false;
}

void move_up(Board& board, State& state) {
    try_move(board, state.player_row - 1, state.player_col, state);
}

void move_down(Board& board, State& state) {
    try_move(board, state.player_row + 1, state.player_col, state);
}

void move_left(Board& board, State& state) {
    try_move(board, state.player_row, state.player_col - 1, state);
}

void move_right(Board& board, State& state) {
    try_move(board, state.player_row, state.player_col + 1, state);
}

void try_move(Board& board, int new_row, int new_col, State& state) {
    int height = (int) board.size();
    int length = (int) board[0].size();
    int player_row = state.player_row;
    int player_col = state.player_col;

    if (new_row < 0 || new_row >= height || new_col < 0 || new_col >= length) {
        std::cout << "Out of bounds. You stay.\n";
        return;
    }

    char target = board[new_row][new_col];
    if (target == '#') {
        std::cout << "A rock blocks your way.\n";
        return;
    }
    if (target == '&') {
        board[player_row][player_col] = ' ';
        board[new_row][new_col] = '@';
        state.player_row = new_row;
        state.player_col = new_col;
        state.won = true;
        return;
    }

    board[player_row][player_col] = ' ';
    board[new_row][new_col] = '@';
    state.player_row = new_row;
    state.player_col = new_col;
}

void print_world(const Board& board) {
    int length = (int) board[0].size();
    std::string border = "+" + std::string(length, '-') + "+";
    std::cout << border << "\n";
    for (const auto& row : board) {
        std::cout << "|";
        for (char ch : row) std::cout << ch;
        std::cout << "|\n";
    }
    std::cout << border << "\n";
}

void print_user_manual() {
    std::cout << "=== LABYRINTH ===\n";
    std::cout << "Symbols:  @ = you   # = rock   & = flag (goal)\n";
    std::cout << "Controls: w = up, a = left, s = down, d = right\n";
    std::cout << "Reach the flag to win. Rocks block you.\n";
    std::cout << "\n";
}

char input_direction() {
    std::string line;
    while (true) {
        std::cout << "Direction (w/a/s/d): " << std::flush;
        if (!std::getline(std::cin, line)) {
            // End of input (Ctrl+D). Exit cleanly.
            std::cout << "\nGoodbye.\n";
            std::exit(0);
        }

        // Strip leading spaces/tabs.
        size_t i = 0;
        while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) i++;

        if (i >= line.size()) {
            std::cout << "Empty input is not allowed.\n";
            continue;
        }

        char ch = (char) std::tolower((unsigned char) line[i]);
        if (ch == 'w' || ch == 'a' || ch == 's' || ch == 'd') {
            return ch;
        }
        std::cout << "Use only w, a, s or d.\n";
    }
}

void labyrinth() {
    State state;

    print_user_manual();
    Board board = create_world_board(HEIGHT, LENGTH, state);
    print_world(board);

    while (!state.won) {
        char dir = input_direction();
        if (dir == 'w')      move_up(board, state);
        else if (dir == 's') move_down(board, state);
        else if (dir == 'a') move_left(board, state);
        else if (dir == 'd') move_right(board, state);

        print_world(board);

        if (state.won) {
            std::cout << "You reached the flag. You win!\n";
        }
    }
}

int main() {
    labyrinth();
    return 0;
}
