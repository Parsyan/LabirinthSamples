/* Compile: gcc -O2 -Wall -o labyrinth labyrinthByOOP.c
 * Run:     ./labyrinth
 *
 * C does NOT have classes, but the four OOP ideas still apply:
 *   - ABSTRACTION    : we describe "a Cell" by what it can do (give a
 *                      symbol, say if it blocks), not by what it is.
 *   - ENCAPSULATION  : Board/Game keep their data hidden inside their
 *                      struct; the outside world only calls their
 *                      "methods" (the board_... and game_... functions).
 *   - INHERITANCE    : Rock, Flag, Player, EmptyCell all "are a Cell".
 *                      In C we do this by putting a Cell as the FIRST
 *                      field of each subclass struct, so a pointer to
 *                      the subclass can be safely cast to Cell*.
 *   - POLYMORPHISM   : each Cell carries a small table of function
 *                      pointers (a "vtable"). Board just calls
 *                      cell->vt->get_symbol(cell) without ever asking
 *                      "what kind of cell are you?".
 *
 * Keep this file as a single .c on purpose: easier for beginners to
 * read top-to-bottom.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define ROCK_COUNT 10

/* ============================================================
 * Cell — the abstract base "class".
 *
 * A Cell struct only holds what every cell has in common: its
 * position and a pointer to its vtable. The vtable is what makes
 * polymorphism work in C.
 * ============================================================ */

struct Cell;  /* forward declaration, vtable refers to Cell */

typedef struct CellVTable {
    /* Each subclass fills these in. The Board calls them through
     * the pointer and gets the right behavior automatically. */
    char (*get_symbol)(const struct Cell *self);
    int  (*is_blocking)(const struct Cell *self);  /* 0 = no, 1 = yes */
    void (*destroy)(struct Cell *self);            /* free memory */
} CellVTable;

typedef struct Cell {
    const CellVTable *vt;  /* points at one of the *_vtable globals below */
    int row;
    int col;
} Cell;

/* Public "methods" of Cell. They just forward to the vtable.
 * Beginner tip: read "cell_get_symbol(c)" as "c.getSymbol()". */
static char cell_get_symbol(const Cell *self) {
    return self->vt->get_symbol(self);
}

static int cell_is_blocking(const Cell *self) {
    return self->vt->is_blocking(self);
}

static void cell_destroy(Cell *self) {
    if (self != NULL) self->vt->destroy(self);
}

/* setPosition is the same for every kind of cell, so it lives here
 * once instead of being in the vtable. */
static void cell_set_position(Cell *self, int row, int col) {
    self->row = row;
    self->col = col;
}

/* Default "is_blocking": no. Subclasses that DO block override this. */
static int default_not_blocking(const struct Cell *self) {
    (void) self;
    return 0;
}

/* Default destroy: every subclass we have here is one flat struct,
 * so a single free is enough. */
static void default_destroy(struct Cell *self) {
    free(self);
}

/* ============================================================
 * EmptyCell : Cell — a walkable empty square.
 *
 * Notice the first field is a Cell. That is how we get
 * "inheritance" in C: a pointer to EmptyCell is also a valid
 * pointer to its first field, a Cell.
 * ============================================================ */
typedef struct {
    Cell base;
} EmptyCell;

static char empty_get_symbol(const struct Cell *self) {
    (void) self;
    return ' ';
}

static const CellVTable EMPTY_VTABLE = {
    empty_get_symbol,
    default_not_blocking,
    default_destroy
};

static Cell *empty_new(int row, int col) {
    EmptyCell *e = (EmptyCell *) malloc(sizeof(EmptyCell));
    e->base.vt  = &EMPTY_VTABLE;
    e->base.row = row;
    e->base.col = col;
    return (Cell *) e;  /* safe: base is the first field */
}

/* ============================================================
 * Rock : Cell — blocks movement.
 * ============================================================ */
typedef struct {
    Cell base;
} Rock;

static char rock_get_symbol(const struct Cell *self) {
    (void) self;
    return '#';
}

static int rock_is_blocking(const struct Cell *self) {
    (void) self;
    return 1;
}

static const CellVTable ROCK_VTABLE = {
    rock_get_symbol,
    rock_is_blocking,
    default_destroy
};

static Cell *rock_new(int row, int col) {
    Rock *r = (Rock *) malloc(sizeof(Rock));
    r->base.vt  = &ROCK_VTABLE;
    r->base.row = row;
    r->base.col = col;
    return (Cell *) r;
}

/* ============================================================
 * Flag : Cell — the goal. Not blocking; stepping here wins.
 * ============================================================ */
typedef struct {
    Cell base;
} Flag;

static char flag_get_symbol(const struct Cell *self) {
    (void) self;
    return '&';
}

static const CellVTable FLAG_VTABLE = {
    flag_get_symbol,
    default_not_blocking,
    default_destroy
};

static Cell *flag_new(int row, int col) {
    Flag *f = (Flag *) malloc(sizeof(Flag));
    f->base.vt  = &FLAG_VTABLE;
    f->base.row = row;
    f->base.col = col;
    return (Cell *) f;
}

