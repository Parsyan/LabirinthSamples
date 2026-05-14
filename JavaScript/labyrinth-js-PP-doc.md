# labyrinth.js — բացատրություն

Այս փաստաթուղթը նկարագրում է, թե ինչպես է կառուցված `labyrinth.js`-ն, ինչու հենց այդպես, և ինչպես են աշխատում ալգորիթմները։ Java-ի տարբերակին ծանոթ ենք, ուստի այստեղ կենտրոնանում ենք **JavaScript-ին հատուկ բաների** վրա՝ async/await, readline, օբյեկտները որպես հղում, և Node-ի առանձնահատկությունները։

---

## 1. Ընդհանուր գաղափարը

Խաղի կանոնները նույնն են. 10×20 տախտակ, `@` խաղացող, `&` դրոշակ, 10 ժայռ `#`, `w/a/s/d` ուղղություններ, ճանապարհի երաշխիք։

Հիմնական տարբերությունը JavaScript-ի տարբերակի և մյուսների միջև. **մուտքը (input) ասինխրոն է**։ Node-ում `readline`-ը callback-ներով կամ Promise-ներով է աշխատում, ոչ թե բլոկող (blocking) ձևով՝ ինչպես Java-ի `Scanner`-ն է կամ Python-ի `input()`-ը։ Ուստի մեզ պետք է մի փոքր այլ կերպ կազմակերպել կոդը։

---

## 2. Ինչու ենք state-ը պահում օբյեկտի մեջ

JavaScript-ում **primitive տիպերը** (`number`, `string`, `boolean`) ֆունկցիա փոխանցելիս պատճենվում են, ինչպես Java-ում.

```js
function f(x) {
    x = 10;  // միայն լոկալ x-ն է փոխվում
}
```

Բայց **օբյեկտներն ու array-ները** փոխանցվում են որպես **հղում** (առավել ճիշտ՝ որպես «հղման պատճեն»). ֆունկցիան ստանում է նույն օբյեկտի վրա ցույց տվող նոր անուն, ուստի `obj.field = ...` փոփոխությունները երևում են արտաքինից.

```js
function f(obj) {
    obj.field = 10;  // արտաքին օբյեկտն էլ է տեսնում
}
```

Ուստի state-ը պահում ենք պարզ object-ի մեջ.

```js
const state = {
    playerRow: 0,
    playerCol: 0,
    flagRow: 0,
    flagCol: 0,
    won: false,
};
```

Ինչու object, ոչ թե array.

- **Անունների շնորհիվ կարդալի է**։ `state.playerRow` շատ ավելի թափանցիկ է, քան `state[0]`։
- **JavaScript-ի object-ները շատ թեթև են**։ Class-ի փոխարեն օգտագործելն ընդունված պրակտիկա է փոքր scripts-ում։
- **Դինամիկ ընդարձակում**։ Կարող ենք ցանկացած պահի ավելացնել նոր դաշտ, օրինակ `state.moves = 0`։

**Ուշադրություն**. `const state = {...}` չի նշանակում, որ state-ը անփոփոխ է։ `const`-ը արգելում է **վերագրել** `state`-ը այլ արժեքի (`state = ...` չի աշխատի), բայց `state.field = ...` հանգիստ թույլատրված է։ Սա շատ տարածված շփոթմունք է JavaScript-ում սկսնակների համար։

---

## 3. `createWorldBoard` — տախտակի կառուցումը

```js
function createWorldBoard(height, length, state) {
    const board = [];
    for (let i = 0; i < height; i++) {
        const row = [];
        for (let j = 0; j < length; j++) {
            row.push(' ');
        }
        board.push(row);
    }
    ...
}
```

Քայլերը նույնն են, ինչ Java-ում.

1. Դատարկ տախտակ՝ բոլոր վանդակները `' '`։
2. Խաղացողին `@` պատահական դիրքում։
3. Դրոշակը `&` պատահական դատարկ վանդակում։
4. Կանչել `placeRocks`-ը։

### JavaScript-ին հատուկ բաներ

**Ինչու չենք գրել `new Array(length).fill(' ')`**.

Կարող ենք գրել.

```js
const row = new Array(length).fill(' ');
```

Դա ճիշտ կաշխատի առաջին մակարդակի համար։ Բայց եթե անեիր.

```js
const board = new Array(height).fill(new Array(length).fill(' '));
```

բոլոր տողերը կլինեին **նույն** array-ի հղումներ։ `board[0][0] = '#'` փոխելու դեպքում `board[1][0]`, `board[2][0]`... նույնպես կփոխվեին։ Շատ տարածված ծուղակ։

Ուստի անվտանգ տարբերակն է կամ `for` ցիկլը (ինչպես արել ենք), կամ `Array.from({length: height}, () => new Array(length).fill(' '))`։

**`Math.random()` և `randInt`**.

