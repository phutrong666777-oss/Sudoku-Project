#include "GameEngine.h"

// Hàm phụ trợ đề xuất thêm (hoặc bạn có thể để ngay trong run)
// bool isRunning = true; 

void GameEngine::run() {
    bool isRunning = true; 
    
    // Khởi tạo mốc thời gian bắt đầu
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (isRunning) {
        // 1. Tính toán Delta Time (dt) bằng C++ chuẩn
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsedTime = currentTime - lastTime;
        lastTime = currentTime;
        
        float dt = elapsedTime.count(); // Lấy ra số giây (ví dụ: 0.016)

        // 2. Chạy Game Loop như bình thường
        handleInput(); 
        update(dt); 
        renderScenes(); 
    }
}

void GameEngine::handleInput() {
    // Xử lý Input tách biệt hoàn toàn khỏi việc vẽ
    switch (currentState) {
        case GameState::PLAYING:
            uiManager.handleMouseInput();
            uiManager.handleKeyboardInput();
            break;
        case GameState::MENU:
        case GameState::PAUSED:
        case GameState::GAME_OVER_WIN:
        case GameState::GAME_OVER_LOSE:
            // Các màn hình này chủ yếu dùng chuột để click nút UI
            uiManager.handleMouseInput(); 
            break;
    }
}

void GameEngine::update(float dt) {
    // [Phú Trọng]: Update logic phụ thuộc vào State, giờ đã có dt để UI chạy mượt
    switch (currentState) {
        case GameState::MENU:
            uiManager.update(dt); // Truyền dt cho UI Menu (để làm hiệu ứng fade/chớp)
            break;

        case GameState::PLAYING:
            board.update();       // Board không cần thời gian thực nên giữ nguyên
            
            uiManager.update(dt); // TRUYỀN dt CHO UI (Để UIManager cộng dồn giây vào Timer)

            // Engine đóng vai trò "Trọng tài" kiểm tra luật
            if (board.checkWin()) {
                changeState(GameState::GAME_OVER_WIN);
            }
            break;

        case GameState::PAUSED:
        case GameState::GAME_OVER_WIN:
        case GameState::GAME_OVER_LOSE:
            // Đóng băng board, chỉ update UI của các menu này
            uiManager.update(dt); 
            break;
    }
}

void GameEngine::renderScenes() { 
    // [Phú Trọng]: Hàm này KHÔNG làm thay đổi logic game, CHỈ VẼ.
    switch (currentState) {
        case GameState::MENU:
            uiManager.drawMenuScreen();
            break;

        case GameState::PLAYING:
            board.draw(); // vẽ board trước (Lớp dưới)
            uiManager.drawTimer(); // rồi vẽ UI (Lớp trên)
            uiManager.drawMistakeCounter();
            uiManager.drawHintIcon();
            break;

        case GameState::PAUSED:
            board.draw();                // Vẽ board làm NỀN
            uiManager.drawPauseScreen(); // Vẽ menu pause làm overlay đè lên GIỮA màn hình
            break;

        case GameState::GAME_OVER_WIN:
            board.draw();
            uiManager.drawWinLoseScreen(true);
            break;

        case GameState::GAME_OVER_LOSE:
            board.draw();
            uiManager.drawWinLoseScreen(false);
            break;
    }
}

void GameEngine::changeState(GameState newState) {
    // Tương lai dễ dàng thêm các logic chuyển cảnh ở đây
    // Ví dụ: Nếu newState == PLAYING, thì restartTimer();
    currentState = newState;
}

void GameEngine::saveGame() {
    // Tên file lưu game 
    const std::string saveFileName = "sudoku_save.json";

    std::cout << " Dang tien hanh luu game..." << std::endl;

    // 1. Lấy dữ liệu đã mã hóa từ Duy Hiếu (Board)
    std::string boardData = board.serializeBoardData();

    // 2. Mở file để ghi (ios::out)
    std::ofstream outFile(saveFileName);

    // 3. AN TOÀN: Kiểm tra xem file có mở thành công không
    if (!outFile.is_open()) {
        std::cerr << "LỖI: Khong the tao hoac mo file " << saveFileName << " de luu!" << std::endl;
        return; // Thoát ngang, không làm crash game
    }

    // 4. Ghi dữ liệu vào file
    outFile << boardData;
    
    // 5. Đóng file và báo thành công
    outFile.close();
    std::cout << " Luu game thanh cong vao file " << saveFileName << "!" << std::endl;
}

void GameEngine::loadGame() {
    const std::string saveFileName = "sudoku_save.json";

    std::cout << "Dang tai game tu file..." << std::endl;

    // 1. Mở file để đọc (ios::in)
    std::ifstream inFile(saveFileName);

    // Kiểm tra xem file có mở thành công không
    if (!inFile.is_open()) {
        std::cerr << "LỖI: Khong tim thay file " << saveFileName << "! (Co the nguoi choi chua luu game)" << std::endl;
        return; 
    }

    // 3. Đọc TOÀN BỘ nội dung file vào chuỗi (string)
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string loadedData = buffer.str();

    inFile.close();

    // Kiểm tra xem file có trống không
    if (loadedData.empty()) {
        std::cerr << "LỖI: File save bi trong (empty data)!" << std::endl;
        return;
    }

    // 5. Chuyển dữ liệu cho Hiếu (Board) xử lý phục hồi
    board.loadBoardData(loadedData);
    std::cout << " Tai dữ liệu Board thanh cong!" << std::endl;

    // 6. Chuyển trạng thái sang PLAYING để người chơi tiếp tục
    changeState(GameState::PLAYING);
}