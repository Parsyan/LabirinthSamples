# LabyrinthByOOP.java — բացատրություն

Այս փաստաթուղթը նկարագրում է, թե ինչպես է կառուցված `LabyrinthByOOP.java`-ն, ինչու հենց այդպես, և ինչպես են աշխատում ալգորիթմները։ 
Ենթադրվում է, որ դու արդեն կարդացել ես `LabyrinthByPP.java`-ի (պրոցեդուրային տարբերակի) բացատրությունը, ուստի խաղի **տրամաբանությունն** արդեն ծանոթ է։ 
Այստեղ մենք կենտրոնանում ենք միայն **OOP-ի** (Object-Oriented Programming) բերած փոփոխությունների վրա։

---

## 1. Ընդհանուր գաղափարը

Խաղի կանոնները չեն փոխվել՝
- 10×20 տախտակ,
- `@` խաղացողը պետք է հասնի `&` դրոշակին,
- `#` ժայռերը խանգարում են,
- կառավարումը՝ `w/a/s/d`։

Փոխվել է միայն **կոդի կազմակերպումը**։ Պրոցեդուրային տարբերակում մենք ունեինք.

- մեկ մեծ `class Labyrinth`,
- մի շարք `static` մեթոդներ,
- `int[] state` զանգված, որով «ձեռքով» էինք պահում խաղացողի դիրքը, դրոշակի դիրքը և հաղթանակի դրոշը,
- `char[][] board` երկչափ զանգված՝ `' '`, `'@'`, `'#'`, `'&'` նիշերով։

OOP-տարբերակում նույն խաղն ապրում է մի քանի **դաս**ի մեջ, և տախտակը ոչ թե նիշերի, այլ **օբյեկտների** ցանց է.

```
Cell (abstract)
 ├── EmptyCell   (' ')
 ├── Rock        ('#')
 ├── Flag        ('&')
 └── Player      ('@')

Board   — պահում և կառավարում է Cell[][] ցանցը
Game    — վարում է խաղի loop-ը, ձեռնարկը, մուտքը
LabyrinthByOOP — մուտքի կետ, ստեղծում է Game ու կանչում
```

Ընդհանուր հոսքը նույնն է, ինչ PP-ում.

```
ձեռնարկ ցույց տալ  →  Board կառուցել  →  տպել  →
   ↓
   while չհաղթել:
       ուղղություն խնդրել  →  շարժվել  →  տպել  →  ստուգել՝ հաղթե՞լ ենք
```

Բայց յուրաքանչյուր քայլ արդեն ոչ թե «ֆունկցիա է, որ ստանում է զանգված ու state», այլ «մեթոդ է որևէ օբյեկտի վրա», որը գիտի իր սեփական տվյալների մասին։

---

## 2. OOP-ի չորս սկզբունքները այս կոդում

Երբ խոսում ենք OOP-ի մասին, սովորաբար նշում են չորս հիմնական գաղափար. **Abstraction**, **Encapsulation**, **Inheritance**, **Polymorphism**։ 
Փորձենք տեսնել, թե յուրաքանչյուրը որտեղ է հայտնվում մեր կոդում, որովհետև սրանք ոչ թե վերացական տերմիններ են, այլ կոնկրետ որոշումներ, որ կայացրել ենք։

### 2.1 Abstraction (վերացարկում)

**Գաղափարը.** թաքցնել ավելորդ մանրուքները և ցույց տալ միայն այն, ինչ կարևոր է։

Մեր կոդում `Cell` դասը հենց **abstraction** է.

```java
abstract class Cell {
    public abstract char getSymbol();
    public boolean isBlocking() { return false; }
}
```

Մենք ասում ենք. «Տախտակի վրա կա ինչ-որ բան, որը նստած է որևէ տողի և սյան վրա, ունի որևէ խորհրդանիշ, և կարող է փակել կամ չփակել ճանապարհը»։ Մենք **դեռ չենք ասում**, թե դա ժայռ է, թե դրոշակ, թե խաղացող։ `Board`-ին դա չի էլ պետք իմանալ՝ նրա համար բավական է, որ կարող է կանչել `cell.getSymbol()` ու `cell.isBlocking()`։

