# labyrinth.cpp — բացատրություն

Այս փաստաթուղթը նկարագրում է, թե ինչպես է կառուցված `labyrinth.cpp`-ն, ինչու հենց այդպես, և ինչպես են աշխատում ալգորիթմները։ Java-ի և C-ի տարբերակները կան, ուստի այստեղ կենտրոնանում ենք **C++-ին հատուկ բաների** վրա՝ `std::vector`, references, `<random>`-ի ժամանակակից generator-ները, `getline`-ը string-ով, `auto`-ն, structured bindings-ը։

C++-ը կարող ենք համարել «C-ից առաջ քայլ»՝ ավելի անվտանգ, ավելի կարդալի, ավելի օբյեկտակողմնորոշիչ։ Բոլոր C-ի յուրահատկությունները հասանելի են այստեղ, բայց ստանդարտ գրադարանը (STL) մեզ տալիս է լուրջ գործիքներ։

---

## 1. Ընդհանուր գաղափարը

Խաղի կանոնները նույնն են. 10×20 տախտակ, `@/&/#`, `w/a/s/d`, ճանապարհի երաշխիք։ Ալգորիթմները նույնն են, ինչ Java-ի կամ C-ի տարբերակում։

Հիմնական տարբերությունները կազմի մակարդակում.

- **`std::vector`** ֆիքսված size-ի array-ի փոխարեն։
- **References (`&`)** pointer-ների փոխարեն։
- **`std::mt19937`** `rand()`-ի փոխարեն։
- **`std::string` + `std::getline`** `char buffer[128]` + `fgets`-ի փոխարեն։
- **Struct-ը կարող է ունենալ default values**, ուստի initialization-ը ավելի կարճ է։

---

## 2. State struct-ով, փոխանցված որպես reference

```cpp
struct State {
    int player_row = 0;
    int player_col = 0;
    int flag_row = 0;
    int flag_col = 0;
    bool won = false;
};
```

### Ինչու struct, ոչ թե class

C++-ում `struct`-ն ու `class`-ը գրեթե միևնույն բանն են. միակ տարբերությունը «default access»-ն է (`struct`-ում default-ով `public`, `class`-ում default-ով `private`)։ Փոքր data holder-ների համար `struct`-ն ընտրությունն է, որովհետև մենք ուզում ենք բոլոր դաշտերը public լինեն։

### In-class member initialization

```cpp
int player_row = 0;
```

C++11-ից սկսած կարող ենք **ուղիղ struct-ի սահմանման մեջ** տալ սկզբնական արժեքներ։ Հետո երբ գրում ենք `State state;`, բոլոր դաշտերը ինքնաբերաբար initialize են լինում։ Չենք ստիպված ձեռքով գրել `State state = {0, 0, 0, 0, false};`։

C-ում սա հնարավոր չէր. ստիպված էինք initialization լիստեր (`State state = {0, 0, 0, 0, 0};`) կամ memset-եր օգտագործել։

### `bool` ընդդեմ `int`-ի «won» դաշտի համար

C-ում օգտագործել էինք `int won` (0 կամ 1), որովհետև «classical» C-ում `bool` չկա (C99-ից կա `_Bool` ու `<stdbool.h>`, բայց պարզ ձև չէ)։ C++-ում `bool`-ը հիմնական տիպ է, ուստի ուղղակի օգտվում ենք.

```cpp
state.won = true;
while (!state.won) { ... }
```

Կարդալի է, ապահով է, սխալվել շատ ավելի դժվար է։

### References (`State&`)

C-ում state-ը փոխանցում էինք pointer-ով. `State *state`։ C++-ում օգտվում ենք **reference**-ից.

```cpp
void try_move(Board& board, int new_row, int new_col, State& state)
```

- Կանչողի կողմից. `try_move(board, r, c, state);` — ոչ `&state` պետք է, ոչ `*state` մարմնի մեջ։
- Մարմնի կողմից. `state.won = true;` — սովորական դոտով, ոչ `state->won`։

**Ինչու references-ը ավելի լավն են, քան pointer-ները.**

