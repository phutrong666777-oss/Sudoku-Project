# 🎮 Sudoku-Project

A Sudoku game written in **C++**, using the **SFML** library for graphics and audio. It applies **Object-Oriented Programming (OOP)** principles and a **Backtracking** algorithm to generate and solve puzzles.

> Object-Oriented Programming course project – Class 25C08 – University of Science, VNU-HCM

---

## 🛠 Tech Stack

| Component | Details |
|---|---|
| Language | C++17 |
| Library | [SFML](https://www.sfml-dev.org/) – Graphics, Window, Audio, System modules |
| Data structures | `std::vector`, `std::stack` |
| Design patterns | Singleton (`SoundManager`), State (`GameState`) |
| Storage | `sudoku_save.json` (saved game), `records.txt` (best times) |

---

## 📂 Project Structure

```
Sudoku-Project/
├── assets/        # Resources: images, fonts, sounds
├── include/       # Header files (.h)
├── source/        # Source files (.cpp)
├── records.txt    # Best completion times per difficulty
├── .gitignore
├── LICENSE
└── README.md
```

| Module | Responsibility |
|---|---|
| `GameEngine` | Runs the game loop (Input → Update → Render), manages `GameState`, saves/loads games |
| `SudokuLogic` | `IGameObject`, `SudokuCell`, `SudokuBoard`: game rules, Backtracking generation/solving, Undo/Redo |
| `UIManager` | Draws the SFML interface, buttons, timer, mistakes, hints, and the leaderboard |
| `SoundManager` | Singleton that manages background music and sound effects |

---

## 🧱 Class Diagram (UML)

```mermaid
classDiagram
    class IGameObject {
        <<interface>>
        +update() void
        +draw(window, font, selectedRow, selectedCol) void
    }
    class SudokuCell {
        -value: int
        -isFixed: bool
        -isWrong: bool
        -row: int
        -col: int
        +getValue() int
        +setValue(val) void
        +getIsWrong() bool
        +update() void
        +draw() void
    }
    class SudokuBoard {
        -grid: vector~vector~SudokuCell~~
        -currentDifficulty: Difficulty
        -undoStack: stack~MoveRecord~
        -redoStack: stack~MoveRecord~
        +setCell(r, c, val) bool
        +isValidMove(r, c, val) bool
        +checkWin() bool
        +undo() bool
        +redo() bool
        +update() void
        +draw() void
    }
    class GameEngine {
        -currentState: GameState
        -board: SudokuBoard
        -uiManager: UIManager
        +run() void
        +changeState(newState) void
        +handleInput() void
        +update() void
        +renderScenes() void
    }
    class UIManager {
        -timerSeconds: int
        -mistakes: int
        -hintsRemaining: int
        +update(dt) void
        +handleMouseInput() void
        +handleKeyboardInput(event) void
        +handleNumberInput(event) int
        +addMistake() void
    }
    class SoundManager {
        -instance: SoundManager$
        -bgMusic: Music
        -clickSound: Sound
        -SoundManager()
        +getInstance() SoundManager&$
        +playClickSound() void
    }

    IGameObject <|.. SudokuCell
    IGameObject <|.. SudokuBoard
    SudokuBoard "1" *-- "81" SudokuCell : contains
    GameEngine "1" *-- "1" SudokuBoard : contains
    GameEngine "1" *-- "1" UIManager : contains
```

---

## 🔄 Workflow: Entering a Number

```mermaid
sequenceDiagram
    actor P as Player
    participant G as GameEngine
    participant U as UIManager
    participant B as SudokuBoard
    participant C as SudokuCell

    P->>G: Press a number key (e.g. 5)
    G->>U: handleNumberInput(event)
    alt A cell is selected (row != -1)
        U-->>G: Return the entered number (5)
        G->>B: setCell(row, col, 5)
        B->>C: isEditable()
        C-->>B: true
        Note over B: Save MoveRecord to undoStack
        B->>C: setValue(5)
        Note over B: Compare with solutionGrid
        B->>C: setIsWrong(true/false)
        B-->>G: return isCorrect
        opt Wrong entry
            G->>U: addMistake()
        end
    else No cell selected
        U-->>G: return -1
    end
```

---

## ▶️ Run the Game

**1. Clone the repository**

```bash
git clone https://github.com/phutrong666777-oss/Sudoku-Project.git
cd Sudoku-Project
```

**2. Install SFML**

```bash
# Ubuntu / Debian
sudo apt install libsfml-dev g++

# macOS
brew install sfml
```

On Windows: download SFML from [sfml-dev.org](https://www.sfml-dev.org/download.php), configure the `include`/`lib` paths, and copy the SFML `.dll` files next to the `.exe`.

**3. Build**

```bash
g++ -std=c++17 -I include source/*.cpp -o Sudoku \
    -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system
```

**4. Run**

```bash
./Sudoku        # Linux / macOS
Sudoku.exe      # Windows
```

> ⚠️ Run the game **from the project root directory** so the program can find the `assets/` folder and the `records.txt` and `sudoku_save.json` files.