JavaScript-ում `Math.random()`-ը վերադարձնում է [0, 1) միջակայքի թիվ։ Java-ի `random.nextInt(max)`-ի համարժեքն ենք պատրաստել մեր սեփական.

```js
function randInt(max) {
    return Math.floor(Math.random() * max);
}
```

Կարող էինք գործածել ուղիղ արտահայտությունը ամեն տեղ, բայց helper ֆունկցիան կոդն ավելի թափանցիկ է դարձնում։

---

## 4. `placeRocks` — նույն ալգորիթմը

Տրամաբանությունը նույնն է, ինչ Java/Python-ում.

```
placed = 0
while placed < 10:
    ընտրիր պատահական դատարկ (r, c)
    դիր '#'
    եթե ճանապարհ դեռ կա → placed++
    հակառակ դեպքում → հետ վերցրու
```

JavaScript-ին հատուկ նրբերանգ չկա այստեղ։

---

## 5. `pathExists` — DFS-ը JavaScript-ով

Նույն ալգորիթմը՝ stack-ով, `visited` 2D, 4 ուղղություն։

### Ինչն է հարմարեցել JavaScript-ին

**Stack-ը պարզ array է**.

```js
const stack = [[startRow, startCol]];
...
const [r, c] = stack.pop();
...
stack.push([nr, nc]);
```

JavaScript-ի array-ները ունեն `push` և `pop` մեթոդներ, որոնք O(1)-ով աշխատում են վերջից։ Stack-ը հենց array-ն է, ինչպես Python-ում էր list-ը։

**Destructuring** —

```js
const [r, c] = stack.pop();
```

Java-ում առանձին-առանձին վերցնում էինք երկու զուգահեռ array-ից։ JavaScript-ում մեկ տողով։

**Հարևանների ցուցակը**.

```js
const deltas = [[-1, 0], [1, 0], [0, -1], [0, 1]];
for (const [dr, dc] of deltas) {
    ...
}
```

`for...of`-ը iteration-ի ժամանակակից ձև է։ Միասին destructuring-ով կարդացվում է ինչպես «չորս զույգ (dr, dc), վերցրու դրանք ուղղակիորեն»։

### Ինչու չենք օգտագործել ռեկուրսիա

Նույն պատճառով, ինչ մյուս լեզուներում. ձեռքով stack-ով տարբերակն ավելի թափանցիկ է։ JavaScript-ում նաև ռեկուրսիան կարող է լինել խնդրահարույց, քանի որ V8-ը (Node-ի շարժիչը) **չի կիրառում tail-call optimization**, ուստի խորը ռեկուրսիան կարող է լցնել stack-ը։ Ձեռքով stack-ով տարբերակն այդ խնդիր չունի։

---

## 6. `moveUp/Down/Left/Right` + `tryMove`

Բոլոր չորսը պարզապես կանչում են `tryMove`-ը նոր կոորդինատներով։ Տրամաբանությունը նույնն է. սահման → ժայռ → դրոշակ → սովորական։

```js
state.won = true;
```

Java-ում `state[4] = 1` (int-ով boolean), Python-ում `state["won"] = True`, JavaScript-ում `state.won = true`։ Object-ի դաշտեր մոտեցումն ամենուր նմանատիպ կարդալիություն է տալիս։

---

## 7. `printWorld` — տպելու մասը

```js
function printWorld(board) {
    const length = board[0].length;
    const border = '+' + '-'.repeat(length) + '+';
    console.log(border);
    for (const row of board) {
        console.log('|' + row.join('') + '|');
    }
    console.log(border);
}
```

JavaScript-ի string-ները ունեն `repeat(n)` մեթոդ՝ `'-'.repeat(20) === '--------------------'`։ Python-ի `'-' * 20`-ի համարժեքն է։

`row.join('')` — array-ի բոլոր էլեմենտները միացնում է մեկ string-ի՝ առանց բաժանարարի։ Հատկապես հարմար է երբ տարրերը նիշեր են։

---

## 8. `inputDirection` — ինչու է async

Սա ամենահետաքրքիր մասն է JavaScript-ի տարբերակում։ Node-ի `readline` մոդուլը callback-ներով է աշխատում.

```js
rl.question("Direction: ", (answer) => {
    // answer-ը հասանելի է այստեղ
});
```

Կոդը հոսում է «հակառակ ուղղությամբ»՝ Java-ում/Python-ում.

```
տող ստանալ → ստուգել → օգտագործել
```

Իսկ callback-ով.

```
ստուգման ու օգտագործման լոգիկան փաթաթել callback-ի մեջ → այն ինչ-որ պահի կկանչվի
```

Սա արագ դառնում է «callback hell» երբ ունենք while-ով խնդրելու loop։

### Promise + async/await լուծումը

Մենք callback-ը փաթաթում ենք Promise-ի մեջ.

```js
function ask(rl, question) {
    return new Promise(resolve =>
        rl.question(question, answer => resolve(answer))
    );
}
```

