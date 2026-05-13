import java.util.Random;
import java.util.Scanner;

public class Labyrinth {

    public static char[][] createWorldBoard(short height, short length, int[] state, Random random) {
        char[][] board = new char[height][length];

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < length; j++) {
                board[i][j] = ' ';
            }
        }

        int playerRow = random.nextInt(height);
        int playerCol = random.nextInt(length);
        board[playerRow][playerCol] = '@';
        state[0] = playerRow;
        state[1] = playerCol;

        while (true) {
            int r = random.nextInt(height);
            int c = random.nextInt(length);
            if (board[r][c] == ' ') {
                board[r][c] = '&';
                state[2] = r;
                state[3] = c;
                break;
            }
        }

        placeRocks(board, height, length, state, random);

        return board;
    }

    public static void placeRocks(char[][] board, short height, short length, int[] state, Random random) {
        int placed = 0;
        int attempts = 0;
        while (placed < 10 && attempts < 1000) {
            attempts++;
            int r = random.nextInt(height);
            int c = random.nextInt(length);
            if (board[r][c] != ' ') continue;

            board[r][c] = '#';
            if (pathExists(board, height, length, state)) {
                placed++;
            } else {
                board[r][c] = ' ';
            }
        }
    }

    public static boolean pathExists(char[][] board, short height, short length, int[] state) {
        int playerRow = state[0];
        int playerCol = state[1];
        int flagRow = state[2];
        int flagCol = state[3];

        boolean[][] visited = new boolean[height][length];
        int[] stackR = new int[height * length];
        int[] stackC = new int[height * length];
        int top = 0;
        stackR[top] = playerRow;
        stackC[top] = playerCol;
        top++;
        visited[playerRow][playerCol] = true;

        while (top > 0) {
            top--;
            int r = stackR[top];
            int c = stackC[top];
            if (r == flagRow && c == flagCol) return true;

            int[] dr = {-1, 1, 0, 0};
            int[] dc = {0, 0, -1, 1};
            for (int k = 0; k < 4; k++) {
                int nr = r + dr[k];
                int nc = c + dc[k];
                if (nr < 0 || nr >= height || nc < 0 || nc >= length) continue;
                if (visited[nr][nc]) continue;
                if (board[nr][nc] == '#') continue;
                visited[nr][nc] = true;
                stackR[top] = nr;
                stackC[top] = nc;
                top++;
            }
        }
        return false;
    }

    public static void moveUp(char[][] board, int[] state) {
        tryMove(board, state[0] - 1, state[1], state);
    }

    public static void moveDown(char[][] board, int[] state) {
        tryMove(board, state[0] + 1, state[1], state);
    }

    public static void moveLeft(char[][] board, int[] state) {
        tryMove(board, state[0], state[1] - 1, state);
    }

    public static void moveRight(char[][] board, int[] state) {
        tryMove(board, state[0], state[1] + 1, state);
    }

    public static void tryMove(char[][] board, int newRow, int newCol, int[] state) {
        int playerRow = state[0];
        int playerCol = state[1];

        if (newRow < 0 || newRow >= board.length || newCol < 0 || newCol >= board[0].length) {
            System.out.println("Out of bounds. You stay.");
            return;
        }
        char target = board[newRow][newCol];
        if (target == '#') {
            System.out.println("A rock blocks your way.");
            return;
        }
        if (target == '&') {
            board[playerRow][playerCol] = ' ';
            board[newRow][newCol] = '@';
            state[0] = newRow;
            state[1] = newCol;
            state[4] = 1;
            return;
        }
        board[playerRow][playerCol] = ' ';
        board[newRow][newCol] = '@';
        state[0] = newRow;
        state[1] = newCol;
    }

    public static void printWorld(char[][] board) {
        int length = board[0].length;
        System.out.print("+");
        for (int j = 0; j < length; j++) System.out.print("-");
        System.out.println("+");
        for (int i = 0; i < board.length; i++) {
            System.out.print("|");
            for (int j = 0; j < length; j++) {
                System.out.print(board[i][j]);
            }
            System.out.println("|");
        }
        System.out.print("+");
        for (int j = 0; j < length; j++) System.out.print("-");
        System.out.println("+");
    }

    public static void printUserManual() {
        System.out.println("=== LABYRINTH ===");
        System.out.println("Symbols:  @ = you   # = rock   & = flag (goal)");
        System.out.println("Controls: w = up, a = left, s = down, d = right");
        System.out.println("Reach the flag to win. Rocks block you.");
        System.out.println();
    }

    public static char inputDirection(Scanner scanner) {
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

    public static void labyrinth() {
        int[] state = new int[5];
        Scanner scanner = new Scanner(System.in);
        Random random = new Random();

        printUserManual();
        char[][] board = createWorldBoard((short) 10, (short) 20, state, random);
        printWorld(board);

        while (state[4] == 0) {
            char dir = inputDirection(scanner);
            if (dir == 'w') moveUp(board, state);
            else if (dir == 's') moveDown(board, state);
            else if (dir == 'a') moveLeft(board, state);
            else if (dir == 'd') moveRight(board, state);

            printWorld(board);

            if (state[4] == 1) {
                System.out.println("You reached the flag. You win!");
            }
        }
    }

    public static void main(String[] args) {
        labyrinth();
    }
}
