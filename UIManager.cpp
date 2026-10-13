#include "UIManager.h"

// Khai báo extern để file này có thể nhìn thấy mainFont từ GameStruct.h
extern sf::Font mainFont;
// IMPLEMENTATION: BUTTON
extern SoundManager& soundManager;
// SỬA LỖI 1: sf::Text bắt buộc phải được truyền font ngay khi khởi tạo
Button::Button() : isHovered(false), isPressed(false), buttonText(mainFont) {}

Button::Button(float x, float y, float width, float height,const sf::Font& font, const std::string& text,
    sf::Color normalColor, sf::Color hColor, sf::Color aColor):idleColor(normalColor), hoverColor(hColor),
     activeColor(aColor),isHovered(false), isPressed(false), buttonText(font) // SỬA LỖI 2: Truyền font vào buttonText
{
    // Cấu hình khung hình chữ nhật cho nút
    shape.setPosition(sf::Vector2f(x, y));
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(idleColor);
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::White);

    // Cấu hình chữ hiển thị trong nút (Không cần .setFont nữa vì đã khởi tạo ở trên)
    buttonText.setString(text);
    buttonText.setCharacterSize(20);
    buttonText.setFillColor(sf::Color::White);

    // Căn giữa chữ vào trong khung Button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(sf::Vector2f(
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
    ));
    buttonText.setPosition(sf::Vector2f(x + width / 2.0f, y + height / 2.0f));
}

void Button::setPosition(float x, float y) {
    shape.setPosition(sf::Vector2f(x, y));
    if (buttonSprite.has_value()) {
        buttonSprite->setPosition(sf::Vector2f(x, y));
    }
    buttonText.setPosition(sf::Vector2f(x + shape.getSize().x / 2.0f, y + shape.getSize().y / 2.0f));
}

void Button::setString(const std::string& text) {
    buttonText.setString(text);
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    
    buttonText.setOrigin(sf::Vector2f(
        textBounds.position.x + textBounds.size.x / 2.0f,
        textBounds.position.y + textBounds.size.y / 2.0f
    ));
    buttonText.setPosition(sf::Vector2f(
        shape.getPosition().x + shape.getSize().x / 2.0f,
        shape.getPosition().y + shape.getSize().y / 2.0f
    ));
}

void Button::updateMousePosition(const sf::Vector2f& mousePos) {
    if (shape.getGlobalBounds().contains(mousePos)) {
        isHovered = true;
        shape.setFillColor(hoverColor);
        // Hiệu ứng hover cho ảnh: làm tối nhẹ ảnh
        if (useTexture && buttonSprite.has_value()) {
            buttonSprite->setColor(sf::Color(210, 210, 210)); 
        }
    } else {
        isHovered = false;
        shape.setFillColor(idleColor);
        // Trả lại màu gốc của ảnh
        if (useTexture && buttonSprite.has_value()) {
            buttonSprite->setColor(sf::Color(255, 255, 255)); 
        }
    }
}

void Button::update() {
    // Tùy chỉnh các hiệu ứng animation (nếu có)
}

void Button::draw() {
    // Hàm rỗng kế thừa từ IGameObject
}

void Button::draw(sf::RenderWindow& window) {
    if (useTexture && buttonSprite.has_value()) {
        window.draw(*buttonSprite); // Vẽ Sprite
    } else {
        window.draw(shape);
        window.draw(buttonText);
    }
}

bool Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) {
    if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseReleased->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (shape.getGlobalBounds().contains(mousePos)) {
                soundManager.playClickSound();
                return true;
            }
        }
    }
    return false;
}


// IMPLEMENTATION: UIMANAGER

