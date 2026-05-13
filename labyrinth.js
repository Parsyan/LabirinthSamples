// Run with: node labyrinth.js
// Uses Node's built-in readline for keyboard input.

const readline = require('readline');

// state is a plain object so any function can change it and the change
// is seen outside. Fields:
//   playerRow, playerCol - where the player @ stands
//   flagRow,   flagCol   - where the flag & stands
//   won                  - false until the player reaches the flag

function randInt(max) {
    return Math.floor(Math.random() * max);
}

function createWorldBoard(height, length, state) {
    const board = [];
    for (let i = 0; i < height; i++) {
        const row = [];
        for (let j = 0; j < length; j++) {
            row.push(' ');
        }
        board.push(row);
    }

    const playerRow = randInt(height);
    const playerCol = randInt(length);
    board[playerRow][playerCol] = '@';
    state.playerRow = playerRow;
    state.playerCol = playerCol;

    while (true) {
        const r = randInt(height);
        const c = randInt(length);
        if (board[r][c] === ' ') {
            board[r][c] = '&';
            state.flagRow = r;
            state.flagCol = c;
            break;
        }
    }

    placeRocks(board, height, length, state);
    return board;
}

function placeRocks(board, height, length, state) {
    let placed = 0;
    let attempts = 0;
    while (placed < 10 && attempts < 1000) {
        attempts++;
        const r = randInt(height);
        const c = randInt(length);
        if (board[r][c] !== ' ') continue;

        board[r][c] = '#';
        if (pathExists(board, height, length, state)) {
            placed++;
        } else {
            board[r][c] = ' ';
        }
    }
}

function pathExists(board, height, length, state) {
    const startRow = state.playerRow;
    const startCol = state.playerCol;
    const flagRow = state.flagRow;
    const flagCol = state.flagCol;

    const visited = [];
    for (let i = 0; i < height; i++) {
        const row = [];
        for (let j = 0; j < length; j++) row.push(false);
        visited.push(row);
    }

    const stack = [[startRow, startCol]];
    visited[startRow][startCol] = true;

    while (stack.length > 0) {
        const [r, c] = stack.pop();
        if (r === flagRow && c === flagCol) return true;

        const deltas = [[-1, 0], [1, 0], [0, -1], [0, 1]];
        for (const [dr, dc] of deltas) {
            const nr = r + dr;
            const nc = c + dc;
            if (nr < 0 || nr >= height || nc < 0 || nc >= length) continue;
            if (visited[nr][nc]) continue;
            if (board[nr][nc] === '#') continue;
            visited[nr][nc] = true;
            stack.push([nr, nc]);
        }
    }
    return false;
}

function moveUp(board, state) {
    tryMove(board, state.playerRow - 1, state.playerCol, state);
}

function moveDown(board, state) {
    tryMove(board, state.playerRow + 1, state.playerCol, state);
}

function moveLeft(board, state) {
    tryMove(board, state.playerRow, state.playerCol - 1, state);
}

function moveRight(board, state) {
    tryMove(board, state.playerRow, state.playerCol + 1, state);
}

function tryMove(board, newRow, newCol, state) {
    const height = board.length;
    const length = board[0].length;
    const playerRow = state.playerRow;
    const playerCol = state.playerCol;

    if (newRow < 0 || newRow >= height || newCol < 0 || newCol >= length) {
        console.log("Out of bounds. You stay.");
        return;
    }

    const target = board[newRow][newCol];
    if (target === '#') {
        console.log("A rock blocks your way.");
        return;
    }
    if (target === '&') {
        board[playerRow][playerCol] = ' ';
        board[newRow][newCol] = '@';
        state.playerRow = newRow;
        state.playerCol = newCol;
        state.won = true;
        return;
    }

    board[playerRow][playerCol] = ' ';
    board[newRow][newCol] = '@';
    state.playerRow = newRow;
    state.playerCol = newCol;
}

function printWorld(board) {
    const length = board[0].length;
    const border = '+' + '-'.repeat(length) + '+';
    console.log(border);
    for (const row of board) {
        console.log('|' + row.join('') + '|');
    }
    console.log(border);
}

function printUserManual() {
    console.log("=== LABYRINTH ===");
    console.log("Symbols:  @ = you   # = rock   & = flag (goal)");
    console.log("Controls: w = up, a = left, s = down, d = right");
    console.log("Reach the flag to win. Rocks block you.");
    console.log();
}

// readline is asynchronous in Node, so we wrap question() in a Promise
// and use async/await. That way the rest of the code reads top-to-bottom
// like the Java/Python versions.
function ask(rl, question) {
    return new Promise(resolve => rl.question(question, answer => resolve(answer)));
}

async function inputDirection(rl) {
    while (true) {
        const line = (await ask(rl, "Direction (w/a/s/d): ")).trim();
        if (line === "") {
            console.log("Empty input is not allowed.");
            continue;
        }
        const ch = line[0].toLowerCase();
        if (ch === 'w' || ch === 'a' || ch === 's' || ch === 'd') {
            return ch;
        }
        console.log("Use only w, a, s or d.");
    }
}

async function labyrinth() {
    const state = {
        playerRow: 0,
        playerCol: 0,
        flagRow: 0,
        flagCol: 0,
        won: false,
    };

    const rl = readline.createInterface({ input: process.stdin, output: process.stdout });

    printUserManual();
    const board = createWorldBoard(10, 20, state);
    printWorld(board);

    while (!state.won) {
        const direction = await inputDirection(rl);
        if (direction === 'w') moveUp(board, state);
        else if (direction === 's') moveDown(board, state);
        else if (direction === 'a') moveLeft(board, state);
        else if (direction === 'd') moveRight(board, state);

        printWorld(board);

        if (state.won) {
            console.log("You reached the flag. You win!");
        }
    }

    rl.close();
}

labyrinth();