Ինչ նշանակում է սա.

- `new Promise(...)` ստեղծում է «ապագա» արժեք։ Հենց որ `resolve(answer)` կանչվի, այդ Promise-ը կհամարվի կատարված, և `answer`-ը կդառնա դրա արժեքը։
- `rl.question`-ը երբ ավարտվում է, կանչում է իր callback-ը, որի մեջ մենք պարզապես `resolve(answer)`-ենք անում։

Հետո կարող ենք գրել.

```js
const line = (await ask(rl, "Direction (w/a/s/d): ")).trim();
```

`await`-ը «սպասում է», մինչև Promise-ը կատարվի, և վերադարձնում է դրա արժեքը։ Կոդը կարդացվում է հենց ուղիղ վերևից ներքև, ճիշտ ինչպես Java/Python տարբերակներում։

### Ինչու սա կարևոր է

Առանց Promise + async/await կոդը կունենար 3-4 մակարդակ ներդրված callback-ներ։ async/await-ը JavaScript-ի 2017-ից (ES2017) ավելացած գործիք է, որը ասինխրոն կոդը գրեթե նույնքան թափանցիկ է դարձնում, որքան սինխրոն տարբերակը։

Բայց **սա պետք է հաշվի առնել**. եթե ֆունկցիան `async` է, այն վերադարձնում է Promise (ոչ թե անմիջական արժեք)։ Ուստի `inputDirection`-ը կանչողը նույնպես պետք է `await` անի։

```js
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
```

---

## 9. `labyrinth` — async ֆունկցիա

```js
async function labyrinth() {
    const state = { ... };
    const rl = readline.createInterface({ input: process.stdin, output: process.stdout });

    printUserManual();
    const board = createWorldBoard(10, 20, state);
    printWorld(board);

    while (!state.won) {
        const direction = await inputDirection(rl);
        ...
    }

    rl.close();
}
```

Քանի որ `inputDirection`-ը async է, `labyrinth`-ն էլ պետք է լինի async՝ իր մեջ `await` օգտագործելու համար։

**`rl.close()` ֆայլի վերջում կարևոր է**. եթե չփակենք readline interface-ը, Node պրոցեսը չի ավարտվի, քանի դեռ stdin-ը բաց է։

---

## 10. Node-ի մուտքի կետը

```js
labyrinth();
```

JavaScript-ում մուտքի կետի հատուկ համաձայնեցում չկա (ինչպիսին է Java-ի `main` մեթոդը)։ Ֆայլը պարզապես կատարվում է վերևից ներքև։ Ուստի վերջում կանչում ենք `labyrinth()`-ը։

Մի կարևոր մանրուք. `labyrinth()`-ը async ֆունկցիա է, ուստի վերադարձնում է Promise։ Մենք այն չենք `await` անում, պարզապես սկսում ենք գործարկել։ Node-ը կպահի պրոցեսը կենդանի, քանի դեռ Promise-ը չի լուծվում (ու քանի դեռ readline-ը բաց է)։

Եթե ցանկանայինք ավելի «մաքուր» գրել.

```js
labyrinth().catch(err => console.error(err));
```

Բայց փոքրիկ խաղի համար սովորական կանչը բավարար է։

---

## 11. Ինչու հենց այս կառուցվածքը

Նույն սկզբունքները, ինչ նախկինում.

- **Մեկ ֆունկցիա՝ մեկ պատասխանատվություն**։
- **Ճանապարհի երաշխիք**։
- **State-ը մեկ տեղում** (object)։
- **Մուտքը պաշտպանված է**։

JavaScript-ին հատուկ ընտրությունները.

- **Object որպես state**, ոչ թե class։ Դեռ չենք սովորել class-եր, և object-ը բավարար է փոքր խաղի համար։
- **Promise + async/await readline-ի համար**։ Թույլ է տալիս ասինխրոն մուտքը կարդալ սինխրոն-տեսք ունեցող կոդով։
- **Iteration-ի ժամանակակից ձևեր**` `for...of`, destructuring, arrow functions։

---

## 12. Հնարավոր հաջորդ քայլեր

- **TypeScript-ի անցում**. Տիպերը կօգնեն բռնել սխալները կազմման փուլում (օրինակ՝ `state` օբյեկտի դաշտերի անունների սխալ գրությունը)։
- **Class-ի վերածել**. `class Game { ... }` ավելի «ավանդական» օբյեկտակողմնորոշիչ դիզայն կտա։
- **Browser-ում աշխատացնել**. readline-ը browser-ում չկա, փոխարենը պիտի օգտագործել HTML form կամ `prompt()`։
- **Visual interface**. SVG կամ Canvas-ով տախտակը գծել, ստեղնաշարի կոճակները ուղղակի լսել։
- **Քայլերի հաշվիչ, multiple levels**, և այլն, ինչպես Java-ում։