// SỬA LỖI 3: Khởi tạo các Text UI kèm theo mainFont qua Initializer List
UIManager::UIManager()
    : timerSeconds(0), timerAcc(0.0f), mistakes(0), hintsRemaining(3),
      timerText(mainFont), mistakeText(mainFont), hintText(mainFont),
      menuBgSprite(menuBgTexture),settingsTitle(mainFont), volumeValueText(mainFont),
      gameBgSprite(gameBgTexture)
{
    if (!mainFont.openFromFile("../assets/arial.ttf")) {
        std::cerr << "[Warning] Call UIManager: Zero font '../assets/arial.ttf' found!\n";
    }
    
    // LOAD HÌNH NỀN
    if (menuBgTexture.loadFromFile("../assets/menu.png")) {
        menuBgSprite.setTexture(menuBgTexture, true); 
        
        // Scale hình vừa vặn kích thước cửa sổ 800x600
        sf::Vector2u texSize = menuBgTexture.getSize();
        menuBgSprite.setScale(sf::Vector2f(800.0f / texSize.x, 600.0f / texSize.y));
    } else {
        std::cerr << "[Warning] Could not load menu_bg.png!\n";
    }

    if (gameBgTexture.loadFromFile("../assets/game_bg.png")) { 
        gameBgSprite.setTexture(gameBgTexture, true);
        
        sf::Vector2u texSize = gameBgTexture.getSize();
        gameBgSprite.setScale(sf::Vector2f(800.0f / texSize.x, 600.0f / texSize.y));
    } else {
        std::cerr << "[Warning] Could not load game_bg.png!\n";
    }

    // --- 1. NÚT ESC (GÓC TRÊN BÊN TRÁI) ---
    escButton = Button(15, 12, 70, 35, mainFont, "ESC", sf::Color(100, 100, 120));

    // --- 2. CỤM ICONS & CHỈ SỐ HUD ---
    float targetIconSize = 42.0f; // Đưa về size 42px để vừa vặn, không bị đè nhau

    // A. Icon & Text Thời gian
    
    sf::Image timerImg;
    if (timerImg.loadFromFile("../assets/timer_icon.png")) {
        timerImg.createMaskFromColor(sf::Color::White);
        (void)timerIconTexture.loadFromImage(timerImg);
        
        timerIconSprite.setTexture(timerIconTexture, true);
        sf::Vector2u tSize = timerIconTexture.getSize();

        // --- TĂNG KÍCH THƯỚC TO NỔI BẬT (48x64 px) ---
        float iconWidth = 48.0f;
        float iconHeight = 64.0f;
        
        timerIconSprite.setScale(sf::Vector2f(
            iconWidth / static_cast<float>(tSize.x), 
            iconHeight / static_cast<float>(tSize.y)
        ));
        
        // Đặt Y = 0.0f để icon to không bị đè xuống dưới
        timerIconSprite.setPosition(sf::Vector2f(95.0f, 0.0f));
    } else {
        std::cerr << "[Error] Cannot load timer_icon.png!\n";
    }

    // Dịch chữ thời gian lùi hẳn sang X = 152px cho rộng rãi, đẹp mắt
    timerText.setCharacterSize(24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(sf::Vector2f(152.0f, 18.0f));
    
    // B. Icon & Text Cảnh báo lỗi (Mistakes)
    sf::Image mistakeImg;
    if (mistakeImg.loadFromFile("../assets/heart_icon.png")) {
        mistakeImg.createMaskFromColor(sf::Color::White);
        (void)mistakeIconTexture.loadFromImage(mistakeImg);
        
        mistakeIconSprite.setTexture(mistakeIconTexture, true);
        sf::Vector2u mSize = mistakeIconTexture.getSize();
        mistakeIconSprite.setScale(sf::Vector2f(targetIconSize / mSize.x, targetIconSize / mSize.y));
        mistakeIconSprite.setPosition(sf::Vector2f(280.0f, 8.0f));
    }
    mistakeText.setCharacterSize(22);
    mistakeText.setFillColor(sf::Color(230, 80, 80));
    // Dịch chữ 0/3 sang phải (X = 330) để KHÔNG BỊ ĐÈ lên icon tam giác nữa
    mistakeText.setPosition(sf::Vector2f(330.0f, 16.0f)); 


    // C. Icon & Text Gợi ý (Hint)
    if (hintIconTexture.loadFromFile("../assets/hint_icon.png")) {
        hintIconSprite.setTexture(hintIconTexture, true);
        sf::Vector2u hSize = hintIconTexture.getSize();
        
        // Kéo dài icon bóng đèn (Chiều rộng 58px, chiều cao 42px)
        float hintWidth = 58.0f; 
        float hintHeight = 42.0f;
        
        hintIconSprite.setScale(sf::Vector2f(hintWidth / hSize.x, hintHeight / hSize.y));
        hintIconSprite.setPosition(sf::Vector2f(440.0f, 8.0f));
    }
    hintText.setCharacterSize(22);
    hintText.setFillColor(sf::Color::Yellow);
    hintText.setPosition(sf::Vector2f(510.0f, 16.0f)); // Dịch số gợi ý nằm sau Icon

    // Nút HINT trùng khớp hoàn toàn vị trí với hintIconSprite để bắt sự kiện click
    hintButton = Button(440, 8, 58, 42, mainFont, "", sf::Color::Transparent, sf::Color::Transparent, sf::Color::Transparent);

    float btnW = 220.0f;
    float btnH = 60.0f;
    float btnX = (800.0f - btnW) / 2.0f;

    // ==========================================
// 1. KHỞI TẠO NÚT UNDO
// ==========================================
if (undoIconTexture.loadFromFile("../assets/undo_icon.png")) {
    undoIconSprite.setTexture(undoIconTexture, true);
    sf::Vector2u uSize = undoIconTexture.getSize();

    float iconWidth = 58.0f;
    float iconHeight = 42.0f;

    undoIconSprite.setScale(sf::Vector2f(iconWidth / uSize.x, iconHeight / uSize.y));
    // Đặt ở vị trí X = 550.0f
    undoIconSprite.setPosition(sf::Vector2f(550.0f, 8.0f));
}
// Khởi tạo vùng click cho Undo tương ứng
undoButton = Button(550, 8, 58, 42, mainFont, "", sf::Color::Transparent, sf::Color::Transparent);


// ==========================================
// 2. KHỞI TẠO NÚT REDO
// ==========================================
if (redoIconTexture.loadFromFile("../assets/redo_icon.png")) {
    redoIconSprite.setTexture(redoIconTexture, true);
    sf::Vector2u rSize = redoIconTexture.getSize();

    float iconWidth = 58.0f;
    float iconHeight = 42.0f;

    redoIconSprite.setScale(sf::Vector2f(iconWidth / rSize.x, iconHeight / rSize.y));
    // Đặt ở vị trí X = 615.0f để nối tiếp ngay sau Undo
    redoIconSprite.setPosition(sf::Vector2f(615.0f, 8.0f));
}
// Khởi tạo vùng click cho Redo tương ứng
redoButton = Button(615, 8, 58, 42, mainFont, "", sf::Color::Transparent, sf::Color::Transparent);


    // --- 3. NÚT PAUSE (GÓC TRÊN BÊN PHẢI) ---
    pauseButton = Button(680, 12, 85, 35, mainFont, "PAUSE", sf::Color(180, 70, 70));
    if (!playBtnTexture.loadFromFile("../assets/play_btn.png"))
        std::cerr << "[Error] Cannot load play_btn.png!\n";

    if (!settingsBtnTexture.loadFromFile("../assets/setting_btn.png"))
        std::cerr << "[Error] Cannot load setting_btn.png!\n";

    if (!replayBtnTexture.loadFromFile("../assets/replay_btn.png"))
        std::cerr << "[Error] Cannot load replay_btn.png!\n";

    if (!easyBtnTexture.loadFromFile("../assets/easy_btn.png"))
        std::cerr << "[Error] Cannot load easy_btn.png!\n";

    if (!mediumBtnTexture.loadFromFile("../assets/medium_btn.png"))
        std::cerr << "[Error] Cannot load medium_btn.png!\n";

    if (!hardBtnTexture.loadFromFile("../assets/hard_btn.png"))
        std::cerr << "[Error] Cannot load hard_btn.png!\n";

    if (!continueBtnTexture.loadFromFile("../assets/continue_btn.png"))
        std::cerr << "[Error] Cannot load continue_btn.png!\n";
    
    if (!newGameBtnTexture.loadFromFile("../assets/newGame_btn.png"))
        std::cerr << "[Error] Cannot load newGame_btn.png!\n"; 
    // --- 2. KHỞI TẠO VÀ GÁN TEXTURE CHO CÁC NÚT ---
    // (Lưu ý: Để chuỗi text rỗng "" vì chữ đã có sẵn trên ảnh nút)

    float btnWidth = 200.0f; 
    float btnHeight = 60.0f; 
    float posX = (800.0f - btnWidth) / 2.0f;

    // --- MÀN HÌNH MENU CHÍNH ---
    playButton     = Button(posX, 250.0f, btnWidth, btnHeight, mainFont, "");
    playButton.setTexture(playBtnTexture);

    settingsButton = Button(posX, 325.0f, btnWidth, btnHeight, mainFont, "");
    settingsButton.setTexture(settingsBtnTexture);

    replayButton   = Button(posX, 400.0f, btnWidth, btnHeight, mainFont, "");
    replayButton.setTexture(replayBtnTexture);

    // --- MÀN HÌNH CHỌN ĐỘ KHÓ ---
    easyButton     = Button(posX, 250.0f, btnWidth, btnHeight, mainFont, "");
    easyButton.setTexture(easyBtnTexture);

    mediumButton   = Button(posX, 325.0f, btnWidth, btnHeight, mainFont, "");
    mediumButton.setTexture(mediumBtnTexture);

    hardButton     = Button(posX, 400.0f, btnWidth, btnHeight, mainFont, "");
    hardButton.setTexture(hardBtnTexture);

    // --- MÀN HÌNH TẠM DỪNG ---
    continueButton     = Button(posX, 350.0f, btnWidth, btnHeight, mainFont, "");
    continueButton.setTexture(continueBtnTexture);

    // --- MÀN HÌNH WIN/LOOSE ---
    newGameButton     = Button(posX, 350.0f, btnWidth, btnHeight, mainFont, "");
    newGameButton.setTexture(newGameBtnTexture);

    settingsTitle.setString("SETTINGS");
    settingsTitle.setCharacterSize(45);
    settingsTitle.setFillColor(sf::Color::White);
    settingsTitle.setStyle(sf::Text::Bold);
    sf::FloatRect stBounds = settingsTitle.getLocalBounds();
    settingsTitle.setOrigin(sf::Vector2f(stBounds.position.x + stBounds.size.x / 2.0f, stBounds.position.y + stBounds.size.y / 2.0f));
    settingsTitle.setPosition(sf::Vector2f(400.0f, 100.0f));
    
    (void)volumeIconTexture.loadFromFile("../assets/volume_icon.png");
    (void)minusBtnTexture.loadFromFile("../assets/btn_minus.png");
    (void)plusBtnTexture.loadFromFile("../assets/btn_plus.png");
    (void)showRecordBtnTexture.loadFromFile("../assets/record.png");
    (void)backBtnTexture.loadFromFile("../assets/back.png");

    // 2. Cụm điều chỉnh Volume (Y = 200)
    float volumeY = 200.0f;

    // A. Icon Loa (Nằm bên trái)
    volumeIconSprite.setTexture(volumeIconTexture, true);
    sf::Vector2u vSize = volumeIconTexture.getSize();
    volumeIconSprite.setScale(sf::Vector2f(50.0f / vSize.x, 50.0f / vSize.y));
    volumeIconSprite.setPosition(sf::Vector2f(220.0f, volumeY));

    // B. Nút Trừ (-)
    btnVolumeMinus = Button(290.0f, volumeY, 50.0f, 50.0f, mainFont, "");
    btnVolumeMinus.setTexture(minusBtnTexture);

    // C. Hiển thị con số Volume
    volumeValueText.setCharacterSize(28);
    volumeValueText.setFillColor(sf::Color::White);
    volumeValueText.setStyle(sf::Text::Bold);
    volumeValueText.setString(std::to_string(volumeLevel));
    
    // D. Nút Cộng (+)
    btnVolumePlus = Button(460.0f, volumeY, 50.0f, 50.0f, mainFont, "");
    btnVolumePlus.setTexture(plusBtnTexture);

    // 3. Nút SHOW RECORD & BACK (Nút dùng Texture ảnh)

    recordButton = Button(btnX, 290.0f, btnW, btnH, mainFont, "");
    recordButton.setTexture(showRecordBtnTexture);

    backButton = Button(btnX, 370.0f, btnW, btnH, mainFont, "");
    backButton.setTexture(backBtnTexture);
}

void UIManager::update(float dt) {
    timerAcc += dt;
    if (timerAcc >= 1.0f) {
        timerSeconds++;
        timerAcc -= 1.0f;
    }

    int mins = timerSeconds / 60;
    int secs = timerSeconds % 60;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << mins << ":"
       << std::setfill('0') << std::setw(2) << secs;
    timerText.setString(ss.str());

    mistakeText.setString(std::to_string(mistakes) + "/3");

    hintText.setString(std::to_string(hintsRemaining));
}

// --- RENDER MÀN HÌNH CHÍNH ---

void UIManager::drawMenuScreen(sf::RenderWindow& window) {
    // 1. Vẽ hình nền menu
    window.draw(menuBgSprite);

    // 2. Cập nhật vị trí chuột và vẽ các nút bấm
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    
    playButton.updateMousePosition(mousePos);
    playButton.draw(window);
    
    // --- Thêm nút SETTINGS ---
    settingsButton.updateMousePosition(mousePos);
    settingsButton.draw(window);

    replayButton.updateMousePosition(mousePos);
    replayButton.draw(window);
}

void UIManager::drawGameBackground(sf::RenderWindow& window) {
    window.draw(gameBgSprite);
}

void UIManager::drawPauseScreen(sf::RenderWindow& window) {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    // SỬA LỖI 6: Dùng constructor trực tiếp 
    sf::Text pauseText(mainFont, "GAME PAUSED", 40);
    pauseText.setFillColor(sf::Color::White);

    sf::FloatRect bounds = pauseText.getLocalBounds();
    pauseText.setOrigin(sf::Vector2f(
        bounds.position.x + bounds.size.x / 2.0f, 
        bounds.position.y + bounds.size.y / 2.0f
    ));
    pauseText.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));

    window.draw(pauseText);
    continueButton.draw(window);
}

