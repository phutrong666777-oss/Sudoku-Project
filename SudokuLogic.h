#pragma once

#include <bits/stdc++.h>
using namespace std;

// [Duy Hiếu & Nhóm UI] Quản lý độ khó
enum class Difficulty { EASY, MEDIUM, HARD, EXPERT };


// --- Abstract Base Class ---
class IGameObject {
public:
    virtual void update() = 0;   
    virtual void draw() = 0;     
    virtual ~IGameObject() {}
};


// =========================================================================
// PHẦN 1: CORE LOGIC & SUDOKU MECHANICS
// Thực hiện chính: DUY HIẾU (Thành viên 2)
// =========================================================================

class SudokuCell : public IGameObject {
private:
    int value;
    bool isFixed; 
    int row, col;

public:
    SudokuCell(int r, int c, int val = 0) : row(r), col(c), value(val), isFixed(val != 0) {}
    
    // [Duy Hiếu]: Logic getters/setters
    int getValue() const { return value; }
    void setValue(int val) { if (!isFixed) value = val; }
    bool isEditable() const { return !isFixed; }

    // [Duy Hiếu]: Implement logic cập nhật trạng thái ô
    void update() override; 
    
    // [Vỹ An & Đăng Khôi]: Implement việc render ô bằng SFML (Font, Màu số cố định/điền thêm)
    void draw() override; 
};

class SudokuBoard : public IGameObject {
private:
    std::vector<std::vector<SudokuCell>> grid;
    Difficulty currentDifficulty;

public:
    SudokuBoard(); 
    
    // --- [Duy Hiếu]: CÁC HÀM CORE SUDOKU LOGIC ---
    bool isValidMove(int r, int c, int val); 
    bool setCell(int r, int c, int val);     
    bool checkWin();                         
    
    // --- [Duy Hiếu]: THUẬT TOÁN TẠO MAP (GENERATOR & SOLVER) ---
    bool solve();                            // Solver dùng thuật toán Backtracking
    void generateSolvedBoard();              // Khởi tạo bảng đã giải hoàn chỉnh
    void generatePuzzle(Difficulty diff);    // Tạo bảng hoàn chỉnh, sau đó gọi removeCells
    void removeCells(int numCellsToRemove);  // Xóa các ô tùy theo Difficulty
    
    // --- [Duy Hiếu]: HỖ TRỢ LƯU/TẢI GAME ---
    std::string serializeBoardData();        // Xuất data bảng để Trọng gọi Save
    void loadBoardData(const std::string& data); // Đọc data bảng để Trọng gọi Load

    // Override IGameObject
    void update() override; 
    void draw() override; // [Vỹ An & Đăng Khôi]: Vẽ lưới Sudoku
};
