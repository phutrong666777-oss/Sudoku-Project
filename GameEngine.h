
#include <string>
#include "SudokuLogic.h"
#include "UIManager.h" // Hoặc "UIManager.h" tùy theo tên file header của bạn
#include "SoundManager.h" // Thêm include cho SoundManager
// Định nghĩa các trạng thái của toàn bộ ứng dụng Game
enum class GameState {
    MENU,
    SETTINGS,
    RECORD,
    SELECT_DIFFICULTY,
    PLAYING,
    PAUSED,
    GAME_OVER_WIN,
    GAME_OVER_LOSE
};

class GameEngine {
private:
    GameState currentState;     // Trạng thái hiện tại của game
    SudokuBoard board;          // Lớp quản lý logic bảng Sudoku
    UIManager uiManager;        // Lớp quản lý giao diện, nút bấm, thời gian, số lỗi

public:
    // Constructor khởi tạo (mặc định vào MENU hoặc PLAYING)
    GameEngine() : currentState(GameState::MENU) {}

    // Vòng lặp chính điều phối toàn bộ game
    void run();

    // Các hàm quản lý trạng thái
    void changeState(GameState newState);
    GameState getCurrentState() const { return currentState; }

    // Các tính năng phụ trợ lưu/tải game
    void saveGame();
    void loadGame();
    void newGame(Difficulty diff = Difficulty::MEDIUM);
private:
    // 3 pha chuẩn trong Game Loop
    void handleInput(bool& isRunning);
    void update();
    void renderScenes();
};