void UIManager::drawDifficultyScreen(sf::RenderWindow& window) {
    // 1. Vẽ nền menu
    window.draw(menuBgSprite);

    // 2. Cập nhật vị trí chuột cho các nút độ khó
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    easyButton.updateMousePosition(mousePos);
    mediumButton.updateMousePosition(mousePos);
    hardButton.updateMousePosition(mousePos);

    // 3. Vẽ 3 nút lên màn hình
    easyButton.draw(window);
    mediumButton.draw(window);
    hardButton.draw(window);
}

// --- OVERLAY GAMEPLAY UI ---

void UIManager::drawTimer(sf::RenderWindow& window) {
    window.draw(timerIconSprite); 
    window.draw(timerText);   
}

void UIManager::drawMistakeCounter(sf::RenderWindow& window) {
    window.draw(mistakeIconSprite); 
    window.draw(mistakeText);   
}

void UIManager::drawHintIcon(sf::RenderWindow& window) {
    window.draw(hintIconSprite);
    window.draw(hintText);     
    hintButton.draw(window);
}

void UIManager::drawUndoIcon(sf::RenderWindow& window) {
    window.draw(undoIconSprite);   
    undoButton.draw(window);
}

void UIManager::drawRedoIcon(sf::RenderWindow& window) {
    window.draw(redoIconSprite);   
    redoButton.draw(window);
}

