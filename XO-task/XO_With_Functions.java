import java.util.Scanner;

public class XO_With_Functions {
    public static void main(String[] args) {
        char[][] board = new char[3][3];
        Scanner scan = new Scanner(System.in);

        // 1. Տախտակը լցնում ենք '-' - ով, որպեսզի տպելիս երևա, որտեղ դեռ խաղ չկա
        for (int i = 0; i < board.length; i++) {
            for (int j = 0; j < board[i].length; j++) {
                board[i][j] = '-';
            }
        }
        int x, y;
        boolean gameOver = false;

        // 2. Հերթական խաղադարձ - մինչև տախտակը լցվի կամ ինչ-որ մեկը հաղթի
        for (int i = 1; i <= board.length * board[0].length && !gameOver; i++) {
            char player = (i % 2 != 0) ? 'X' : 'O';
            System.out.println(player + " turn!");

            System.out.print("Input x coordinate (0-2): ");
            x = scan.nextInt();
            System.out.print("Input y coordinate (0-2): ");
            y = scan.nextInt();

            // 3. Ստուգում ենք՝ ճիշտ կոորդինատներ, դատարկ խցիկ
            if (x < 0 || x > 2 || y < 0 || y > 2) {
                System.out.println("Out of bounds! Try again.");
                i--;
                continue;
            }
            if (board[x][y] != '-') {
                System.out.println("Cell already taken! Try again.");
                i--;
                continue;
            }
            board[x][y] = player;

            // 4. Հաղթանակի ստուգում - տողեր, սյուներ, անկյունագծեր
            for (int k = 0; k < 3; k++) {
                if (board[k][0] != '-' && board[k][0] == board[k][1] && board[k][1] == board[k][2]) {
                    System.out.println(board[k][0] + " is winner!");
                    gameOver = true;
                }
                if (board[0][k] != '-' && board[0][k] == board[1][k] && board[1][k] == board[2][k]) {
                    System.out.println(board[0][k] + " is winner!");
                    gameOver = true;
                }
            }
            if (board[0][0] != '-' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
                System.out.println(board[0][0] + " is winner!");
                gameOver = true;
            }
            if (board[0][2] != '-' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
                System.out.println(board[0][2] + " is winner!");
                gameOver = true;
            }

            // 5. Տպել տախտակը
            for (char[] r : board) {
                System.out.print("[ ");
                for (char c : r) {
                    System.out.print(c + " ");
                }
                System.out.println("]");
            }
        }
    }
}