/* ============================================================
 * Player : Cell — the cell the user controls.
 * ============================================================ */
typedef struct {
    Cell base;
} Player;

static char player_get_symbol(const struct Cell *self) {
    (void) self;
    return '@';
}

static const CellVTable PLAYER_VTABLE = {
    player_get_symbol,
    default_not_blocking,
    default_destroy
};

static Cell *player_new(int row, int col) {
    Player *p = (Player *) malloc(sizeof(Player));
    p->base.vt  = &PLAYER_VTABLE;
    p->base.row = row;
    p->base.col = col;
    return (Cell *) p;
}

/* Small helpers for "what kind of cell is this?". We compare the
 * vtable pointer instead of using RTTI. This is C's version of
 * "instanceof". We only need it in a couple of places. */
static int is_flag(const Cell *c)   { return c->vt == &FLAG_VTABLE; }
static int is_empty(const Cell *c)  { return c->vt == &EMPTY_VTABLE; }

/* ============================================================
 * Board — owns the 2D grid of Cell* and knows how to build it,
 * draw it, and move the player. ENCAPSULATION: callers never
 * touch board->grid directly; they go through board_* functions.
 * ============================================================ */
typedef struct {
    Cell ***grid;     /* grid[row][col] is a Cell* */
    short height;
    short length;
    Cell *player;     /* same pointer also lives in the grid */
    Cell *flag;       /* same pointer also lives in the grid */
    int won;          /* 0 = no, 1 = yes */
} Board;

/* Forward declarations of Board's "private methods". */
static void board_create_world(Board *self);
static void board_place_rocks(Board *self);
static int  board_path_exists(const Board *self);
static void board_try_move(Board *self, int new_row, int new_col);

/* "Constructor". Allocates the board, fills it in, and returns it.
 * The caller must later call board_destroy(b). */
static Board *board_new(short height, short length) {
    Board *b = (Board *) malloc(sizeof(Board));
    b->height = height;
    b->length = length;
    b->won    = 0;
    b->player = NULL;
    b->flag   = NULL;

    b->grid = (Cell ***) malloc(sizeof(Cell **) * height);
    for (int i = 0; i < height; i++) {
        b->grid[i] = (Cell **) malloc(sizeof(Cell *) * length);
        for (int j = 0; j < length; j++) {
            b->grid[i][j] = NULL;
        }
    }

    board_create_world(b);
    return b;
}

static void board_destroy(Board *self) {
    if (self == NULL) return;
    for (int i = 0; i < self->height; i++) {
        for (int j = 0; j < self->length; j++) {
            cell_destroy(self->grid[i][j]);
        }
        free(self->grid[i]);
    }
    free(self->grid);
    free(self);
}

static int board_is_won(const Board *self) {
    return self->won;
}

/* ---- World generation -----------------------------------------
 * Flow:
 *   1) fill the whole grid with EmptyCell
 *   2) place the Player at a random spot
 *   3) place the Flag at a different random empty spot
 *   4) place ROCK_COUNT rocks, each only if a path still exists
 */
static void board_create_world(Board *self) {
    for (int i = 0; i < self->height; i++) {
        for (int j = 0; j < self->length; j++) {
            self->grid[i][j] = empty_new(i, j);
        }
    }

    int pr = rand() % self->height;
    int pc = rand() % self->length;
    cell_destroy(self->grid[pr][pc]);
    self->player = player_new(pr, pc);
    self->grid[pr][pc] = self->player;

    while (1) {
        int r = rand() % self->height;
        int c = rand() % self->length;
        if (is_empty(self->grid[r][c])) {
            cell_destroy(self->grid[r][c]);
            self->flag = flag_new(r, c);
            self->grid[r][c] = self->flag;
            break;
        }
    }

    board_place_rocks(self);
}

/* Try to place ROCK_COUNT rocks. After each candidate rock we
 * check that the player can still reach the flag; if not, we
 * put an empty cell back where the rock was. */
static void board_place_rocks(Board *self) {
    int placed = 0;
    int attempts = 0;
    while (placed < ROCK_COUNT && attempts < 1000) {
        attempts++;
        int r = rand() % self->height;
        int c = rand() % self->length;
        if (!is_empty(self->grid[r][c])) continue;

        cell_destroy(self->grid[r][c]);
        self->grid[r][c] = rock_new(r, c);

        if (board_path_exists(self)) {
            placed++;
        } else {
            cell_destroy(self->grid[r][c]);
            self->grid[r][c] = empty_new(r, c);
        }
    }
}

/* DFS from the player's position to the flag's position,
 * walking over any non-blocking cell. POLYMORPHISM in action:
 * we just call cell_is_blocking(...) on whatever sits there. */