void UIManager::drawPauseIcon(sf::RenderWindow& window) {
    pauseButton.draw(window);
}
void UIManager::drawEscIcon(sf::RenderWindow& window) {
    escButton.draw(window);
}

// --- XỬ LÝ SỰ KIỆN ---

// 1. Xử lý sự kiện chuột (Nhận thêm các tham số tọa độ bàn cờ từ GameEngine truyền xuống)
void UIManager::handleMouseInput(const sf::Event& event, sf::RenderWindow& window, float gridOffsetX, float gridOffsetY, float cellSize) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    pauseButton.updateMousePosition(mousePos);
    hintButton.updateMousePosition(mousePos);

    // Xử lý click các nút UI (Pause, Hint)
    if (pauseButton.isClicked(event, window)) {
        std::cout << "[UI] Clicked Pause Button!\n";
    }

    // Xử lý click chọn ô trên Bàn cờ Sudoku
    if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseReleased->button == sf::Mouse::Button::Left) {
            // Kiểm tra xem chuột có click trúng vào khu vực 9x9 của bàn cờ không
            if (mousePos.x >= gridOffsetX && mousePos.x < gridOffsetX + 9 * cellSize &&
                mousePos.y >= gridOffsetY && mousePos.y < gridOffsetY + 9 * cellSize) 
            {
                soundManager.playClickSound();
                selectedCol = static_cast<int>((mousePos.x - gridOffsetX) / cellSize);
                selectedRow = static_cast<int>((mousePos.y - gridOffsetY) / cellSize);
                std::cout << "[UI] Selected Cell: (" << selectedRow << ", " << selectedCol << ")\n";
            }
        }
    }
}

