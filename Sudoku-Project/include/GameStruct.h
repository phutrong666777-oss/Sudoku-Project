#ifndef GAME_ARCHITECTURE_H
#define GAME_ARCHITECTURE_H

#include <iostream>
#include <SFML/Graphics.hpp> // Dành cho Thành viên 3

// =====================================================================
// KHAI BÁO CHUNG (Dùng chung cho toàn game)
// =====================================================================
enum GameState { MENU, PLAYING, PAUSED, GAME_OVER_WIN, GAME_OVER_LOSE };
enum Difficulty { EASY, MEDIUM, HARD };

// Chuyển tiếp khai báo class (Forward declarations)
class GameEngine;
class SudokuBoard;

// =====================================================================
// THÀNH VIÊN 4: GAMEPLAY (Timer, Hint, Points, Difficulty...)
// =====================================================================
class GameplayManager {
private:
    int timer;
    int mistakeCounter;
    const int MAX_MISTAKES = 3;
    int hintsLeft;
    int currentScore;
    Difficulty currentDifficulty;
    bool isPaused;

    // Cấu trúc cho Leaderboard
    struct Record {
        char playerName[50];
        int time;
        int score;
    };
    Record leaderboard[10]; 
    int recordCount;

public:
    GameplayManager();
    
    void updateTimer(int deltaTime);
    void addMistake();
    void useHint(); // Gọi logic từ Board
    void addPoints(int points);
    
    void togglePause();
    bool checkLoseCondition(); // Trả về true nếu mistakeCounter >= MAX_MISTAKES
    
    // Quản lý Leaderboard
    void saveToLeaderboard(const char* name, int time, int score);
    void loadLeaderboard(); // Đọc từ file
};

// =====================================================================
// THÀNH VIÊN 2: SUDOKU LOGIC (Duy Hiếu)
// =====================================================================
class SudokuBoard {
private:
    int board[9][9];
    int solution[9][9];
    bool initialCells[9][9]; // Đánh dấu các ô gốc không được sửa

public:
    SudokuBoard();

    // Thuật toán Solver (Backtracking)
    bool solve(int grid[9][9]); 

    void generateSolvedBoard();
    void generatePuzzle(Difficulty diff); // Tùy độ khó sẽ gọi removeCells với số lượng khác nhau
    void removeCells(int count);

    bool isValidMove(int row, int col, int value);
    bool setCell(int row, int col, int value);
    bool checkWin(); // Kiểm tra mảng board có khớp hoàn toàn solution không

    int getCell(int row, int col);
    bool isInitialCell(int row, int col);
};

// =====================================================================
// THÀNH VIÊN 3: GRAPHICS / UI (& Đăng Khôi)
// =====================================================================
// Base class cho đa hình (Polymorphism)
class UIElement {
public:
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void handleInput(sf::Event& event, sf::RenderWindow& window) = 0;
    virtual ~UIElement() {}
};

class Button : public UIElement {
private:
    sf::RectangleShape rect;
    sf::Text text;
    bool isHovered;

public:
    Button(float x, float y, float width, float height, const char* label, sf::Font& font);
    void draw(sf::RenderWindow& window) override;
    void handleInput(sf::Event& event, sf::RenderWindow& window) override;
    bool isClicked(sf::RenderWindow& window);
};

class SudokuCell {
private:
    sf::RectangleShape box;
    sf::Text valueText;
    sf::Text pencilMarksText; // Cho tính năng nháp (pencil)
    int row, col;
    bool isSelected;

public:
    SudokuCell();
    void init(int r, int c, float x, float y, float size, sf::Font& font);
    void updateVisuals(int value, bool isInitial);
    void setSelected(bool state);
    
    void draw(sf::RenderWindow& window);
};

class BoardUI : public UIElement {
private:
    SudokuCell cells[9][9];
    SudokuBoard* logicBoard; // Con trỏ liên kết với logic của Duy Hiếu

public:
    BoardUI(SudokuBoard* board, sf::Font& font);
    void draw(sf::RenderWindow& window) override; // Vẽ toàn bộ lưới và gọi cells[i][j].draw()
    void handleInput(sf::Event& event, sf::RenderWindow& window) override; // Xử lý chuột/phím chọn ô
};

// =====================================================================
// THÀNH VIÊN 1: GAME ENGINE & SCENE MANAGEMENT (Phú Trọng)
// =====================================================================
// Abstract class cho các màn hình (Menu, Board, Win/Lose)
class Scene {
protected:
    GameEngine* engine; // Con trỏ về Engine để chuyển state
public:
    Scene(GameEngine* eng) : engine(eng) {}
    virtual void update(int deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void handleInput(sf::Event& event, sf::RenderWindow& window) = 0;
    virtual ~Scene() {}
};

class GameEngine {
private:
    sf::RenderWindow window;
    GameState currentState;
    Scene* currentScene; // Quản lý đa hình Scene
    
    // Tài nguyên dùng chung
    sf::Font mainFont;
    SudokuBoard gameBoard;
    GameplayManager gameplayManager;

public:
    GameEngine();
    ~GameEngine();

    void init();
    void run(); // GAME LOOP chính

    void processEvents();
    void update(int deltaTime);
    void render();

    // Scene & State Management
    void changeState(GameState newState);
    void changeScene(Scene* newScene);

    // Xử lý File IO
    void saveGame(); // Lưu board (gọi logic board) và trạng thái (gọi gameplay stats)
    void loadGame();

    // Getters cho các Scene truy cập
    sf::RenderWindow& getWindow();
    SudokuBoard* getBoard();
    GameplayManager* getGameplayManager();
};



// =====================================================================
// HÀM MAIN (Khởi chạy Game Loop)
// =====================================================================
/*
int main() {
    GameEngine game;
    game.init();
    game.run();
    return 0;
}
*/

#endif
