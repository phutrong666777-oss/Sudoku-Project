#include "GameEngine.h"
#include <iostream>
#include <fstream>
#include <sstream>

// Khai báo extern từ các file khác nếu cần
extern sf::RenderWindow window;
extern sf::Font mainFont;
SoundManager& soundManager = SoundManager::getInstance(); // Lấy instance duy nhất của SoundManager
void GameEngine::run() {
    bool isRunning = true;
    soundManager.playBGM("../assets/ezyZip.ogg");
    
    while (isRunning && window.isOpen()) {
        // Pha 1: INPUT - Thu thập toàn bộ sự kiện từ SFML pollEvent
        handleInput(isRunning); 

        // Pha 2: UPDATE - Cập nhật logic game 
        update(); 

        // Pha 3: RENDER - Vẽ các màn hình tương ứng theo State
        window.clear(sf::Color(30, 30, 45)); // Xóa màn hình với màu nền tối đẹp mắt
        renderScenes(); 
        window.display();                     // Đẩy buffer lên màn hình
    }
}

void GameEngine::handleInput(bool& isRunning) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        
        // Xử lý sự kiện đóng cửa sổ chung
        if (event->is<sf::Event::Closed>()) {
            isRunning = false;
            window.close();
            soundManager.stopBGM(); // Dừng nhạc nền khi thoát game
            return;
        }

        // Xử lý Input tách biệt theo GameState
        switch (currentState) {
            case GameState::MENU: {
                // 1. Cập nhật vị trí chuột cho các nút ở màn hình Menu (hiệu ứng hover)
                //uiManager.updateMenuMouse(mousePos);
                // 2. Kiểm tra nếu người chơi click vào nút PLAY
                if (uiManager.isPlayButtonClicked(*event, window)) {
                    std::cout << "[UI] Play Button Clicked! Switching to SELECT_DIFFICULTY state...\n";
                    changeState(GameState::SELECT_DIFFICULTY);
}

                if (uiManager.isSettingsButtonClicked(*event, window)) {
                std::cout << "[UI] Settings Button Clicked!\n";
            }

                if (uiManager.isReplayButtonClicked(*event, window)) {
                    std::cout << "[UI] Replay Button Clicked! Switching to MENU state...\n";
                    loadGame(); // [Trọng] Tải lại game từ file save
                }

                if (uiManager.isSettingsButtonClicked(*event, window)) {
                std::cout << "[UI] Settings Button Clicked! Switching to SETTINGS state...\n";
                changeState(GameState::SETTINGS); // Chuyển state
                }
                break;
            }

            case GameState::SETTINGS: {
            if (uiManager.isBackButtonClicked(*event, window)) {
                std::cout << "[UI] Back Button Clicked! Switching to MENU state...\n";
                changeState(GameState::MENU);
            }
            
            if (uiManager.isVolumeMinusClicked(*event, window)) {
                soundManager.setVolume(uiManager.getVolumeLevel()); // Cập nhật âm lượng sang SoundManager
            }
            if (uiManager.isVolumePlusClicked(*event, window)) {
                soundManager.setVolume(uiManager.getVolumeLevel());
            }
            if (uiManager.isBackButtonClicked(*event, window)) {
                changeState(GameState::MENU); // Quay lại menu chính
            }
            
            if (uiManager.isRecordButtonClicked(*event, window)) {
                uiManager.loadRecords();
                std::cout << "[UI] Show Record Clicked!\n";
                changeState(GameState::RECORD); 
            }
            break;
            }

            case GameState::RECORD: {
                // Thoát khỏi Record quay về Settings (hoặc Menu tùy ý bạn)
                if (uiManager.isBackButtonClicked(*event, window)) {
                    std::cout << "[UI] Back Button Clicked! Returning to SETTINGS state...\n";
                    changeState(GameState::SETTINGS);
                }
                break;
            }

            case GameState::SELECT_DIFFICULTY: {
                if (uiManager.isEasyButtonClicked(*event, window)) {
                    newGame(Difficulty::EASY);
            }
                else if (uiManager.isMediumButtonClicked(*event, window)) {
                    newGame(Difficulty::MEDIUM);
            }
                else if (uiManager.isHardButtonClicked(*event, window)) {
                    newGame(Difficulty::HARD);
            }
    break;
}

            case GameState::PLAYING: {
                float gridX = 175.0f;
                float gridY = 75.0f;
                float cellSize = 50.0f;

                // UIManager bắt sự kiện click chuột (Nút HUD hoặc chọn ô bàn cờ)
                uiManager.handleMouseInput(*event, window, gridX, gridY, cellSize);

                if(uiManager.isPauseButtonClicked(*event, window)){
                    std::cout << "[UI] Pause Button Clicked! Switching to PAUSING state...\n";
                    changeState(GameState::PAUSED);

                }

                if(uiManager.isEscButtonClicked(*event, window)){
                    std::cout << "[UI] Esc Button Clicked! Switching to MENU state...\n";
                    changeState(GameState::MENU);

                }

                if(uiManager.isHintButtonClicked(*event, window)){
                    if (uiManager.getHintRemaining() > 0) {
                        uiManager.setHints(uiManager.getHintRemaining()-1);
                        std::cout << "[UI] Used 1 Hint! Remaining: " << uiManager.getHintRemaining() << "\n";

                        int hintRow = uiManager.getSelectedRow();
                        int hintCol = uiManager.getSelectedCol();

                        if(hintRow != -1 && hintCol != -1 ) {
                            bool hintGiven = board.isGiveHint(hintRow, hintCol);
                            if (hintGiven) {
                                std::cout << "[UI] Hint given at (" << hintRow << ", " << hintCol << ")\n";
                            } else {
                                std::cout << "[UI] Hint not given. Cell already filled or invalid selection.\n";
                            }
                        } else {
                            std::cout << "[UI] No more cell selected for hint.\n";
                        }
                            }
                    
                    }

                if(uiManager.isUndoButtonClicked(*event, window)){
                    if(!board.undo()){
                        cout<<"[BOARD] No number left for undo.\n";

                    }
                }

                if(uiManager.isRedoButtonClicked(*event, window)){
                    if(!board.redo()){
                        cout<<"[BOARD] No number left for redo.\n";
                    }
                }
                
                // UIManager bắt phím chức năng và phím số
                uiManager.handleKeyboardInput(*event);

                int inputVal = uiManager.handleNumberInput(*event); 
                if (inputVal != -1) {
                    int r = uiManager.getSelectedRow();
                    int c = uiManager.getSelectedCol();

                    if (r != -1 && c != -1) {
                        bool isCorrect = board.setCell(r, c, inputVal);
                        if (!isCorrect && inputVal != 0) {
                            uiManager.addMistake();
                        }
                    }
                }
                break;
            }

            case GameState::PAUSED:{
                if(uiManager.isContinueButtonClicked(*event,window)){
                    std::cout << "[UI] Continue Button Clicked! Switching to PLAYING state...\n";
                    changeState(GameState::PLAYING);
                }
                break;
            }
            case GameState::GAME_OVER_WIN:
            case GameState::GAME_OVER_LOSE: {
                if (uiManager.isMenuButtonClicked(*event, window)) {
                    std::cout << "[UI] Menu Button Clicked! Switching to MENU state...\n";
                    changeState(GameState::MENU);
                }

                if (uiManager.isNewGameButtonClicked(*event, window)) {
                    std::cout << "[UI] New Game Button Clicked! Switching to SELECT_DIFFICULTY state...\n";
                    changeState(GameState::SELECT_DIFFICULTY);
                }
                break;
            }
                uiManager.handleMouseInput(*event, window, 175.0f, 75.0f, 50.0f);
                break;
        }
    }
}