### 2.2 Encapsulation (պատամփակում)

**Գաղափարը.** օբյեկտի ներքին տվյալները թաքցնել, իսկ դրսից թույլատրել միայն ստուգված գործողություններ։

`Cell`-ի դաշտերը՝ `row` ու `col`, **private** են.

```java
private int row;
private int col;

public int getRow() { return row; }
public int getCol() { return col; }
public void setPosition(int row, int col) { ... }
```

Դրսից չի կարելի ուղղակիորեն գրել `cell.row = -5;`։ Դա կարևոր է, որովհետև եթե բոլորին թույլ տանք ուղղակի փոխել դիրքը, ինչ-որ տեղ ինչ-որ մեկը կարող է ժայռի դիրքը դարձնել «-5», ու տախտակը կկոտրվի։

Նույն ձևով `Board`-ի մեջ.

```java
private final Cell[][] grid;
private Player player;
private Flag flag;
private boolean won;
```

Արտաքին աշխարհը (օրինակ՝ `Game`-ը) `grid`-ին **երբեք ուղղակի չի դիպչում**։ Նա խնդրում է. «`board.moveUp()` արա», «`board.printWorld()` արա», «`board.isWon()` ?»: Ինչպես է դա ներքին ձևով արվում, `Board`-ի սեփական գործն է։

Համեմատիր PP-տարբերակի հետ. այնտեղ `board[][]` և `state[]` զանգվածները շրջում էին որպես պարամետրեր, և ցանկացած ֆունկցիա կարող էր դրանք ինչ ուզում, փոխեր։ Encapsulation-ը մի շերտ պաշտպանություն է ավելացնում։

### 2.3 Inheritance (ժառանգում)

**Գաղափարը.** մի դասը/class-ը կարող է վերցնել մյուսի ամբողջ ֆունկցիոնալը և ավելացնել կամ վերագրել մի մաս։

```java
class EmptyCell extends Cell { ... }
class Rock      extends Cell { ... }
class Flag      extends Cell { ... }
class Player    extends Cell { ... }
```

Չորս ենթադասերը/child class-ները **ժառանգում են** `Cell`-ից `row`, `col`, `getRow()`, `getCol()`, `setPosition()`, և `isBlocking()`-ի լռելյայն տարբերակը։ 
Իրենք միայն ավելացնում են այն, ինչ տարբեր է՝ սովորաբար միայն `getSymbol()`-ը, իսկ `Rock`-ը՝ նաև `isBlocking()`-ի վերագրումը.

```java
class Rock extends Cell {
    @Override public char getSymbol()  { return '#'; }
    @Override public boolean isBlocking() { return true; }
}
```

Փոխանակ չորս առանձին դասի մեջ կրկնելու դիրքի կոդը, գրում ենք մեկ անգամ `Cell`-ի մեջ։

### 2.4 Polymorphism (բազմաձևություն)

**Գաղափարը.** նույն մեթոդի կանչը կարող է վարվել տարբեր ձևերով՝ կախված օբյեկտի իրական տեսակից։

Տես `Board.printWorld()`-ը.

```java
for (int j = 0; j < length; j++) {
    System.out.print(grid[i][j].getSymbol());
}
```

`grid[i][j]`-ն այստեղ պարզապես `Cell` է։ Կոդը **չի հարցնում** «ի՞նչ ես՝ ժայռ, դրո՞շ, թե խաղացող»։ Նա պարզապես ասում է` «արտածիր քո խորհրդանիշը»։ Java-ն ինքն է որոշում, թե որ ենթադասի `getSymbol()`-ը կանչել՝ կախված այդ պահի օբյեկտից։

Նույն կերպ `Board.pathExists()`-ում.

```java
if (grid[nr][nc].isBlocking()) continue;
```

Չենք գրում «եթե `Rock` է, շրջանցիր»։ Հարցնում ենք բուն օբյեկտին։ Եթե վաղը ավելացնենք նոր տեսակի խոչընդոտ (օրինակ՝ «ջուր»), որը նույնպես `isBlocking() == true` է, **այս ոչ մի տող փոխել պետք չի լինի**։

Սա հենց OOP-ի ուժն է. ֆունկցիոնալությունն ընդարձակելու ժամանակ քո հին կոդը մնում է անձեռնմխելի։

---

## 3. `Cell`-ի հիերարխիան մանրամասն

Դիտարկենք դասերը մեկ առ մեկ։

### 3.1 `Cell` — աբստրակտ հիմքը

```java
abstract class Cell {
    private int row;
    private int col;

