import java.util.Random;
import java.util.Scanner;

// ------------------------------------------------------------------
// Cell — abstract base for everything that can sit on the board.
// This is ABSTRACTION: we say "a cell has a symbol", without yet
// caring whether it is a rock, a player, a flag or empty space.
// ------------------------------------------------------------------
abstract class Cell {
    // ENCAPSULATION: the position is private. Outside code reads it
    // through getters and changes it through setPosition(...).
    private int row;
    private int col;

    public Cell(int row, int col) {
        this.row = row;
        this.col = col;
    }

    public int getRow() { return row; }
    public int getCol() { return col; }

    public void setPosition(int row, int col) {
        this.row = row;
        this.col = col;
    }

    // POLYMORPHISM: every subclass decides what symbol to draw.
    // The Board just calls cell.getSymbol() without knowing the type.
    public abstract char getSymbol();

    // By default a cell blocks no one. Rock will override this.
    public boolean isBlocking() {
        return false;
    }
}

// ------------------------------------------------------------------
// Empty cell — a place the player can walk into.
// INHERITANCE: it extends Cell and only fills in the symbol.
// ------------------------------------------------------------------
class EmptyCell extends Cell {
    public EmptyCell(int row, int col) {
        super(row, col);
    }

    @Override
    public char getSymbol() {
        return ' ';
    }
}

// ------------------------------------------------------------------
// Rock — blocks movement.
// ------------------------------------------------------------------
class Rock extends Cell {
    public Rock(int row, int col) {
        super(row, col);
    }

    @Override
    public char getSymbol() {
        return '#';
    }

    @Override
    public boolean isBlocking() {
        return true;
    }
}

// ------------------------------------------------------------------
// Flag — the goal. Not blocking; stepping on it wins the game.
// ------------------------------------------------------------------
class Flag extends Cell {
    public Flag(int row, int col) {
        super(row, col);
    }

    @Override
    public char getSymbol() {
        return '&';
    }
}

// ------------------------------------------------------------------
// Player — the cell the user controls.
// ------------------------------------------------------------------
class Player extends Cell {
    public Player(int row, int col) {
        super(row, col);
    }

    @Override
    public char getSymbol() {
        return '@';
    }
}

// ------------------------------------------------------------------
// Board — owns the 2D grid of cells and knows how to build it,
// print it, and move the player. ENCAPSULATION: the grid itself
// is private; the outside world talks to the Board only through
// its public methods.
// ------------------------------------------------------------------
class Board {
    private final Cell[][] grid;
    private final int height;
    private final int length;
    private Player player;
    private Flag flag;
    private boolean won;

    private static final int ROCK_COUNT = 10;

    public Board(short height, short length, Random random) {
        this.height = height;
        this.length = length;
        this.grid = new Cell[height][length];
        this.won = false;
        createWorldBoard(random);
    }

    public boolean isWon() {
        return won;
    }

