#pragma once
#ifndef GAME_ARCHITECTURE_H
#define GAME_ARCHITECTURE_H

#include <vector>
#include <string>
#include <memory>
#include <stack>

// Giả định thư viện đồ họa (Vỹ An & Đăng Khôi sẽ include thư viện SFML thực tế ở đây)
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <bits/stdc++.h>
using namespace std;

// [Duy Hiếu & Nhóm UI] Quản lý độ khó
enum class Difficulty { EASY, MEDIUM, HARD, EXPERT };


// --- Abstract Base Class ---
class IGameObject {
public:
    virtual void update() = 0;   
    virtual void draw(sf::RenderWindow& window, const sf::Font& font, int selectedRow, int selectedCol) = 0;     
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
    bool isWrong; // <-- THÊM BIẾN NÀY
    int row, col;

public:
    // Cập nhật constructor để khởi tạo isWrong = false
    SudokuCell(int r, int c, int val = 0) : value(val), isFixed(val != 0), isWrong(false), row(r), col(c) {}
    
    int getValue() const { return value; }
    void setValue(int val) { if (!isFixed) value = val; }
    void setFixed(bool fixed) { this->isFixed = fixed; }
    bool isEditable() const { return !isFixed; }

    // <-- THÊM 2 HÀM SETTER/GETTER NÀY
    bool getIsWrong() const { return isWrong; }
    void setIsWrong(bool wrong) { isWrong = wrong; }

    void update() override; 
    void draw(sf::RenderWindow& window, const sf::Font& font, int selectedRow, int selectedCol) override; 
};

struct MoveRecord {
    int row;
    int col;
    int oldValue;
    int newValue;
};

class SudokuBoard : public IGameObject {
private:
    std::vector<std::vector<SudokuCell>> grid;
    Difficulty currentDifficulty;
    int solutionGrid[9][9]; // Bảng giải hoàn chỉnh để hint

    //2 STACK ĐỂ QUẢN LÝ UNDO/REDO
    std::stack<MoveRecord> undoStack;
    std::stack<MoveRecord> redoStack;

public:
    SudokuBoard(); 
    
    // --- [Duy Hiếu]: CÁC HÀM CORE SUDOKU LOGIC ---
    bool isValidMove(int r, int c, int val); 
    bool setCell(int r, int c, int val);     
    bool checkWin();   
    bool isGiveHint(int& hintRow, int& hintCol); // Đưa ra gợi ý nếu hợp lệ 
    
    // THÊM 2 HÀM MỚI NÀY VÀO PHẦN CORE LOGIC
    bool undo();
    bool redo();
    void clearHistory(); // Dùng khi tạo ván mới hoặc load game
    
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
    void draw(sf::RenderWindow& window, const sf::Font& font, int selectedRow, int selectedCol); // [Vỹ An & Đăng Khôi]: Vẽ lưới Sudoku

    int (&getSolutionGrid())[9][9] { return solutionGrid; } // Lấy bảng giải để GameEngine gọi hint

    Difficulty getCurrentDifficulty() { return currentDifficulty; }
};

#endif