void GameEngine::update() {
    switch (currentState) {
        case GameState::MENU:
        case GameState::SELECT_DIFFICULTY:
            break;
        case GameState::PLAYING:
            board.update();    // Update logic bảng Sudoku
            uiManager.update(0.016f); // Update thời gian (Delta time giả định ~ 60FPS)

            // Kiểm tra điều kiện thua (Quá 3 lỗi)
            if (uiManager.getMistakes() >= 3) {
                changeState(GameState::GAME_OVER_LOSE);
            }

            // Kiểm tra điều kiện thắng
            if (board.checkWin()) {
                int currentDifficulty = 0; // Thay bằng biến độ khó thực tế của bạn
                if(board.getCurrentDifficulty()==Difficulty::EASY){
                    currentDifficulty=0;
                }

                if(board.getCurrentDifficulty()==Difficulty::MEDIUM){
                    currentDifficulty=1;
                }

                if(board.getCurrentDifficulty()==Difficulty::HARD){
                    currentDifficulty=2;
                }
                
                uiManager.saveRecord(currentDifficulty, uiManager.getTimerSeconds());
                changeState(GameState::GAME_OVER_WIN);
            }
            break;

        case GameState::PAUSED:
        case GameState::GAME_OVER_WIN:
        case GameState::GAME_OVER_LOSE:
            break;
    }
}

