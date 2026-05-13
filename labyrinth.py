import random


# state is a dict so any function can change it and the change is seen outside.
# Keys:
#   "player_row", "player_col" - where the player @ stands
#   "flag_row",   "flag_col"   - where the flag & stands
#   "won"                      - False until the player reaches the flag


def create_world_board(height, length, state):
    board = [[' ' for _ in range(length)] for _ in range(height)]

    player_row = random.randint(0, height - 1)
    player_col = random.randint(0, length - 1)
    board[player_row][player_col] = '@'
    state["player_row"] = player_row
    state["player_col"] = player_col

    while True:
        r = random.randint(0, height - 1)
        c = random.randint(0, length - 1)
        if board[r][c] == ' ':
            board[r][c] = '&'
            state["flag_row"] = r
            state["flag_col"] = c
            break

    place_rocks(board, height, length, state)
    return board


def place_rocks(board, height, length, state):
    placed = 0
    attempts = 0
    while placed < 10 and attempts < 1000:
        attempts += 1
        r = random.randint(0, height - 1)
        c = random.randint(0, length - 1)
        if board[r][c] != ' ':
            continue

        board[r][c] = '#'
        if path_exists(board, height, length, state):
            placed += 1
        else:
            board[r][c] = ' '


def path_exists(board, height, length, state):
    start_row = state["player_row"]
    start_col = state["player_col"]
    flag_row = state["flag_row"]
    flag_col = state["flag_col"]

    visited = [[False for _ in range(length)] for _ in range(height)]
    stack = [(start_row, start_col)]
    visited[start_row][start_col] = True

    while stack:
        r, c = stack.pop()
        if r == flag_row and c == flag_col:
            return True

        for dr, dc in ((-1, 0), (1, 0), (0, -1), (0, 1)):
            nr = r + dr
            nc = c + dc
            if nr < 0 or nr >= height or nc < 0 or nc >= length:
                continue
            if visited[nr][nc]:
                continue
            if board[nr][nc] == '#':
                continue
            visited[nr][nc] = True
            stack.append((nr, nc))
    return False


def move_up(board, state):
    try_move(board, state["player_row"] - 1, state["player_col"], state)


def move_down(board, state):
    try_move(board, state["player_row"] + 1, state["player_col"], state)


def move_left(board, state):
    try_move(board, state["player_row"], state["player_col"] - 1, state)


def move_right(board, state):
    try_move(board, state["player_row"], state["player_col"] + 1, state)


def try_move(board, new_row, new_col, state):
    height = len(board)
    length = len(board[0])
    player_row = state["player_row"]
    player_col = state["player_col"]

    if new_row < 0 or new_row >= height or new_col < 0 or new_col >= length:
        print("Out of bounds. You stay.")
        return

    target = board[new_row][new_col]
    if target == '#':
        print("A rock blocks your way.")
        return
    if target == '&':
        board[player_row][player_col] = ' '
        board[new_row][new_col] = '@'
        state["player_row"] = new_row
        state["player_col"] = new_col
        state["won"] = True
        return

    board[player_row][player_col] = ' '
    board[new_row][new_col] = '@'
    state["player_row"] = new_row
    state["player_col"] = new_col


def print_world(board):
    length = len(board[0])
    print('+' + '-' * length + '+')
    for row in board:
        print('|' + ''.join(row) + '|')
    print('+' + '-' * length + '+')


def print_user_manual():
    print("=== LABYRINTH ===")
    print("Symbols:  @ = you   # = rock   & = flag (goal)")
    print("Controls: w = up, a = left, s = down, d = right")
    print("Reach the flag to win. Rocks block you.")
    print()


def input_direction():
    while True:
        line = input("Direction (w/a/s/d): ").strip()
        if line == "":
            print("Empty input is not allowed.")
            continue
        ch = line[0].lower()
        if ch in ('w', 'a', 's', 'd'):
            return ch
        print("Use only w, a, s or d.")


def labyrinth():
    state = {
        "player_row": 0,
        "player_col": 0,
        "flag_row": 0,
        "flag_col": 0,
        "won": False,
    }

    print_user_manual()
    board = create_world_board(10, 20, state)
    print_world(board)

    while not state["won"]:
        direction = input_direction()
        if direction == 'w':
            move_up(board, state)
        elif direction == 's':
            move_down(board, state)
        elif direction == 'a':
            move_left(board, state)
        elif direction == 'd':
            move_right(board, state)

        print_world(board)

        if state["won"]:
            print("You reached the flag. You win!")


if __name__ == "__main__":
    labyrinth()