    public Cell(int row, int col) { this.row = row; this.col = col; }
    public int getRow() { return row; }
    public int getCol() { return col; }
    public void setPosition(int row, int col) { ... }

    public abstract char getSymbol();
    public boolean isBlocking() { return false; }
}
```

Մի քանի կարևոր կետ.

- **`abstract class`** — սա նշանակում է, որ չենք կարող ուղղակիորեն գրել `new Cell(...)`։ `Cell`-ը ինքնին «ինչ-որ բան» չէ տախտակի վրա, այլ պարզապես **ընդհանուր կաղապար**։
- **`abstract char getSymbol()`** — մարմին չունեցող մեթոդ։ Ամեն ենթադաս **պարտավոր է** իր մարմինը տալ։ Կոմպիլյատորը հենց դա էլ կստուգի։
- **`isBlocking()`-ը լռելյայն վերադարձնում է `false`** — այսինքն լռելյայնը «չի փակում»։ Ենթադասերից միայն `Rock`-ը պետք է վերագրի սա` `true`-ի։ `EmptyCell`, `Flag`, `Player`-ի մասին այս հարցում մտածել պետք չէ՝ ժառանգում են լռելյայնը։

### 3.2 `EmptyCell`, `Rock`, `Flag`, `Player`

Բոլոր չորսը գրեթե նույն կառուցվածքն ունեն, տարբերվում են միայն խորհրդանիշով.

```java
class EmptyCell extends Cell {
    public EmptyCell(int row, int col) { super(row, col); }
    @Override public char getSymbol() { return ' '; }
}
```

- **`super(row, col)`** — կանչում ենք ծնող `Cell`-ի կոնստրուկտորին, որ նա պահի `row` ու `col` դաշտերը։ Մենք **չենք կրկնում** այդ տողերը մեր մեջ։
- **`@Override`** — Java-ին ասում ենք. «Սա ծնողի մեթոդի վերագրումն է»։ Եթե ինչ-որ պատճառով սխալ ենք գրել անունը (օրինակ՝ `getSybmol`), կոմպիլյատորը մեզ սխալ կտա։

`Rock`-ն ունի մի լրացուցիչ վերագրում.

```java
@Override public boolean isBlocking() { return true; }
```

Իսկ `Player`-ը հատկապես ոչնչով յուրահատուկ չէ, պարզապես ուրիշ խորհրդանիշով։ Որպեսզի **հետագայում** ավելացնենք, օրինակ, քայլերի հաշվիչ կամ «կյանք» (HP), կարող ենք դա անել `Player`-ի մեջ՝ առանց մյուս դասերին դիպչելու։

### 3.3 Ինչու սա ավելի լավ է, քան `char`-երը

PP-տարբերակում տախտակը պարզ `char[][]` էր, և ամեն տեղ կոդը «բացում» էր այդ նիշերը.

```java
if (board[r][c] == '#') ...
if (board[r][c] == '&') ...
if (board[r][c] == ' ') ...
```

Խնդիրը. այս ստուգումները **ցրված են** ամբողջ կոդով։ Եթե վաղը որոշես, որ ժայռը դառնա `'*'`, պետք է գտնես **բոլոր** `'#'`-ները ու փոխես։ Իսկ եթե ինչ-որ տեղ մոռանաս, bug է լինելու։