- Չեն կարող լինել `nullptr`. C++-ի reference-ն միշտ ցույց է տալիս ինչ-որ իրական օբյեկտի։ Չենք պետք է ստուգել `nullptr`։
- Չեն կարող փոխվել՝ ինչ-որ ուրիշ բանի ցույց տալ։ Մի անգամ bind-ած՝ ընդմիշտ։
- Syntax-ն ավելի մաքուր է. ոչ `*` ու `&` ամեն տեղ։

`const Board& board` — ինչպես `pathExists`-ում, ասում ենք. «board-ը հղումով եմ ուղարկում (արագ), բայց չպիտի փոխվի»։ Կոմպիլյատորն ստուգում է, որ չենք պատահականորեն փոխել։ Սա «const correctness» է, C++-ի կարևոր սովորույթներից մեկը։

---

## 3. `std::vector` ընդդեմ fixed-size array-ի

C-ում օգտագործել էինք `char board[HEIGHT][LENGTH]`։ C++-ում.

```cpp
using Board = std::vector<std::vector<char>>;
```

### Ինչու vector

`std::vector`-ն ինքնակառավարվող dynamic array է։ Մեր օգուտները.

- **Հեշտ չափի փոփոխություն**. եթե հետագայում ուզենք օգտատիրոջը խնդրել տախտակի չափը, պարզապես `Board(h, std::vector<char>(l, ' '))` ենք գրում։ Fixed-size array-ով կանգնած էինք compile-time-ով սահմանվող չափի դեմ։
- **Իմանում է իր չափը**. `board.size()`, `board[0].size()`։ Java-ի `board.length`-ի համարժեքն է։ C-ում ստիպված էինք `HEIGHT`/`LENGTH` macros-ները ձեռքով տանել։
- **Hide-ում է memory management**։ Vector-ը ինքն է հատկացնում ու ազատում հիշողությունը։ Չկա `malloc`/`free` հարցեր, չկա memory leaks։

### `using Board = ...`

```cpp
using Board = std::vector<std::vector<char>>;
```

C++11-ի «type alias»։ Հին C++-ում նմանատիպ էր `typedef std::vector<std::vector<char>> Board;`, բայց `using`-ը մի փոքր ավելի կարդալի է։ Տեսքով նման է. «նոր անունով անվանիր այս տիպը»։

Հիմա կոդի մեջ ամեն տեղ կարող ենք գրել `Board` երկար `std::vector<std::vector<char>>`-ի փոխարեն։

### Vector-ի ստեղծումը

```cpp
Board board(height, std::vector<char>(length, ' '));
```

Կարդացվում է. «Board, որն ունի `height` հատ row, ամեն row-ը `length` երկարությամբ vector՝ լցված `' '`-ով»։

Երկու-մակարդակ vector-ի այս ինիցիալիզացիան էլեգանտ լուծում է. չենք ստիպված `for` ցիկլ գրել։

---

## 4. `std::mt19937` — ինչու ոչ `rand()`

C-ում օգտվել ենք `rand() % HEIGHT`-ից։ C++-ում օգտվում ենք **`<random>` գրադարանից**.

```cpp
std::mt19937& rng() {
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

int rand_int(int max) {
    std::uniform_int_distribution<int> dist(0, max - 1);
    return dist(rng());
}
```

### Ինչու սա ավելի լավն է

- **`mt19937`**-ը (Mersenne Twister) շատ ավելի լավ որակի pseudo-random generator է, քան C-ի `rand()`-ը (որի որակը կարող է ճիշտ տկար լինել)։
- **`uniform_int_distribution`**-ը հաշվարկում է առանց modulo-ի շեղման (bias-ի)։ Մաքուր հավասարաչափ բաշխում։
- **`random_device`**-ը OS-ից վերցնում է «իրական» random սերմ՝ `time(NULL)`-ից շատ ավելի լավ։

### `static` փոփոխականը ֆունկցիայի ներսում

```cpp
static std::mt19937 engine{std::random_device{}()};
```

`static` ֆունկցիայի ներսում նշանակում է. «այս փոփոխականը initialize է լինում **միայն առաջին անգամ** ֆունկցիան կանչելիս, և մնում է հետագա կանչերի համար»։ Ուստի մեկ engine ենք ստեղծում ողջ ծրագրի համար ու վերօգտագործում։

