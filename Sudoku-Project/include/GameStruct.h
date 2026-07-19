#ifndef GAME_ARCHITECTURE_H
#define GAME_ARCHITECTURE_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
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
    GameplayManager() {
        timer = 0;
        mistakeCounter = 0;
        hintsLeft = 3;
        currentScore = 0;
        currentDifficulty = EASY;
        isPaused = false;
        recordCount = 0;

        for (int i = 0; i < 10; ++i) {
            leaderboard[i].time = 0;
            leaderboard[i].score = 0;
            leaderboard[i].playerName[0] = '\0';
        }   
    }
    
    void updateTimer(int deltaTime);
    void addMistake();
    void useHint(); // Gọi logic từ Board
    void addPoints(int points);
    
    void togglePause();
    bool checkLoseCondition() {
        return mistakeCounter > MAX_MISTAKES;
    } // Trả về true nếu mistakeCounter >= MAX_MISTAKES

    //Hàm phụ để save leaderboard
    bool compareRecords(const GameplayManager::Record& a, const GameplayManager::Record& b) {
        return a.score == b.score ? a.time < b.time : a.score > b.score ;
    }
    
    // Quản lý Leaderboard
    void saveToLeaderboard(const char* name, int time, int score) {
        if (recordCount < 10) {
            strcpy(leaderboard[recordCount].playerName, name);
            leaderboard[recordCount].time = time;
            leaderboard[recordCount].score = score;
            recordCount++;
        }
        else {
            strcpy(leaderboard[9].playerName, name);
            leaderboard[9].time = time;
            leaderboard[9].score = score;
        }
        std::sort(leaderboard, leaderboard + recordCount, compareRecords);
        
        std::ofstream file ("leaderboard.txt");
        if (file.is_open()) {
            for (int i = 0; i < recordCount; ++i) {
                file << leaderboard[i].playerName << " "
                     << leaderboard[i].time << " "
                     << leaderboard[i].score << "\n";
            }
            file.close();
        }
        else std::cout << "Khong the ghi file\n";
    }
    void loadLeaderboard() {
        std::ifstream file("leaderboard.txt");
        recordCount = 0;

        if (file.is_open()) {
            char name[50];
            int time, score;

            while (file >> name >> time >> score && recordCount < 10) {
                strcpy(leaderboard[recordCount].playerName , name);
                leaderboard[recordCount].time = time;
                leaderboard[recordCount].score = score;
                recordCount++;
            }
        }
        else {
            std::cout << "chua co file\n";
        }
    } // Đọc từ file
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
    void init(int r, int c, float x, float y, float size, sf::Font& font) {
        row = r;
        col = c;

        box.setPosition(sf::Vector2f(x, y));
        box.setSize(sf::Vector2f(size, size));
        box.setFillColor(sf::Color::White);
        box.setOutlineColor(sf::Color(213, 155, 194));
        box.setOutlineThickness(1.0f);

        valueText.setFont(font);
        valueText.setCharacterSize(static_cast<int>(size * 0.6f));
        valueText.setString("");
    }
    void updateVisuals(int value, bool isInitial) {
        if (value == 0) valueText.setString("");
        else {
            valueText.setString(std::to_string(value));
            valueText.setFillColor(isInitial ? sf::Color:: Black : sf::Color::Blue);
            sf::FloatRect textRect = valueText.getLocalBounds();
            valueText.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f,
                                            textRect.position.y + textRect.size.y / 2.0f));
            valueText.setPosition(sf::Vector2f(box.getPosition().x + box.getSize().x /2.0f,
                                  box.getPosition().y + box.getSize().y / 2.0f)); 
        }
    }
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
    void render() {
        window.clear(sf::Color(213, 155, 194));//tui lựa màu này làm nền cho dịu mắt
        if (currentScene != NULL) currentScene->draw(window);

        window.display();
    }

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