OOP-տարբերակում խորհրդանիշը **մեկ տեղում է**՝ համապատասխան դասի `getSymbol()`-ում։ Մնացած կոդը չի մտածում նիշերի մասին։ Փոխելու համար մի տող է պետք։

---

## 4. `Board` դասը

`Board`-ը խաղի «աշխարհն» է. նա պահում է ցանցը, գիտի ինչպես կառուցել այն, տպել, և շարժել խաղացողին։

### 4.1 Դաշտերը

```java
private final Cell[][] grid;
private final int height;
private final int length;
private Player player;
private Flag flag;
private boolean won;

private static final int ROCK_COUNT = 10;
```

- **`grid`** — տախտակը՝ որպես `Cell` օբյեկտների երկչափ զանգված։ `final` է, որովհետև հենց որ կառուցվել է, այլևս հղումը չի փոխվում (բովանդակությունը, իհարկե, փոխվում է)։
- **`player` և `flag`** — առանձին հղումներ, որպեսզի արագ հասնենք նրանց առանց ամբողջ ցանցը շրջելու։ Սա PP-ի `state[0..3]`-ի փոխարինումն է, բայց **իմաստավորված անուններով**։
- **`won`** — փոխարինում է PP-ի `state[4]`-ին։ Հիմա ուղղակի `boolean` է, ոչ թե 0/1։
- **`ROCK_COUNT`** — հաստատուն, `static final`, որպեսզի «10»-ը մեկ տեղ լինի (եթե վաղը դարձնենք 15, մի տեղում ենք փոխում)։

### 4.2 Կոնստրուկտորը

```java
public Board(short height, short length, Random random) {
    this.height = height;
    this.length = length;
    this.grid = new Cell[height][length];
    this.won = false;
    createWorldBoard(random);
}
```

Կարևորը. `Random`-ը **դրսից** է փոխանցվում, ոչ թե ստեղծվում է ներսում։ Դա կոչվում է **dependency injection** (կախվածության ներարկում)։ Օգուտը՝ թեստավորման ժամանակ կարող ենք փոխանցել «կեղծ» Random (նույն seed-ով), որպեսզի տախտակը կանխատեսելի լինի։

### 4.3 `createWorldBoard` — տախտակի կառուցում

Քայլերը նույնն են, ինչ PP-ում.

1. Լցնել ամեն վանդակ `EmptyCell`-ով.

   ```java
   for (int i = 0; i < height; i++)
       for (int j = 0; j < length; j++)
           grid[i][j] = new EmptyCell(i, j);
   ```

2. Պատահական վանդակում տեղադրել `Player`-ին։
3. Մեկ ուրիշ դատարկ վանդակում տեղադրել `Flag`-ին։
4. Կանչել `placeRocks`-ը։

Ուշադրություն դարձրու.

```java
if (grid[r][c] instanceof EmptyCell) { ... }
```

`instanceof`-ով հարցնում ենք. «Այս վանդակը *դատարկ ցելլ*-ի օրինակ է՞»: Քանի որ `Player`-ն արդեն տեղադրված է, սա մեզ պաշտպանում է, որ `Flag`-ը նրա վրա չգրենք։

### 4.4 `placeRocks` — «փորձիր ու հետ կանչիր»

Ալգորիթմն այստեղ նույնն է, ինչ PP-ում, այսինքն հենց trial-and-rollback-ը.

```java
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
        grid[r][c] = backup;     // հետ բերում ենք
    }
}
```

Մի փոքր նուրբ մանրուք. **ինչու ենք `backup`-ը պահում, եթե էդ վանդակն ուղղակի `EmptyCell` էր**։ Սխալ չէր լինի գրել՝

```java
grid[r][c] = new EmptyCell(r, c);
```