void GameEngine::renderScenes() { 
    switch (currentState) {
        case GameState::MENU:
            uiManager.drawMenuScreen(window);
            break;
        case GameState::SETTINGS: // <-- THÊM CASE NÀY
            uiManager.drawSettingsScreen(window);
            break;
        case GameState::RECORD: // Thêm case hiển thị màn hình Record
            uiManager.drawLeaderboard(window);
            break;
        case GameState::SELECT_DIFFICULTY:
            uiManager.drawDifficultyScreen(window); // <-- THÊM MỚI
            break;
        case GameState::PLAYING:
// 1. Vẽ hình nền Gameplay đầu tiên (nằm dưới cùng)
            uiManager.drawGameBackground(window);

            // 2. Vẽ bàn cờ đè lên nền
            board.draw(window, mainFont, uiManager.getSelectedRow(), uiManager.getSelectedCol()); 
            
            // 3. Vẽ các icon HUD (Timer, Mistakes, Buttons...) đè lên trên cùng
            uiManager.drawTimer(window);
            uiManager.drawMistakeCounter(window);
            uiManager.drawHintIcon(window);
            uiManager.drawPauseIcon(window);
            uiManager.drawEscIcon(window);
            uiManager.drawUndoIcon(window);
            uiManager.drawRedoIcon(window);
            break;

        case GameState::PAUSED:
            uiManager.drawGameBackground(window); // 1. Vẽ nền[cite: 5]
            board.draw(window, mainFont, uiManager.getSelectedRow(), uiManager.getSelectedCol()); // 2. Vẽ bàn cờ[cite: 5]        
            uiManager.drawPauseScreen(window); // 3. Lớp mờ + Nút "GAME PAUSED" đè lên[cite: 3, 5]
            break;

        case GameState::GAME_OVER_WIN:
            uiManager.drawGameBackground(window); // 1. Vẽ nền[cite: 5]
            board.draw(window, mainFont, uiManager.getSelectedRow(), uiManager.getSelectedCol()); // 2. Vẽ bàn cờ đã hoàn thành[cite: 5]
            uiManager.drawWinLoseScreen(window, true); // 3. Lớp mờ + Chữ "YOU WIN!" đè lên[cite: 3, 5]
            break;

        case GameState::GAME_OVER_LOSE:
            uiManager.drawGameBackground(window); // 1. Vẽ nền[cite: 5]
            board.draw(window, mainFont, uiManager.getSelectedRow(), uiManager.getSelectedCol()); // 2. Vẽ bàn cờ bị thua[cite: 5]
            uiManager.drawWinLoseScreen(window, false); // 3. Lớp mờ + Chữ "GAME OVER" đè lên[cite: 3, 5]
            break;
    }
}

