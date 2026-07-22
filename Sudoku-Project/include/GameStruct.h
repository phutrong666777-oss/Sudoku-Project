#ifndef GAME_ARCHITECTURE_H
#define GAME_ARCHITECTURE_H

#include <vector>
#include <string>
#include <memory>

// Giả định thư viện đồ họa (Vỹ An & Đăng Khôi sẽ include thư viện SFML thực tế ở đây)
// #include <SFML/Graphics.hpp>
// #include <SFML/Window.hpp>

// --- ENUMS ---
// [Phú Trọng & Nhóm UI] Quản lý trạng thái luồng game
enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER_WIN, GAME_OVER_LOSE };

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


// =========================================================================
// PHẦN 2: GRAPHICS, UI & INPUT (SFML)
// Thực hiện chính: VỸ AN & ĐĂNG KHÔI (Thành viên 3 & 4)
// =========================================================================

// [Vỹ An & Đăng Khôi]: Class đại diện cho các nút bấm UI
class Button : public IGameObject {
public:
    // Chứa cấu trúc của SFML::RectangleShape, SFML::Text...
    void update() override;
    void draw() override;
    bool isClicked(); // Xử lý Mouse Input
};

// [Vỹ An & Đăng Khôi]: Lớp quản lý toàn bộ giao diện màn hình hiển thị
class UIManager {
private:
    int timerSeconds;
    int mistakes;
    int hintsRemaining;
    
    // Các UI Elements (Buttons, Text...)
    // sf::Font mainFont;

public:
    UIManager();
    
    // --- [Vỹ An & Đăng Khôi]: RENDER CÁC MÀN HÌNH CHÍNH ---
    void drawMenuScreen();
    void drawPauseScreen();
    void drawWinLoseScreen(bool isWin);
    void drawLeaderboard();

    // --- [VỹÀn & Đăng Khôi]: OVERLAY GAMEPLAY UI ---
    void drawTimer();
    void drawMistakeCounter();
    void drawHintIcon();

    // --- [Vỹ An & Đăng Khôi]: XỬ LÝ SỰ KIỆN SFML ---
    void handleMouseInput();
    void handleKeyboardInput();
};


// =========================================================================
// PHẦN 3: GAME ENGINE & SCENE MANAGEMENT
// Thực hiện chính: PHÚ TRỌNG (Thành viên 1)
// =========================================================================

class GameEngine {
private:
    GameState currentState;
    SudokuBoard board;
    UIManager uiManager; // Chứa hệ thống SFML Window bên trong

public:
    GameEngine() : currentState(GameState::MENU) {}
    
    // [Phú Trọng]: Điều phối vòng lặp chính (The Game Loop)
    // Bao gồm Poll Events -> Update -> Render
    void run(); 

    // [Phú Trọng]: Hàm Update chung (gọi update của Board, Timer, UI...)
    void update();
    
    // [Phú Trọng]: Quản lý luồng trạng thái/Scene (Menu <-> Playing <-> GameOver)
    void changeState(GameState newState);
    void manageScenes(); 

    // [Phú Trọng]: Xử lý Save/Load (Đọc ghi File/JSON) 
    // Giao tiếp với Duy Hiếu thông qua serializeBoardData() của class SudokuBoard
    void saveGame();
    void loadGame();
};

#endif