Բայց ընդհանուր `backup`-ով նախշն ավելի հստակ արտահայտում է մտադրությունը` «վերականգնում ենք այն, ինչ կար մինչ ճանապարհը կտրելու փորձը»։ Հետագա կոդի փոփոխությունների դեպքում (եթե ինչ-որ պատճառով այնտեղ դատարկ վանդակից տարբեր բան լինի), այս նախշը չի կոտրվի։

`attempts < 1000`-ի դերը. եթե տախտակն այնքան փոքր է, որ 10 ժայռ ընդհանրապես չի տեղավորվում առանց ճանապարհը կտրելու, infinite loop-ի փոխարեն պարզապես կանգնում ենք։

### 4.5 `pathExists` — DFS

Սա հենց այն ալգորիթմն է, որ PP-տարբերակում մանրամասն բացատրեցինք։ OOP-տարբերակում **ալգորիթմը նույնն է**, փոխվել է միայն ստուգման ձևը.

PP-տարբերակում էր.

```java
if (board[nr][nc] == '#') continue;  // ժայռ է, մի անցիր
```

OOP-տարբերակում.

```java
if (grid[nr][nc].isBlocking()) continue;
```

Այստեղ չենք հարցնում «ի՞նչ տիպի վանդակ է», այլ՝ «արդյո՞ք փակում է ճանապարհը»։ Այսպիսով `pathExists`-ը ոչինչ չգիտի `Rock` դասի մասին։ Եթե վաղը ավելացնենք «բուշ», «ջուր» կամ «պատ» տեսակ, որը նույնպես `isBlocking() == true` է, **`pathExists`-ին դիպչել պետք չէ**։

Մնացած մասը նույնն է.

```java
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
```

Հիշիր.

- **`visited`-ի դերը** — առանց դրա DFS-ը անվերջ պտտվում է հարևան երկու վանդակի միջև։
- **`dr`/`dc` զույգերը** — չորս ուղղությունները (վերև, ներքև, ձախ, աջ) կոմպակտ կերպով արտահայտում են։
- **Stack-ը` ձեռքով** — Java-ի `Stack` դասի փոխարեն օգտագործում ենք երկու `int[]` և `top` փոփոխական։ Սա ավելի թափանցիկ է սովորելու համար ու ավելի արագ։

---

## 5. Շարժումը՝ `moveUp/Down/Left/Right` + `tryMove`

Չորս ուղղության մեթոդները հենց պարզ delegation-ի նմուշ են.

```java
public void moveUp()    { tryMove(player.getRow() - 1, player.getCol()); }
public void moveDown()  { tryMove(player.getRow() + 1, player.getCol()); }
public void moveLeft()  { tryMove(player.getRow(),     player.getCol() - 1); }
public void moveRight() { tryMove(player.getRow(),     player.getCol() + 1); }
```