// 2. Xử lý phím chức năng hệ thống (Phím 'P' để Pause, phím 'H' để gọi Gợi ý)
void UIManager::handleKeyboardInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        
        // 1. Xử lý các phím chức năng (P: Pause, H: Hint)
        if (keyPressed->code == sf::Keyboard::Key::P) {
            std::cout << "[UI] Pressed 'P' -> Toggle Pause!\n";
        }
        if (keyPressed->code == sf::Keyboard::Key::H) {
            std::cout << "[UI] Pressed 'H' -> Request Hint!\n";
            if (hintsRemaining > 0) {
                hintsRemaining--;
                std::cout << "[UI] Used 1 Hint via Keyboard! Remaining: " << hintsRemaining << "\n";
            }
        }

        // 2. Xử lý điều hướng bằng bàn phím (Mũi tên hoặc W/A/S/D)
        bool isNavigating = (
            keyPressed->code == sf::Keyboard::Key::Up || keyPressed->code == sf::Keyboard::Key::W ||
            keyPressed->code == sf::Keyboard::Key::Down || keyPressed->code == sf::Keyboard::Key::S ||
            keyPressed->code == sf::Keyboard::Key::Left || keyPressed->code == sf::Keyboard::Key::A ||
            keyPressed->code == sf::Keyboard::Key::Right || keyPressed->code == sf::Keyboard::Key::D
        );

        if (isNavigating) {
            // Nếu chưa có ô nào được chọn (trạng thái -1, -1), đặt mặc định ở giữa bảng (4, 4)
            if (selectedRow == -1 || selectedCol == -1) {
                selectedRow = 4;
                selectedCol = 4;
            } else {
                // Thay đổi tọa độ dựa trên phím bấm (Có hiệu ứng cuộn vòng qua cạnh đối diện)
                switch (keyPressed->code) {
                    case sf::Keyboard::Key::Up:
                    case sf::Keyboard::Key::W:
                        selectedRow = (selectedRow > 0) ? selectedRow - 1 : 8; // Lên
                        break;
                    case sf::Keyboard::Key::Down:
                    case sf::Keyboard::Key::S:
                        selectedRow = (selectedRow < 8) ? selectedRow + 1 : 0; // Xuống
                        break;
                    case sf::Keyboard::Key::Left:
                    case sf::Keyboard::Key::A:
                        selectedCol = (selectedCol > 0) ? selectedCol - 1 : 8; // Trái
                        break;
                    case sf::Keyboard::Key::Right:
                    case sf::Keyboard::Key::D:
                        selectedCol = (selectedCol < 8) ? selectedCol + 1 : 0; // Phải
                        break;
                    default:
                        break;
                }
            }
            std::cout << "[UI] Keyboard Navigated to Cell: (" << selectedRow << ", " << selectedCol << ")\n";
        }
    }
}