Սա «singleton» նմուշի կարճ ձև է, շատ տարածված C++-ում։

### Brace-initialization

```cpp
std::mt19937 engine{std::random_device{}()};
```

`{}` փոխարեն `()`-ի։ Modern C++-ում brace-initialization-ը նախընտրելի է (uniform initialization, ավելի քիչ նրբերանգներ, օրինակ չի թողնում narrowing conversion)։

---

## 5. `path_exists` — DFS-ը C++-ով

Նույն ալգորիթմը, բայց stack-ը հիմա `std::vector<std::pair<int,int>>` է.

```cpp
std::vector<std::pair<int,int>> stack;
stack.reserve(height * length);
stack.push_back({state.player_row, state.player_col});
```

### `std::pair` — ինչու սա

C-ում ունեինք երկու զուգահեռ array (`stack_r`, `stack_c`)։ C++-ում կա `std::pair<int,int>`՝ երկու արժեքի «թեթև» զույգ։ Մեկ array, մեկ տիպ։

Կարող էինք օգտվել նաև `std::tuple`-ից (ավելի ընդհանուր), կամ սեփական struct-ից (`struct Cell {int r, c;};`)։ Փոքր helper-ի համար `std::pair`-ը բավարար է։

### `reserve` — մանր օպտիմիզացիա

```cpp
stack.reserve(height * length);
```

Մենք գիտենք, որ stack-ում առավելագույնը կունենանք `height * length` էլեմենտ։ `reserve`-ով ասում ենք vector-ին. «անմիջապես հատկացրու այսքան տեղ»։ Առանց դրա vector-ը պետք է մի քանի անգամ կրկնապատկեր ու copy աներ (re-allocate)։

Չպարտադիր է, բայց լավ հաբիթ է երբ գիտես ապահով upper bound։

### Structured bindings

```cpp
auto [r, c] = stack.back();
stack.pop_back();
```

C++17-ից սկսած կարող ենք pair-ից կամ tuple-ից մի քանի արժեք «բացել» մեկ տողով։ Java-ի կամ Python-ի tuple unpacking-ի համարժեքն է։

`auto`-ն թողնում է կոմպիլյատորին որոշել տիպը. այստեղ կորոշի `int`։

### `stack.back() + pop_back()` — ինչու երկու քայլ

C-ում/Python-ում/JavaScript-ում `pop`-ը մեկ քայլով և՛ վերցնում էր, և՛ հանում։ C++-ի `std::vector`-ի `pop_back()`-ը **չի վերադարձնում** էլեմենտը։ Սա մտածված դիզայնի որոշում է. exception safety-ի համար։

Ուստի անում ենք երկու քայլ. `back()` (վերցնել վերջը), `pop_back()` (հանել)։ Մի փոքր ավելի վերակոմպոզվող, բայց ապահով։

---

## 6. `printWorld` — `std::string`-ով

```cpp
std::string border = "+" + std::string(length, '-') + "+";
```

`std::string(length, '-')` ստեղծում է string՝ `length` հատ `-` նիշերից։ Java-ի `"-".repeat(length)`-ի կամ Python-ի `'-' * length`-ի համարժեքն է։

`<<` օպերատորով տպում ենք.

```cpp
std::cout << border << "\n";
```

C++-ում `std::cout`-ը stream է. `<<`-ով «մղում ենք» նրա մեջ տվյալներ։ Կարող ենք chain անել. `std::cout << "a" << b << "c";`։

---

## 7. `inputDirection` — `std::getline`-ով

```cpp
std::string line;
while (true) {
    std::cout << "Direction (w/a/s/d): " << std::flush;
    if (!std::getline(std::cin, line)) {
        std::cout << "\nGoodbye.\n";
        std::exit(0);
    }
    ...
}
```

### `std::getline` ընդդեմ C-ի `fgets`-ի

C-ում օգտվում էինք `fgets(buffer, sizeof(buffer), stdin)`-ից։ C++-ում `std::getline(std::cin, line)`-ն ավելի մաքուր է.