Իրականում ամբողջ տրամաբանությունը մեկ տեղ է` `tryMove`-ի մեջ.

```java
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
        grid[player.getRow()][player.getCol()] =
            new EmptyCell(player.getRow(), player.getCol());
        player.setPosition(newRow, newCol);
        grid[newRow][newCol] = player;
        won = true;
        return;
    }

    // target is an EmptyCell: walk in.
    grid[player.getRow()][player.getCol()] =
        new EmptyCell(player.getRow(), player.getCol());
    player.setPosition(newRow, newCol);
    grid[newRow][newCol] = player;
}
```

Ստուգումների **հերթականությունը** կարևոր է.

1. **Դուրս ենք տախտակի՞ց** — եթե այո, դուրս գանք առանց ոչինչ փոխելու։ Եթե սա բաց թողնենք, հաջորդ տողի `grid[newRow][newCol]`-ը կփլուզի ծրագիրը։
2. **Ճանապարհը փակվա՞ծ է** — `target.isBlocking()`-ով, **առանց հարցնելու որ դա հենց ժայռ է**։
3. **Դրոշակն ենք գտել** — `instanceof Flag`։ Շարժվում ենք, դնում `won = true`, ու վերջացնում խաղը։
4. **Մնացյալ դեպք** — տարածքը դատարկ է, ուղղակի շարժվում ենք։

### Շարժման երեք քայլը

Երկու բարդ ճյուղերում (Flag և EmptyCell) շարժումն ունի նույն երեք գործողությունը.

1. **Հին վանդակը դարձնել `EmptyCell`** — որպեսզի այն տեղում, որտեղ կանգնած էինք, հիմա «դատարկություն» լինի։
2. **`player.setPosition(newRow, newCol)`** — Player օբյեկտի ներքին դիրքը թարմացնել։
3. **`grid[newRow][newCol] = player`** — հենց Player **հղումը** դնել նոր տեղում։

Կարևոր է հասկանալ, որ `Player` օբյեկտը մեկն է. մենք չենք ստեղծում նոր `Player` ամեն շարժման ժամանակ։ Պարզապես **շարժում ենք միևնույն հղումը** ցանցի մեկ վանդակից մյուսը։

### Ինչու չի աշխատի «նոր Player ստեղծել» տարբերակը

Մտածես՝ ավելի կարճ կլիներ.

```java
grid[newRow][newCol] = new Player(newRow, newCol);
```

Բայց հետո `Board`-ի `player` դաշտը (որով գիտենք, որտեղ է խաղացողը) ուշանում է հին օբյեկտի վրա։ Հաջորդ DFS-ը կամ տպումը կօգտագործեր սխալ տեղեկություն։ Միակ ճիշտ ձևն է **թարմացնել նույն օբյեկտի** ներքին դիրքը։

### Out-of-bounds vs blocking — ինչու երկու տարբեր հաղորդագրություն

Խաղացողը պետք է հասկանա, թե **ինչու** իր ուզած շարժումը չաշխատեց՝

- «Out of bounds. You stay.» → հասել եմ պատին։
- «A rock blocks your way.» → առջևումս ժայռ կա։

PP-տարբերակում նույն հաղորդագրությունները կային, OOP-ն այստեղ բացառություն չէ. UX-ը կարևոր է անկախ ոճից։

---

## 6. `printWorld` — տպելու մասը (polymorphism գործողության մեջ)

```java
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
```

Տեսքով ճիշտ նույնն է, ինչ PP-տարբերակում, **բացի մեկ տողից**.

PP-տարբերակում էր.

```java
System.out.print(board[i][j]);   // char-ի տպում
```

OOP-տարբերակում.

```java
System.out.print(grid[i][j].getSymbol());   // հարցնում ենք օբյեկտին
```

Հենց այստեղ է հայտնվում **polymorphism**-ը. Java-ն ինքն է որոշում, թե որ ենթադասի `getSymbol()`-ը կանչել։ Եթե վանդակում `Player` օբյեկտ է, կկանչվի `Player.getSymbol()` ու կվերադարձնի `'@'`։ Եթե `Rock` է` `'#'`։ Կոդը նույնն է, արդյունքը` տարբեր։

Շրջանակը (`+---+` և `|`) մնում է որպես պարզ տեսողական օգնություն, որպեսզի խաղացողը հստակ տեսնի, թե որտեղ է տախտակի սահմանը։ Հատկապես կարևոր է բացատների (դատարկ վանդակների) դեպքում, որպեսզի դրանք չշփոթվեն էկրանի դատարկության հետ։

---

## 7. `Game` դասը և `main`

### 7.1 Ինչու `Game` և `Board` առանձին են

Կարելի էր ամեն ինչ խցկել `Board`-ի մեջ։ Բայց երկու դասերն ունեն **տարբեր պատասխանատվություններ**.

- **`Board`-ը գիտի** տախտակի մասին` ինչպես է կառուցված, ինչ կա ներսում, ինչպես շարժել խաղացողին։ Նա **չի խոսում** օգտատիրոջ հետ։
- **`Game`-ը գիտի**, թե ինչպես է խաղը խաղացվում. ձեռնարկ տպել, մուտք խնդրել, շարժվել, ստուգել հաղթանակը։ Նա **չգիտի** ցանցի կառուցվածքի մասին։

Սա կոչվում է **Single Responsibility Principle**` մեկ դաս՝ մեկ պատասխանատվություն։ Հետագայում եթե որոշես, որ խաղը պետք է աշխատի GUI-ով (գրաֆիկական տեսքով), `Game`-ը պետք է փոխվի, բայց `Board`-ը` ոչ։

### 7.2 Դաշտերը

