#include "GameStruct.h"
#include <fstream>
#include <iostream>
#include <sstream> // Cần cho việc đọc toàn bộ file dễ dàng
#include <chrono> // Dùng thư viện chuẩn của C++, khỏi lo lỗi cài đặt thư viện

enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER_WIN, GAME_OVER_LOSE };
class GameEngine {
private:

    void renderScenes(); // Sử dụng SFML(từ UIManager) để vẽ các Scene (Menu, Playing, Pause, GameOver)
    void handleInput(); // Xử lý sự kiện SFML(từ UIManager) (Mouse, Keyboard, Window Close)

    GameState currentState;
    SudokuBoard board;
    UIManager uiManager; // Chứa hệ thống SFML Window bên trong

public:
    GameEngine() : currentState(GameState::MENU) {}
    
    // [Phú Trọng]: Điều phối vòng lặp chính (The Game Loop)
    // Bao gồm Poll Events -> Update -> Render
    void run(); 

    // [Phú Trọng]: Hàm Update chung (gọi update của Board, Timer, UI...)
    void update(float dt); // dt = delta time (thời gian trôi qua giữa các frame, mặc định 60FPS)
    
    //Đổi tên từ manageScenes() thành RenderScenes() để rõ ràng hơn
   
    void manageScenes();
    // [Phú Trọng]: Quản lý luồng trạng thái/Scene (Menu <-> Playing <-> GameOver)
    void changeState(GameState newState);
    

    // [Phú Trọng]: Xử lý Save/Load (Đọc ghi File/JSON) 
    // Giao tiếp với Duy Hiếu thông qua serializeBoardData() của class SudokuBoard
    void saveGame();
    void loadGame();
};