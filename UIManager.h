#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "SoundManager.h" // Thêm include cho SoundManager
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <optional>
#include <algorithm>
#include <fstream>

class Button  {
private:
    sf::RectangleShape shape;
    sf::Text buttonText;
    std::optional<sf::Sprite> buttonSprite;
    bool useTexture = false;
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
    void setTexture(sf::Texture& texture) {
    texture.setSmooth(false);
    buttonSprite.emplace(texture);
    buttonSprite->setPosition(shape.getPosition());
    
    sf::Vector2u texSize = texture.getSize();
    sf::Vector2f shapeSize = shape.getSize();
    buttonSprite->setScale(sf::Vector2f(shapeSize.x / texSize.x, shapeSize.y / texSize.y));
    
    useTexture = true;
}

    // Update trạng thái nút theo vị trí chuột
    void updateMousePosition(const sf::Vector2f& mousePos);

    // Override từ IGameObject
    void update();
    void draw();
    void draw(sf::RenderWindow& window); // Hàm vẽ chính truyền window

    // Kiểm tra người dùng có click vào nút không (xử lý Mouse Input)
    bool isClicked(const sf::Event& event,const sf::RenderWindow& window);
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
    int selectedCol=-1;
    int selectedRow=-1;

    sf::Font mainFont;

    sf::Texture menuBgTexture;
    sf::Texture gameBgTexture;

    sf::Sprite menuBgSprite{ menuBgTexture };
    sf::Sprite gameBgSprite{ gameBgTexture };

    sf::Texture playBtnTexture;
    sf::Texture replayBtnTexture;
    sf::Texture settingsBtnTexture;
    sf::Texture easyBtnTexture;
    sf::Texture mediumBtnTexture;
    sf::Texture hardBtnTexture;
    sf::Texture continueBtnTexture;
    sf::Texture newGameBtnTexture;

    // UI Elements
    sf::Text timerText;
    sf::Text mistakeText;
    sf::Text hintText;

    sf::Texture timerIconTexture;
    sf::Texture mistakeIconTexture;
    sf::Texture hintIconTexture;
    sf::Texture undoIconTexture;
    sf::Texture redoIconTexture;

    sf::Sprite timerIconSprite{ timerIconTexture };
    sf::Sprite mistakeIconSprite{ mistakeIconTexture };
    sf::Sprite hintIconSprite{ hintIconTexture };
    sf::Sprite undoIconSprite{ undoIconTexture };
    sf::Sprite redoIconSprite{ redoIconTexture };

    // Các Button chính
    Button playButton;
    Button pauseButton;
    Button hintButton;
    Button continueButton;
    Button escButton;
    Button menuButton;
    Button replayButton;
    Button newGameButton; 
    Button settingsButton;
    Button undoButton;
    Button redoButton;

    Button easyButton;
    Button mediumButton;
    Button hardButton;

    sf::Text settingsTitle;

    sf::Texture volumeIconTexture;
    sf::Sprite volumeIconSprite{ volumeIconTexture };

    sf::Texture minusBtnTexture;
    sf::Texture plusBtnTexture;
    Button btnVolumeMinus;
    Button btnVolumePlus;

    sf::Text volumeValueText;
    int volumeLevel = 50; // Giá trị âm lượng mặc định (0 - 100)

    // Image Buttons cho Show Record và Back
    sf::Texture showRecordBtnTexture;
    sf::Texture backBtnTexture;
    Button recordButton;
    Button backButton;

public:
    UIManager();

    int recordEasy = 9999, recordMedium = 9999, recordHard = 9999;
    void loadRecords();
    void saveRecord(int difficulty, int timeInSeconds);
    void drawSettingsScreen(sf::RenderWindow& window);
    
    bool isVolumeMinusClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isVolumePlusClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isRecordButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isBackButtonClicked(const sf::Event& event, sf::RenderWindow& window);

    int getVolumeLevel() const { return volumeLevel; }
    void setVolumeLevel(int level) { 
        volumeLevel = std::clamp(level, 0, 100); 
        volumeValueText.setString(std::to_string(volumeLevel));
    }

    // Setters / Getters hỗ trợ
    void setTimerSeconds(int timer) { timerSeconds = (float)timer; }
    void setMistakes(int count) { mistakes = count; }
    void setHints(int count) { hintsRemaining = count; }
    void resetTimer() { timerSeconds = 0; timerAcc = 0.0f; }
    void resetSelectedCell() { selectedRow = -1; selectedCol = -1; }
    void resetMistakes() { mistakes = 0; }
    void resetHint() { hintsRemaining = 3; }

    void update(float dt = 0.016f);

    // --- RENDER CÁC MÀN HÌNH CHÍNH ---
    void drawMenuScreen(sf::RenderWindow& window);
    void drawPauseScreen(sf::RenderWindow& window);
    void drawWinLoseScreen(sf::RenderWindow& window,bool isWin);
    void drawDifficultyScreen(sf::RenderWindow& window);
    void drawLeaderboard(sf::RenderWindow& window);
    void drawGameBackground(sf::RenderWindow& window);  

    // --- OVERLAY GAMEPLAY UI ---
    void drawTimer(sf::RenderWindow& window);
    void drawMistakeCounter(sf::RenderWindow& window);
    void drawHintIcon(sf::RenderWindow& window);
    void drawPauseIcon(sf::RenderWindow& window);
    void drawEscIcon(sf::RenderWindow& window);
    void drawUndoIcon(sf::RenderWindow& window);
    void drawRedoIcon(sf::RenderWindow& window);

    // --- XỬ LÝ SỰ KIỆN SFML ---
    void handleMouseInput( const sf::Event& event, sf::RenderWindow& window, float gridOffsetX, float gridOffsetY, float cellSize);
    void handleKeyboardInput(const sf::Event& event);
    int handleNumberInput(const sf::Event& event);

    void addMistake() { mistakes++; }
    int getMistakes() const { return mistakes; }
    int getSelectedRow() const { return selectedRow; }
    int getSelectedCol() const { return selectedCol; }
    int getHintRemaining() const { return hintsRemaining; }
    float getTimerSeconds() const { return timerSeconds; }

    // Trong UIManager.h
    // Hàm kiểm tra xem nút Play trên menu đã được click chưa
    bool isPlayButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    void updateMenuMouse(const sf::Vector2f& mousePos); // Cập nhật vị trí chuột cho menu nếu cần
    bool isPauseButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isContinueButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isEscButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isMenuButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isReplayButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isNewGameButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isSettingsButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isHintButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isUndoButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isRedoButtonClicked(const sf::Event& event, sf::RenderWindow& window);

    bool isEasyButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isMediumButtonClicked(const sf::Event& event, sf::RenderWindow& window);
    bool isHardButtonClicked(const sf::Event& event, sf::RenderWindow& window);
};