```java
private final Board board;
private final Scanner scanner;
```

Երկուսն էլ `final` են, որովհետև մեկ անգամ ստեղծվում են և այլևս չեն փոխվում։ PP-տարբերակում `Scanner`-ը, `Random`-ը և `state`-ը լոկալ էին `labyrinth()`-ի մեջ, ու շարժվում էին որպես պարամետրեր։ Հիմա սրանք `Game`-ի սեփական դաշտերն են, և օբյեկտն ինքն իր մեջ պահում է իր state-ը։

### 7.3 `printUserManual`

```java
public void printUserManual() {
    System.out.println("=== LABYRINTH ===");
    System.out.println("Symbols:  @ = you   # = rock   & = flag (goal)");
    System.out.println("Controls: w = up, a = left, s = down, d = right");
    System.out.println("Reach the flag to win. Rocks block you.");
    System.out.println();
}
```

Տեքստ է, ոչ ավելին։ Բայց պետք է լինի, որպեսզի առաջին անգամ խաղացողը հասկանա, թե ինչ պետք է անի։

### 7.4 `inputDirection`

```java
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
```

Տրամաբանությունը նույնն է, ինչ PP-ում.

- դատարկ տող → մերժում ենք,
- `toLowerCase` → որ `W`-ն էլ ընդունվի,
- `charAt(0)` → միայն առաջին նիշը,
- վավեր նիշերի `if` → վերադարձ կամ բողոք։

Տարբերությունն այն է, որ սա այստեղ `static` չէ։ Մեթոդն օգտագործում է `this.scanner`-ը (դաս-ի դաշտը), ուստի կանչողը կարող է կարճ գրել `inputDirection()` առանց պարամետրեր փոխանցելու։

### 7.5 `labyrinth` — խաղի հիմնական ցիկլը

```java
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
```

Համեմատիր PP-տարբերակի հետ.

| PP                              | OOP                       |
|----------------------------------|---------------------------|
| `while (state[4] == 0)`         | `while (!board.isWon())`  |
| `moveUp(board, state)`           | `board.moveUp()`          |
| `printWorld(board)`             | `board.printWorld()`      |
| `state[4] == 1`                 | `board.isWon()`           |

Տրամաբանությունը նույնն է, արտահայտությունն այլ։ OOP-տարբերակը կարդալիս չպետք է իմանաս, թե ինչ `state[4]` է, պարզապես ասում ենք. «board-ը հաղթե՞լ է»։

### 7.6 `LabyrinthByOOP` և `main`

```java
public class LabyrinthByOOP {
    public static void main(String[] args) {
        new Game((short) 10, (short) 20).labyrinth();
    }
}
```

`main`-ը հատուկ կարճ է` ընդամենը մեկ տող։ Սա մտադրված է. **`main`-ը մուտքն է, ոչ թե խաղի տեղը**։ Նա պարզապես.

1. ստեղծում է `Game` օբյեկտ 10×20 չափով,
2. կանչում է `labyrinth()`-ը։

Այս ձևն ընդունված է գրեթե բոլոր Java ծրագրերում. `main`-ի մեջ քիչ կոդ, որպեսզի թեստավորելը հեշտ լինի և ծրագիրը հեշտ ընդարձակվի։

---

## 8. Ինչու հենց այս կառուցվածքը

Մի քանի սկզբունք, որ արժի ընդգծել. սրանք OOP-ի «արժեքներն» են, որոնք երևում են այս փոքր ծրագրում նույնքան, ինչքան մեծերում։

### 8.1 «Մեկ դաս` մեկ պատասխանատվություն»

- **`Cell` ենթադասերը** գիտեն միայն, թե ինչպիսին են տեսքով և արդյոք փակում են ճանապարհը։
- **`Board`-ը** գիտի ցանցի մասին` կառուցում, տպում, շարժում, ճանապարհի ստուգում։
- **`Game`-ը** գիտի օգտատիրոջ հետ հաղորդակցության մասին` ձեռնարկ, մուտք, ցիկլ, հաղթանակի հաղորդագրություն։
- **`LabyrinthByOOP`-ի `main`-ը** գիտի միայն, թե ինչպես սկսել խաղը։