// 3. Xử lý bắt phím số để điền vào bảng Sudoku (Chỉ chạy khi đã chọn ô)
int UIManager::handleNumberInput(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        // Chỉ bắt phím số khi người chơi đã dùng chuột chọn 1 ô hợp lệ trên bàn cờ
        if (selectedRow != -1 && selectedCol != -1) {
            switch (keyPressed->code) {
                case sf::Keyboard::Key::Num1: case sf::Keyboard::Key::Numpad1: return 1;
                case sf::Keyboard::Key::Num2: case sf::Keyboard::Key::Numpad2: return 2;
                case sf::Keyboard::Key::Num3: case sf::Keyboard::Key::Numpad3: return 3;
                case sf::Keyboard::Key::Num4: case sf::Keyboard::Key::Numpad4: return 4;
                case sf::Keyboard::Key::Num5: case sf::Keyboard::Key::Numpad5: return 5;
                case sf::Keyboard::Key::Num6: case sf::Keyboard::Key::Numpad6: return 6;
                case sf::Keyboard::Key::Num7: case sf::Keyboard::Key::Numpad7: return 7;
                case sf::Keyboard::Key::Num8: case sf::Keyboard::Key::Numpad8: return 8;
                case sf::Keyboard::Key::Num9: case sf::Keyboard::Key::Numpad9: return 9;
                case sf::Keyboard::Key::Delete: 
                case sf::Keyboard::Key::Backspace: return 0; // Trả về 0 để xóa số trong ô
                default: break;
            }
        }
    }
    return -1; // Trả về -1 nếu không có phím số nào được nhấn
}