    // ---- World generation -----------------------------------------
    // The flow:
    //   1) fill everything with EmptyCell
    //   2) place the player at a random spot
    //   3) place the flag at a different random empty spot
    //   4) place 10 rocks, each one only if a path still exists
    private void createWorldBoard(Random random) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < length; j++) {
                grid[i][j] = new EmptyCell(i, j);
            }
        }

        int pr = random.nextInt(height);
        int pc = random.nextInt(length);
        player = new Player(pr, pc);
        grid[pr][pc] = player;

        while (true) {
            int r = random.nextInt(height);
            int c = random.nextInt(length);
            if (grid[r][c] instanceof EmptyCell) {
                flag = new Flag(r, c);
                grid[r][c] = flag;
                break;
            }
        }

        placeRocks(random);
    }

    // Try to place ROCK_COUNT rocks. After each candidate rock
    // we check that the player can still reach the flag; if not,
    // we put the empty cell back.
    private void placeRocks(Random random) {
        int placed = 0;
        int attempts = 0;
        while (placed < ROCK_COUNT && attempts < 1000) {
            attempts++;
            int r = random.nextInt(height);
            int c = random.nextInt(length);
            if (!(grid[r][c] instanceof EmptyCell)) continue;

            Cell backup = grid[r][c];
            grid[r][c] = new Rock(r, c);
            if (pathExists()) {
                placed++;
            } else {
                grid[r][c] = backup;
            }
        }
    }

    // DFS from the player's position to the flag's position,
    // walking over any non-blocking cell.
    private boolean pathExists() {
        boolean[][] visited = new boolean[height][length];
        int[] stackR = new int[height * length];
        int[] stackC = new int[height * length];
        int top = 0;

        stackR[top] = player.getRow();
        stackC[top] = player.getCol();
        top++;
        visited[player.getRow()][player.getCol()] = true;

        int[] dr = {-1, 1, 0, 0};
        int[] dc = {0, 0, -1, 1};

        while (top > 0) {
            top--;
            int r = stackR[top];
            int c = stackC[top];
            if (r == flag.getRow() && c == flag.getCol()) return true;

            for (int k = 0; k < 4; k++) {
                int nr = r + dr[k];
                int nc = c + dc[k];
                if (nr < 0 || nr >= height || nc < 0 || nc >= length) continue;
                if (visited[nr][nc]) continue;
                if (grid[nr][nc].isBlocking()) continue;
                visited[nr][nc] = true;
                stackR[top] = nr;
                stackC[top] = nc;
                top++;
            }
        }
        return false;
    }

    // ---- Movement -------------------------------------------------
    // The four public move* methods just compute the target square
    // and hand off to tryMove, which has the shared rules.
    public void moveUp()    { tryMove(player.getRow() - 1, player.getCol()); }
    public void moveDown()  { tryMove(player.getRow() + 1, player.getCol()); }
    public void moveLeft()  { tryMove(player.getRow(),     player.getCol() - 1); }
    public void moveRight() { tryMove(player.getRow(),     player.getCol() + 1); }

    private void tryMove(int newRow, int newCol) {
        if (newRow < 0 || newRow >= height || newCol < 0 || newCol >= length) {
            System.out.println("Out of bounds. You stay.");
            return;
        }

        Cell target = grid[newRow][newCol];

        if (target.isBlocking()) {
            System.out.println("A rock blocks your way.");
            return;
        }

        if (target instanceof Flag) {
            grid[player.getRow()][player.getCol()] = new EmptyCell(player.getRow(), player.getCol());
            player.setPosition(newRow, newCol);
            grid[newRow][newCol] = player;
            won = true;
            return;
        }

        // target is an EmptyCell: walk in.
        grid[player.getRow()][player.getCol()] = new EmptyCell(player.getRow(), player.getCol());
        player.setPosition(newRow, newCol);
        grid[newRow][newCol] = player;
    }

    // ---- Drawing --------------------------------------------------
    // POLYMORPHISM in action: we don't ask "what kind of cell?",
    // we just ask each cell for its symbol.
    public void printWorld() {
        System.out.print("+");
        for (int j = 0; j < length; j++) System.out.print("-");
        System.out.println("+");
        for (int i = 0; i < height; i++) {
            System.out.print("|");
            for (int j = 0; j < length; j++) {
                System.out.print(grid[i][j].getSymbol());
            }
            System.out.println("|");
        }
        System.out.print("+");
        for (int j = 0; j < length; j++) System.out.print("-");
        System.out.println("+");
    }
}

// ------------------------------------------------------------------
// Game — runs the user-facing loop: manual, input, movement, win.
// It owns a Board and a Scanner.
// ------------------------------------------------------------------
class Game {
    private final Board board;
    private final Scanner scanner;

    public Game(short height, short length) {
        this.board = new Board(height, length, new Random());
        this.scanner = new Scanner(System.in);
    }

    public void printUserManual() {
        System.out.println("=== LABYRINTH ===");
        System.out.println("Symbols:  @ = you   # = rock   & = flag (goal)");
        System.out.println("Controls: w = up, a = left, s = down, d = right");
        System.out.println("Reach the flag to win. Rocks block you.");
        System.out.println();
    }

    public char inputDirection() {
        while (true) {
            System.out.print("Direction (w/a/s/d): ");
            String line = scanner.nextLine();
            if (line == null) continue;
            line = line.trim();
            if (line.isEmpty()) {
                System.out.println("Empty input is not allowed.");
                continue;
            }
            char ch = Character.toLowerCase(line.charAt(0));
            if (ch == 'w' || ch == 'a' || ch == 's' || ch == 'd') {
                return ch;
            }
            System.out.println("Use only w, a, s or d.");
        }
    }

    public void labyrinth() {
        printUserManual();
        board.printWorld();

        while (!board.isWon()) {
            char dir = inputDirection();
            if      (dir == 'w') board.moveUp();
            else if (dir == 's') board.moveDown();
            else if (dir == 'a') board.moveLeft();
            else if (dir == 'd') board.moveRight();

            board.printWorld();

            if (board.isWon()) {
                System.out.println("You reached the flag. You win!");
            }
        }
    }
}

// ------------------------------------------------------------------
// Entry point. Kept tiny on purpose: it just builds a Game and runs.
// ------------------------------------------------------------------
public class LabyrinthByOOP {
    public static void main(String[] args) {
        new Game((short) 10, (short) 20).labyrinth();
    }
}