void GameEngine::changeState(GameState newState) {
    
    if(currentState == GameState::GAME_OVER_WIN || currentState == GameState::GAME_OVER_LOSE){
        if(newState == GameState::PLAYING){
            newGame(); // Tạo game mới khi chuyển từ GAME_OVER hoặc GAME_WIN sang PLAYING
        }
    }

    if (currentState == GameState::PLAYING && newState == GameState::MENU) {
        saveGame(); // Tự động lưu game khi thoát về Menu
    }

    if (currentState == GameState::PLAYING && newState == GameState::GAME_OVER_LOSE) {
        soundManager.playLM("../assets/looseSound.wav"); 
    }

    if (currentState == GameState::PLAYING && newState == GameState::GAME_OVER_WIN) {
        soundManager.playWM("../assets/winSound.wav"); 
    }
    currentState = newState;
}

void GameEngine::saveGame() {
    const std::string saveFileName = "sudoku_save.json";
    std::cout << "Dang tien hanh luu game...\n";

    std::string boardData = board.serializeBoardData();
    std::string timeData = std::to_string((int)uiManager.getTimerSeconds());
    
    // Cần dùng std::to_string để chuyển độ dài chuỗi (size_t) thành std::string
    std::string timeLength = std::to_string(timeData.size()); 
    
    // Ghép timeData, timeLength cùng với Hint và Mistake
    std::string hint_Mistake_Time_Data = timeData + timeLength + 
                                         std::to_string(uiManager.getHintRemaining()) + 
                                         std::to_string(uiManager.getMistakes());

    std::ofstream outFile(saveFileName);

    if (!outFile.is_open()) {
        std::cerr << "LỖI: Không thể tạo hoặc mở file " << saveFileName << "\n";
        return;
    }

    outFile << boardData + hint_Mistake_Time_Data;
    outFile.close();
    std::cout << "Luu game thanh cong vao file " << saveFileName << "!\n";
}

void GameEngine::loadGame() {
    const std::string saveFileName = "sudoku_save.json";
    std::cout << "Dang tai game tu file...\n";

    std::ifstream inFile(saveFileName);
    if (!inFile.is_open()) {
        std::cerr << "LỖI: Không tìm thấy file " << saveFileName << "\n";
        return;
    }

    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string loadedData = buffer.str();
    inFile.close();

    if (loadedData.empty()) {
        std::cerr << "LỖI: File save bị trống!\n";
        return;
    }

    // --- LOGIC TẢI THỜI GIAN VÀ DỮ LIỆU ---
    // 1. Lấy độ dài chuỗi timeData (nằm ở vị trí thứ 3 từ dưới lên)
    int timeLength = loadedData[loadedData.length() - 3] - '0';

    // 2. Tách chuỗi timeData và khôi phục vào uiManager
    std::string timeData = loadedData.substr(loadedData.length() - 3 - timeLength, timeLength);
    uiManager.setTimerSeconds(std::stoi(timeData)); // Kiểm tra tên hàm setter trong UIManager của bạn (ví dụ: setTimerSeconds hoặc setTimer)

    // 3. Tách dữ liệu bàn chơi (loại bỏ phần timeData + timeLength + hint + mistake ở cuối)
    board.loadBoardData(loadedData.substr(0, loadedData.length() - 3 - timeLength));

    // 4. Lấy Hint và Mistake
    uiManager.setHints(loadedData[loadedData.length() - 2] - '0');
    uiManager.setMistakes(loadedData[loadedData.length() - 1] - '0');

    std::cout << "Tải dữ liệu Board thành công!\n";

    changeState(GameState::PLAYING);
}

void GameEngine::newGame(Difficulty diff) {
    std::cout << "[GameEngine] Reset game state for new PLAYING session with selected difficulty.\n";
    board.generatePuzzle(diff);
    uiManager.resetTimer();
    uiManager.resetSelectedCell();
    uiManager.resetMistakes();
    uiManager.resetHint();
    changeState(GameState::PLAYING);
}