void UIManager::drawWinLoseScreen(sf::RenderWindow& window, bool isWin) {
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    overlay.setFillColor(sf::Color(0, 0, 0, 200)); 
    window.draw(overlay);

    sf::Text resultText(mainFont, isWin ? "YOU WIN!" : "GAME OVER", 70);
    resultText.setFillColor(isWin ? sf::Color::Green : sf::Color::Red);
    resultText.setStyle(sf::Text::Bold);
    
    sf::FloatRect textRect = resultText.getLocalBounds();
    resultText.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f));
    resultText.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.5f));
    
    window.draw(resultText);
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    menuButton.updateMousePosition(mousePos);
    menuButton.draw(window);
    newGameButton.updateMousePosition(mousePos);
    newGameButton.draw(window);
}

void UIManager::drawLeaderboard(sf::RenderWindow& window) {// 1. Vẽ hình nền (Dùng lại nền Menu)
    window.draw(menuBgSprite);

    // 2. Vẽ lớp phủ tối màu để chữ nổi bật hơn
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

    // 3. Vẽ Tiêu đề
    sf::Text titleText(mainFont, "RECORDS", 50);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect bounds = titleText.getLocalBounds();
    titleText.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f));
    titleText.setPosition(sf::Vector2f(window.getSize().x / 2.0f, 100.0f));
    window.draw(titleText);

    // 4. Vẽ Nội dung Record (Hiện tại là chữ giả lập, sau này bạn có thể đọc từ file)

    std::string text = "Easy: " + (recordEasy == 9999 ? "--:--" : std::to_string(recordEasy) + "s") + "\n\n" +
                       "Medium: " + (recordMedium == 9999 ? "--:--" : std::to_string(recordMedium) + "s") + "\n\n" +
                       "Hard: " + (recordHard == 9999 ? "--:--" : std::to_string(recordHard) + "s");

    sf::Text recordContent(mainFont, text, 30);
    recordContent.setFillColor(sf::Color::White);
    sf::FloatRect contentBounds = recordContent.getLocalBounds();
    recordContent.setOrigin(sf::Vector2f(contentBounds.position.x + contentBounds.size.x / 2.0f, contentBounds.position.y + contentBounds.size.y / 2.0f));
    recordContent.setPosition(sf::Vector2f(window.getSize().x / 2.0f, 250.0f));
    window.draw(recordContent);

    // 5. Cập nhật chuột và vẽ nút Back để thoát
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    backButton.updateMousePosition(mousePos);
    backButton.draw(window);
}

