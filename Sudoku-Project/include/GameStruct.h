#ifndef GAME_ARCHITECTURE_H
#define GAME_ARCHITECTURE_H

#include <vector>
#include <string>
#include <memory> // [Member 1]: Dùng cho quản lý bộ nhớ

// --- Abstract Base Class ---
class IGameObject {
public:
    virtual void update() = 0;   
    virtual void draw() = 0;     
    virtual ~IGameObject() {}
};

enum GameState { MENU, PLAYING, GAME_OVER };

// --- Lớp ô Sudoku ---
class SudokuCell : public IGameObject {
private:
    int value;
    bool isFixed; 
    int row, col;

public:
    SudokuCell(int r, int c, int val = 0) : row(r), col(c), value(val), isFixed(val != 0) {}
    
    int getValue() const { return value; }
    void setValue(int val) { if (!isFixed) value = val; }
    bool isEditable() const { return !isFixed; }

    // [Member 2 - Core Dev]: Implement logic cập nhật giá trị
    void update() override { /* Xử lý logic tại ô */ }
    
    // [Member 3 - UI Dev]: Implement việc render ô (xác định tọa độ vẽ trên màn hình)
    void draw() override { /* Vẽ ô lên màn hình */ }
};

// --- Logic Bàn cờ ---
class SudokuBoard : public IGameObject {
private:
    std::vector<std::vector<SudokuCell>> grid;
    int score;
    int mistakes;

public:
    SudokuBoard(); 
    void reset();
    
    // [Member 4 - QA/Logic]: Implement logic kiểm tra thắng cuộc (check 3x3, hàng, cột)
    bool checkWin();
    
    // [Member 2 - Core Dev]: Logic update toàn bộ bảng
    void update() override; 
    
    // [Member 3 - UI Dev]: Vẽ lưới bàn cờ và gọi draw() của từng ô con
    void draw() override;   
    
    // [Member 5 - Documentation]: Viết tài liệu giải thích logic tính điểm
    void addScore(int points);
};

// --- Quản lý Game (Main Loop) ---
class GameEngine {
private:
    GameState currentState;
    SudokuBoard board;

public:
    GameEngine() : currentState(MENU) {}
    
    // [Member 1 - Architect]: Điều phối vòng lặp chính (Game loop control)
    void run(); 
    
    // [Member 3 - UI Dev]: Xử lý sự kiện bàn phím/chuột từ người chơi
    void handleInput(); 
    
    // [Member 1 - Architect]: Quản lý luồng trạng thái (Menu -> Playing -> Game Over)
    void changeState(GameState newState);
};

// [Member 5 - Documentation]: Dùng Doxygen format để comment giải thích 
// toàn bộ file này cho báo cáo cuối kỳ.

#endif