- **Չափի սահման չկա**. `std::string`-ն ինքնակառավարվող է, աճում է ինչքան պետք է։ Buffer overflow վտանգ չկա։
- **`\n`-ը ինքնաբերաբար հանվում է**։ C-ի `fgets`-ը պահում էր նաև newline-ը, ստիպված էինք ձեռքով կտրել։
- **EOF-ի դեպքը**` `getline`-ը վերադարձնում է stream-ի reference, որը `if (!...)`-ի մեջ ստուգվում է որպես falsy։ Մաքուր, պարզ։

### `<< std::flush`

C-ի `fflush(stdout)`-ի համարժեքն է. ստիպում է buffer-ը «լցել» էկրանին։ Տպելու prompt-ի համար, որը `\n`-ով չի ավարտվում, դա պարտադիր է։

Կարող էինք գրել նաև `std::cout << "Direction (w/a/s/d): " << std::endl;`, որը նաև flush է անում։ Բայց `endl`-ը նաև ավելացնում է `\n`։ Մեզ պետք էր flush առանց newline-ի, ուստի օգտվեցինք `std::flush`-ից։

### Տողի մաքրումը

```cpp
size_t i = 0;
while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) i++;

if (i >= line.size()) {
    std::cout << "Empty input is not allowed.\n";
    continue;
}