void UIManager::updateMenuMouse(const sf::Vector2f& mousePos) {
    playButton.updateMousePosition(mousePos); // Cập nhật trạng thái hover cho nút Play
}

bool UIManager::isPlayButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return playButton.isClicked(event, window);
}
bool UIManager::isPauseButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return pauseButton.isClicked(event, window);
}
bool UIManager::isContinueButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return continueButton.isClicked(event, window);
}
bool UIManager::isEscButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return escButton.isClicked(event, window);
}
bool UIManager::isMenuButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return menuButton.isClicked(event, window);
}
bool UIManager::isReplayButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return replayButton.isClicked(event, window);
}
bool UIManager::isNewGameButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return newGameButton.isClicked(event, window);
}
bool UIManager::isHintButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return hintButton.isClicked(event, window);
}
bool UIManager::isSettingsButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return settingsButton.isClicked(event, window);
}
bool UIManager::isEasyButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return easyButton.isClicked(event, window);
}
bool UIManager::isMediumButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return mediumButton.isClicked(event, window);
}
bool UIManager::isHardButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return hardButton.isClicked(event, window);
}
bool UIManager::isUndoButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return undoButton.isClicked(event, window);
}
bool UIManager::isRedoButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return redoButton.isClicked(event, window);
}

// --- RENDER MÀN HÌNH SETTINGS ---
void UIManager::drawSettingsScreen(sf::RenderWindow& window) {
    window.draw(menuBgSprite);

    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

    // 2. Vẽ Tiêu đề
    window.draw(settingsTitle);

    // 3. Cập nhật vị trí chuột cho các nút
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    btnVolumeMinus.updateMousePosition(mousePos);
    btnVolumePlus.updateMousePosition(mousePos);
    recordButton.updateMousePosition(mousePos);
    backButton.updateMousePosition(mousePos);

    // 4. Vẽ cụm Volume (Loa, Trừ, Số, Cộng)
    window.draw(volumeIconSprite);
    btnVolumeMinus.draw(window);

    // Căn giữa con số vào vị trí giữa nút (-) và (+)
    sf::FloatRect numBounds = volumeValueText.getLocalBounds();
    volumeValueText.setOrigin(sf::Vector2f(numBounds.position.x + numBounds.size.x / 2.0f, numBounds.position.y + numBounds.size.y / 2.0f));
    volumeValueText.setPosition(sf::Vector2f(400.0f, 222.0f));
    window.draw(volumeValueText);

    btnVolumePlus.draw(window);

    // 5. Vẽ các nút Show Record và Back
    recordButton.draw(window);
    backButton.draw(window);
}

bool UIManager::isVolumeMinusClicked(const sf::Event& event, sf::RenderWindow& window) {
    if (btnVolumeMinus.isClicked(event, window)) {
        if (volumeLevel > 0) {
            setVolumeLevel(volumeLevel - 10); // Giảm 10 mỗi lần click
        }
        return true;
    }
    return false;
}

bool UIManager::isVolumePlusClicked(const sf::Event& event, sf::RenderWindow& window) {
    if (btnVolumePlus.isClicked(event, window)) {
        if (volumeLevel < 100) {
            setVolumeLevel(volumeLevel + 10); // Tăng 10 mỗi lần click
        }
        return true;
    }
    return false;
}

bool UIManager::isRecordButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return recordButton.isClicked(event, window);
}

bool UIManager::isBackButtonClicked(const sf::Event& event, sf::RenderWindow& window) {
    return backButton.isClicked(event, window);
}

void UIManager::loadRecords() {
    std::ifstream file("records.txt");
    if (file.is_open()) {
        file >> recordEasy >> recordMedium >> recordHard;
    }
}

void UIManager::saveRecord(int difficulty, int time) {
    if (difficulty == 0 && time < recordEasy) recordEasy = time;
    else if (difficulty == 1 && time < recordMedium) recordMedium = time;
    else if (difficulty == 2 && time < recordHard) recordHard = time;

    std::ofstream file("records.txt");
    file << recordEasy << " " << recordMedium << " " << recordHard;
}