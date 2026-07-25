#include "UIManager2.h"

// IMPLEMENTATION: BUTTON

Button::Button() : isHovered(false), isPressed(false) {}

Button::Button(float x, float y, float width, float height,
    const sf::Font& font, const std::string& text,
    sf::Color normalColor, sf::Color hColor, sf::Color aColor)
    : idleColor(normalColor), hoverColor(hColor), activeColor(aColor),
    isHovered(false), isPressed(false)
{
    // Cấu hình khung hình chữ nhật cho nút
    shape.setPosition(sf::Vector2f(x, y));
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(idleColor);
    shape.setOutlineThickness(2.0f);
    shape.setOutlineColor(sf::Color::White);

    // Cấu hình chữ hiển thị trong nút
    buttonText.setFont(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(20);
    buttonText.setFillColor(sf::Color::White);

    // Căn giữa chữ vào trong khung Button
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(textBounds.left + textBounds.width / 2.0f,
        textBounds.top + textBounds.height / 2.0f);
    buttonText.setPosition(x + width / 2.0f, y + height / 2.0f);
}

void Button::setPosition(float x, float y) {
    shape.setPosition(sf::Vector2f(x, y));
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setPosition(x + shape.getSize().x / 2.0f, y + shape.getSize().y / 2.0f);
}

void Button::setString(const std::string& text) {
    buttonText.setString(text);
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(textBounds.left + textBounds.width / 2.0f,
        textBounds.top + textBounds.height / 2.0f);
    buttonText.setPosition(shape.getPosition().x + shape.getSize().x / 2.0f,
        shape.getPosition().y + shape.getSize().y / 2.0f);
}

void Button::updateMousePosition(const sf::Vector2f& mousePos) {
    // Kiểm tra xem con trỏ chuột có nằm trong diện tích Nút không
    if (shape.getGlobalBounds().contains(mousePos)) {
        isHovered = true;
        shape.setFillColor(hoverColor);
    }
    else {
        isHovered = false;
        shape.setFillColor(idleColor);
    }
}

void Button::update() {
    // Tùy chỉnh các hiệu ứng animation (nếu có)
}

void Button::draw() {
    // Hàm rỗng kế thừa từ IGameObject
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(buttonText);
}

bool Button::isClicked(const sf::Event& event, const sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (shape.getGlobalBounds().contains(mousePos)) {
            return true;
        }
    }
    return false;
}


// IMPLEMENTATION: UIMANAGER

UIManager::UIManager()
    : timerSeconds(0), timerAcc(0.0f), mistakes(0), hintsRemaining(3)
{
    // Load Font mặc định (Chú ý: Đảm bảo có file font trong thư mục chạy)
    if (!mainFont.loadFromFile("arial.ttf")) {
        // Dự phòng nếu không tải được font
        std::cerr << "[Warning] Call UIManager: Zero font 'arial.ttf' found!\n";
    }

    // Khởi tạo các đoạn Text UI HUD
    timerText.setFont(mainFont);
    timerText.setCharacterSize(22);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(50.0f, 20.0f);

    mistakeText.setFont(mainFont);
    mistakeText.setCharacterSize(22);
    mistakeText.setFillColor(sf::Color(230, 80, 80)); // Đỏ nhạt
    mistakeText.setPosition(250.0f, 20.0f);

    hintText.setFont(mainFont);
    hintText.setCharacterSize(22);
    hintText.setFillColor(sf::Color::Yellow);
    hintText.setPosition(450.0f, 20.0f);

    // Khởi tạo Buttons
    playButton = Button(250, 300, 150, 50, mainFont, "PLAY");
    pauseButton = Button(550, 15, 80, 35, mainFont, "PAUSE");
    hintButton = Button(450, 50, 100, 40, mainFont, "HINT", sf::Color(180, 140, 40));
}

void UIManager::update(float dt) {
    // Tích lũy Delta-time để tăng Timer mỗi giây
    timerAcc += dt;
    if (timerAcc >= 1.0f) {
        timerSeconds++;
        timerAcc -= 1.0f;
    }

    // Format chuỗi hiển thị thời gian MM:SS
    int mins = timerSeconds / 60;
    int secs = timerSeconds % 60;
    std::stringstream ss;
    ss << "Time: " << std::setfill('0') << std::setw(2) << mins << ":"
        << std::setfill('0') << std::setw(2) << secs;
    timerText.setString(ss.str());

    // Cập nhật text Mistake & Hint
    mistakeText.setString("Mistakes: " + std::to_string(mistakes) + "/3");
    hintText.setString("Hints: " + std::to_string(hintsRemaining));
}

// --- RENDER MÀN HÌNH CHÍNH ---

void UIManager::drawMenuScreen(sf::RenderWindow& window) {
    // Phủ phông nền Menu
    window.clear(sf::Color(30, 30, 45));

    sf::Text title("SUDOKU GAME", mainFont, 48);
    title.setFillColor(sf::Color::Cyan);

    // Căn giữa Title
    sf::FloatRect textBounds = title.getLocalBounds();
    title.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    title.setPosition(window.getSize().x / 2.0f, 150.0f);

    window.draw(title);

    // Vẽ Button Play trên Menu
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    playButton.updateMousePosition(mousePos);
    playButton.draw(window);
}

void UIManager::drawPauseScreen(sf::RenderWindow& window) {
    // Tạo lớp phủ mờ (Overlay) đen trong suốt lên màn hình đang chơi
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    sf::Text pauseText("GAME PAUSED", mainFont, 40);
    pauseText.setFillColor(sf::Color::White);

    sf::FloatRect bounds = pauseText.getLocalBounds();
    pauseText.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    pauseText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    window.draw(pauseText);
}

// --- OVERLAY GAMEPLAY UI ---

void UIManager::drawTimer(sf::RenderWindow& window) {
    window.draw(timerText);
}

void UIManager::drawMistakeCounter(sf::RenderWindow& window) {
    window.draw(mistakeText);
}

void UIManager::drawHintIcon(sf::RenderWindow& window) {
    window.draw(hintText);
    hintButton.draw(window);
}

// --- XỬ LÝ SỰ KIỆN ---

void UIManager::handleMouseInput(const sf::Event& event, sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Cập nhật trạng thái Hover cho các nút
    pauseButton.updateMousePosition(mousePos);
    hintButton.updateMousePosition(mousePos);

    // Kiểm tra sự kiện Click
    if (pauseButton.isClicked(event, window)) {
        std::cout << "[UI] Clicked Pause Button!\n";
        // Call GameEngine sang trạng thái PAUSED (Phú Trọng xử lý)
    }

    if (hintButton.isClicked(event, window)) {
        if (hintsRemaining > 0) {
            hintsRemaining--;
            std::cout << "[UI] Used 1 Hint! Remaining: " << hintsRemaining << "\n";
            // Call SudokuBoard giải gợi ý 1 ô (Duy Hiếu xử lý)
        }
    }
}

void UIManager::handleKeyboardInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        // Xử lý phím tắt nhanh
        if (event.key.code == sf::Keyboard::P) {
            std::cout << "[UI] Pressed 'P' -> Toggle Pause!\n";
        }
        if (event.key.code == sf::Keyboard::H) {
            std::cout << "[UI] Pressed 'H' -> Request Hint!\n";
        }
    }
}