static int board_path_exists(const Board *self) {
    int h = self->height, w = self->length;
    int *visited = (int *) calloc((size_t) h * w, sizeof(int));
    int *stack_r = (int *) malloc(sizeof(int) * h * w);
    int *stack_c = (int *) malloc(sizeof(int) * h * w);
    int top = 0;
    int found = 0;

    stack_r[top] = self->player->row;
    stack_c[top] = self->player->col;
    top++;
    visited[self->player->row * w + self->player->col] = 1;

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = { 0, 0,-1, 1};

    while (top > 0) {
        top--;
        int r = stack_r[top];
        int c = stack_c[top];
        if (r == self->flag->row && c == self->flag->col) {
            found = 1;
            break;
        }

        for (int k = 0; k < 4; k++) {
            int nr = r + dr[k];
            int nc = c + dc[k];
            if (nr < 0 || nr >= h || nc < 0 || nc >= w) continue;
            if (visited[nr * w + nc]) continue;
            if (cell_is_blocking(self->grid[nr][nc])) continue;
            visited[nr * w + nc] = 1;
            stack_r[top] = nr;
            stack_c[top] = nc;
            top++;
        }
    }

    free(visited);
    free(stack_r);
    free(stack_c);
    return found;
}

/* ---- Movement -------------------------------------------------
 * The four public move_* functions just compute the target square
 * and hand off to board_try_move, which has the shared rules. */
static void board_move_up(Board *self) {
    board_try_move(self, self->player->row - 1, self->player->col);
}
static void board_move_down(Board *self) {
    board_try_move(self, self->player->row + 1, self->player->col);
}
static void board_move_left(Board *self) {
    board_try_move(self, self->player->row, self->player->col - 1);
}
static void board_move_right(Board *self) {
    board_try_move(self, self->player->row, self->player->col + 1);
}

static void board_try_move(Board *self, int new_row, int new_col) {
    if (new_row < 0 || new_row >= self->height ||
        new_col < 0 || new_col >= self->length) {
        printf("Out of bounds. You stay.\n");
        return;
    }

    Cell *target = self->grid[new_row][new_col];

    if (cell_is_blocking(target)) {
        printf("A rock blocks your way.\n");
        return;
    }

    int pr = self->player->row;
    int pc = self->player->col;

    if (is_flag(target)) {
        /* Walk onto the flag. The flag cell is replaced by the player
         * pointer; the old player spot becomes a fresh EmptyCell. */
        cell_destroy(self->grid[new_row][new_col]);  /* free flag */
        self->flag = NULL;
        self->grid[pr][pc] = empty_new(pr, pc);
        cell_set_position(self->player, new_row, new_col);
        self->grid[new_row][new_col] = self->player;
        self->won = 1;
        return;
    }

    /* Target is an EmptyCell: free it, swap the player in. */
    cell_destroy(self->grid[new_row][new_col]);
    self->grid[pr][pc] = empty_new(pr, pc);
    cell_set_position(self->player, new_row, new_col);
    self->grid[new_row][new_col] = self->player;
}

/* ---- Drawing --------------------------------------------------
 * POLYMORPHISM: we don't ask "what kind of cell?" — we just ask
 * each cell for its symbol. */
static void board_print_world(const Board *self) {
    printf("+");
    for (int j = 0; j < self->length; j++) printf("-");
    printf("+\n");
    for (int i = 0; i < self->height; i++) {
        printf("|");
        for (int j = 0; j < self->length; j++) {
            printf("%c", cell_get_symbol(self->grid[i][j]));
        }
        printf("|\n");
    }
    printf("+");
    for (int j = 0; j < self->length; j++) printf("-");
    printf("+\n");
}

/* ============================================================
 * Game — runs the user-facing loop: manual, input, movement, win.
 * It owns a Board.
 * ============================================================ */
typedef struct {
    Board *board;
} Game;

static Game *game_new(short height, short length) {
    Game *g = (Game *) malloc(sizeof(Game));
    g->board = board_new(height, length);
    return g;
}

static void game_destroy(Game *self) {
    if (self == NULL) return;
    board_destroy(self->board);
    free(self);
}

static void game_print_user_manual(const Game *self) {
    (void) self;
    printf("=== LABYRINTH ===\n");
    printf("Symbols:  @ = you   # = rock   & = flag (goal)\n");
    printf("Controls: w = up, a = left, s = down, d = right\n");
    printf("Reach the flag to win. Rocks block you.\n");
    printf("\n");
}

static char game_input_direction(const Game *self) {
    (void) self;
    char buffer[128];
    while (1) {
        printf("Direction (w/a/s/d): ");
        fflush(stdout);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\nGoodbye.\n");
            exit(0);
        }

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

static void game_labyrinth(Game *self) {
    game_print_user_manual(self);
    board_print_world(self->board);

    while (!board_is_won(self->board)) {
        char dir = game_input_direction(self);
        if      (dir == 'w') board_move_up(self->board);
        else if (dir == 's') board_move_down(self->board);
        else if (dir == 'a') board_move_left(self->board);
        else if (dir == 'd') board_move_right(self->board);

        board_print_world(self->board);

        if (board_is_won(self->board)) {
            printf("You reached the flag. You win!\n");
        }
    }
}

/* ============================================================
 * Entry point. Kept tiny on purpose: build a Game and run it.
 * ============================================================ */
int main(void) {
    srand((unsigned int) time(NULL));
    Game *game = game_new((short) 10, (short) 20);
    game_labyrinth(game);
    game_destroy(game);
    return 0;
}