char ch = (char) std::tolower((unsigned char) line[i]);
```

- `size_t` — unsigned integer type, որը չափերի համար է։ `int` ուղղակի կարող է թվալ ավելի կարճ, բայց signed/unsigned-ի compiler warning-ները կիրառվեն (`i < line.size()` երբ `i` signed է)։
- `std::tolower` + `(unsigned char)` cast — նույն պատճառով, ինչ C-ում. `tolower`-ը undefined է signed `char`-ի համար բացասական արժեքների դեպքում։

---

## 8. `labyrinth` — հոսքի ուղեղը

```cpp
void labyrinth() {
    State state;                                  // ինքնաբերաբար 0-ով initialize է

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
```

Կարդացվում է գրեթե նույնը, ինչ Java-ի կամ Python-ի տարբերակը։ Մի քանի C++-ին հատուկ բան.

- **`State state;`** — և ամեն ինչ արդեն ճիշտ initialize է (in-class member initialization-ի շնորհիվ)։ C-ում ստիպված էինք գրել `State state = {0, 0, 0, 0, 0};`։
- **`Board board = create_world_board(...);`** — ֆունկցիան վերադարձնում է `Board` object, որը հիմա պատկանում է `board` փոփոխականին։ Vector-ները ինքնակառավարվող են, ուստի memory-ի մասին մտածելու հարկ չկա։

---

## 9. RVO/Move semantics — vector վերադարձնելու մասին

Հարց է առաջանում. «Բայց vector-ը մեծ է, արդյոք `create_world_board`-ից վերադարձնելիս չի՞ կրկնապատկվում»։ Ոչ։

C++ ունի **Return Value Optimization (RVO)** ու **move semantics**, ինչը նշանակում է, որ ֆունկցիայից vector վերադարձնելը գործնականում **զրո պատճենում** է։ Կոմպիլյատորը կամ ուղղակիորեն ստեղծում է vector-ն այնտեղ, որտեղ ստացողը այն կպահի (RVO), կամ «մետաֆիզիկապես տեղափոխում» է բովանդակությունը (move)։

Ուստի մենք կարող ենք գրել պարզ, թափանցիկ կոդ՝ առանց անհանգստանալու կատարողականության մասին.

```cpp
Board board = create_world_board(HEIGHT, LENGTH, state);
```

Հին C++-ում սա կարող էր լինել մտահոգություն. C++11-ից այլևս չէ։ Սա հատկապես կարևոր տարբերակում է C-ից, որտեղ նմանատիպ բան անելու համար ստիպված կլինեիր ձեռքով `malloc` ու pointer փոխանցել։

---

## 10. Անվանատարածք (namespace) և `std::`

Հնարավոր է նկատել, որ ամեն տեղ գրում ենք `std::vector`, `std::string`, `std::cout`, ոչ թե պարզ `vector`, `string`, `cout`։

C++-ում ստանդարտ գրադարանի ամեն ինչը գտնվում է `std` namespace-ում։ Մենք **չենք գրել `using namespace std;`** ֆայլի սկզբում, ինչը հաճախ տեսնվում է tutorial-ներում։

Ինչու.

- **Անվանումների բախում**։ Եթե write ենք `using namespace std;` ու հետո ստեղծում ենք մեր սեփական `count` ֆունկցիան, դա կարող է հակասել `std::count`-ի հետ։
- **Կարդալիություն**։ Երբ տեսնում ես `std::vector`, անմիջապես հասկանում ես, որ սա ստանդարտ գրադարանից է, ոչ թե մեր սեփական։

Մեծ կոդբեյզներում `using namespace std;`-ը համարվում է վատ ոճ։ Փոքր script-ների համար ընդունելի է, բայց հաբիթ ձևավորելու համար ավելի լավ է հենց սկզբից սովորել `std::`-ի հետ։

---

## 11. Ինչու հենց այս կառուցվածքը

Նույն սկզբունքները, ինչ նախորդ լեզուներում.

- **Մեկ ֆունկցիա՝ մեկ պատասխանատվություն**։
- **Ճանապարհի երաշխիք**։
- **State-ը մեկ struct-ում**։
- **Մուտքը պաշտպանված է**, ներառյալ EOF-ի դեպքը։

C++-ին հատուկ ընտրությունները.

- **`std::vector`** fixed-size array-ի կամ raw pointers-ի փոխարեն — ինքնակառավարվող, ապահով։
- **References** pointer-ների փոխարեն, որտեղ հարմար է — մաքուր syntax։
- **`std::mt19937` + `<random>`** — որակյալ ու հավասարաչափ պատահականություն։
- **`std::string` + `std::getline`** — buffer overflow-ից ազատ մուտքի կարդալը։
- **`auto` + structured bindings** — ժամանակակից C++-ի ընթերցելիության գործիքներ։
- **Հստակ `std::` namespace** — սխալների ու բախումների կանխարգելման համար։

---

## 12. C++ ընդդեմ C-ի — հիմնական դասը

Եթե կարդացել ես C-ի փաստաթուղթը (`labyrinth-c-doc.md`), կարող ես հիմա համեմատել.

| Հարց | C | C++ |
|------|---|-----|
| State փոխանցում | `State *state`, `state->won` | `State& state`, `state.won` |
| 2D array | `char board[H][L]` (fixed) | `std::vector<std::vector<char>>` |
| Stack | երկու զուգահեռ array | `std::vector<std::pair<int,int>>` |
| Մուտք | `fgets(buffer, sz, stdin)` | `std::getline(std::cin, line)` |
| String | `char buffer[128]` | `std::string` |
| Random | `rand() % max` | `std::uniform_int_distribution` |
| Boolean | `int` (0/1) | `bool` |
| Initialization | ձեռքով կամ `= {0}` | դաշտերում default values |

C++-ը պահպանում է C-ի ողջ ուժը (կարող ենք pointer-ներ, raw arrays, malloc օգտագործել երբ պետք է), բայց ավելացնում է մի շարք ապահով, կարդալի աբստրակցիաներ։ Modern C++-ով գրած կոդը հաճախ նման է Java/C# կոդի, քան classical C-ի։

---

## 13. Հնարավոր հաջորդ քայլեր

- **OOP**` `State`-ը փոխարկել `class Game`-ի՝ private դաշտերով ու public մեթոդներով։ `board`-ն ու `state`-ը կդառնան class-ի անդամներ, ֆունկցիաները՝ մեթոդներ։
- **Smart pointers**. Եթե dynamic allocation պետք լինի, օգտվել `std::unique_ptr` և `std::shared_ptr`-ից, ոչ թե raw `new`/`delete`-ից։
- **Templates**. `path_exists`-ը կարող ենք դարձնել template՝ ցանկացած նիշերի տիպի համար։
- **Exception handling**. Մուտքի սխալների կամ ֆայլերի IO-ի համար `try`/`catch`։
- **Class hierarchy**. `Cell`, `Player`, `Flag`, `Rock` դասերի համակարգ՝ polymorphism-ով։