Եթե հաջորդը հանդիպես bug-ի, մեկ այլ անգամ կհամոզվես, որ սա արժե. երբ պատասխանատվությունը մեկ տեղում է, գիտես **որտեղ** նայել։

### 8.2 PP-ից OOP-ի անցումը` ի՞նչը փոխվեց

| PP                                | OOP                                  |
|------------------------------------|---------------------------------------|
| `char[][] board`                  | `Cell[][] grid`                       |
| `int[] state` (5 արժեք)           | `player`, `flag`, `won` դաշտեր        |
| `static` մեթոդներ                  | դաս-ի մեթոդներ (օբյեկտի վրա)         |
| `if (board[r][c] == '#')`         | `if (grid[r][c].isBlocking())`        |
| `System.out.print(board[i][j])`   | `System.out.print(grid[i][j].getSymbol())` |
| «State-ը պարամետրով շրջում է»    | «State-ը օբյեկտի ներսում է»           |

Կարևորը` **ալգորիթմները չեն փոխվել**։ DFS-ը նույն DFS-ն է, trial-and-rollback-ը՝ նույն trial-and-rollback-ը։ Փոխվել է միայն այն, թե **ով է** այս ալգորիթմների տերը։

### 8.3 Ինվարիանտները պահպանվում են

- **Ճանապարհի երաշխիք** — ամեն ժայռի դնելուց հետո ստուգում ենք` ճանապարհը կա։ Եթե կտրվում է, հետ ենք բերում։ Սա կարևոր է, որպեսզի խաղն ընդհանրապես խաղալի լինի։
- **Encapsulation-ի երաշխիք** — `Board.player`, `Board.flag`, `Board.grid` դաշտերը private են, ուստի դրսից ոչ ոք դրանք չի կարող կոտրել։
- **Մուտքի վավերականացում** — `Game.inputDirection()`-ից դուրս երբեք չի կարող սխալ ուղղություն դուրս գա։

### 8.4 Հնարավոր հաջորդ քայլեր

Եթե ուզում ես այս կոդի վրա շարունակել ինքնուրույն աշխատանք.

- **Քայլերի հաշվիչ** — `Player`-ի մեջ ավելացնել `int steps` դաշտ ու `incrementSteps()` մեթոդ։ Հաղթելիս տպել, թե քանի քայլով հասար։
- **«Quit» հրաման** — `Game.inputDirection()`-ին ավելացնել `'q'` որպես վավեր մուտք, ու `labyrinth()` ցիկլում ստուգել` հատկապես հանգիստ դուրս գալու համար։
- **Տախտակի չափը խնդրել օգտատիրոջից** — `Game`-ի կոնստրուկտորից առաջ կամ `main`-ում` `Scanner`-ով։
- **Նոր տեսակի վանդակ** — օրինակ` `Mud` (տիղմ)։ Տիղմը չի փակում, բայց երկու քայլ ծախսում է վրայով անցնելու համար։ Կարիք կլինի փոքր փոփոխություն անել միայն `Cell`-ի կամ `tryMove`-ի մեջ։
- **Մի քանի դրոշակ** — `Flag`-երի ցուցակ. հաղթում ես, երբ բոլորին հավաքեցիր։
- **GUI** — փոխարինել `Game.printWorld`-ը գրաֆիկական տարբերակով։ Կարևորը` `Board`-ի և `Cell`-երի կոդը գրեթե չի փոխվի, պարզապես `getSymbol()`-ի փոխարեն կարող ենք ավելացնել `getColor()` կամ `getIcon()`։

Վերջին կետը հատկապես կարևոր է, որպեսզի զգաս OOP-ի ուժը. **բիզնես-տրամաբանությունը** (խաղի կանոնները) անկախ է **ներկայացման ձևից** (տեքստ թե գրաֆիկա)։ Հենց դա է պատճառը, որ մարդիկ OOP-ով են գրում մեծ ծրագրեր։
