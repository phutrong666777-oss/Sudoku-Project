#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <iostream>

// Base Class Interface từ architecture gốc
class IGameObject {
public:
    virtual void update() = 0;
    virtual void draw() = 0; // Để tương thích, có thể xài hoặc dùng hàm overload có sf::RenderWindow
    virtual ~IGameObject() {}
};

// CLASS BUTTON: Nút bấm giao diện SFML
class Button : public IGameObject {
private:
    sf::RectangleShape shape;
    sf::Text buttonText;
    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color activeColor;

    bool isHovered;
    bool isPressed;

public:
    Button();
    Button(float x, float y, float width, float height,
        const sf::Font& font, const std::string& text,
        sf::Color normalColor = sf::Color(70, 130, 180),
        sf::Color hColor = sf::Color(100, 149, 237),
        sf::Color aColor = sf::Color(30, 144, 255));

    void setPosition(float x, float y);
    void setString(const std::string& text);

    // Update trạng thái nút theo vị trí chuột
    void updateMousePosition(const sf::Vector2f& mousePos);

    // Override từ IGameObject
    void update() override;
    void draw() override;
    void draw(sf::RenderWindow& window); // Hàm vẽ chính truyền window

    // Kiểm tra người dùng có click vào nút không (xử lý Mouse Input)
    bool isClicked(const sf::Event& event, const sf::RenderWindow& window);
};

// =========================================================================
// CLASS UIMANAGER: Quản lý toàn bộ giao diện và màn hình SFML
// =========================================================================
class UIManager {
private:
    float timerAcc; // BỘ đếm dồn thời gian
    int timerSeconds;
    int mistakes;
    int hintsRemaining;

    sf::Font mainFont;

    // UI Elements
    sf::Text timerText;
    sf::Text mistakeText;
    sf::Text hintText;

    // Các Button chính
    Button playButton;
    Button pauseButton;
    Button hintButton;

public:
    UIManager();

    // Setters / Getters hỗ trợ
    void setMistakes(int count) { mistakes = count; }
    void setHints(int count) { hintsRemaining = count; }
    void resetTimer() { timerSeconds = 0; timerAcc = 0.0f; }

    void update(float dt = 0.016f);

    // --- RENDER CÁC MÀN HÌNH CHÍNH ---
    void drawMenuScreen(sf::RenderWindow& window);
    void drawPauseScreen(sf::RenderWindow& window);

    // --- OVERLAY GAMEPLAY UI ---
    void drawTimer(sf::RenderWindow& window);
    void drawMistakeCounter(sf::RenderWindow& window);
    void drawHintIcon(sf::RenderWindow& window);

    // --- XỬ LÝ SỰ KIỆN SFML ---
    void handleMouseInput(const sf::Event& event, sf::RenderWindow& window);
    void handleKeyboardInput(const sf::Event& event);
};

#endif // UI_MANAGER_H