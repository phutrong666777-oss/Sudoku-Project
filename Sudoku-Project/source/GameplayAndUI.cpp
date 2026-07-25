#include "sudoku.h" 
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

extern sf::RenderWindow window; 
extern sf::Font mainFont;

static int selectedRow = -1;
static int selectedCol = -1;

void SudokuCell::draw() {
    sf::RectangleShape cellBox(sf::Vector2f(50.0f, 50.0f)); 
    float x = col * 50.0f + 175.0f; 
    float y = row * 50.0f + 75.0f;
    
    cellBox.setPosition(sf::Vector2f(x, y));
    
    if (row == selectedRow && col == selectedCol) {
        cellBox.setFillColor(sf::Color(200, 200, 200)); 
    } else {
        cellBox.setFillColor(sf::Color::White); 
    }
    
    cellBox.setOutlineThickness(1.0f);
    cellBox.setOutlineColor(sf::Color(200, 200, 200));
    window.draw(cellBox);

    if (value != 0) {
        sf::Text text(mainFont); 
        text.setString(std::to_string(value));
        text.setCharacterSize(30);
        
        if (isFixed) {
            text.setFillColor(sf::Color::Black);
        } else {
            text.setFillColor(sf::Color::Blue);
        }

        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f));
        text.setPosition(sf::Vector2f(x + 25.0f, y + 25.0f));

        window.draw(text);
    }
}

void SudokuCell::update() {}

void SudokuBoard::draw() {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            grid[i][j].draw();
        }
    }

    float startX = 175.0f;
    float startY = 75.0f;
    float cellSize = 50.0f;
    float boardSize = cellSize * 9;

    for (int i = 0; i <= 9; i += 3) {
        sf::RectangleShape hLine(sf::Vector2f(boardSize, 4.0f));
        hLine.setFillColor(sf::Color::Black);
        hLine.setPosition(sf::Vector2f(startX, startY + (i * cellSize) - 2.0f));
        window.draw(hLine);

        sf::RectangleShape vLine(sf::Vector2f(4.0f, boardSize));
        vLine.setFillColor(sf::Color::Black);
        vLine.setPosition(sf::Vector2f(startX + (i * cellSize) - 2.0f, startY));
        window.draw(vLine);
    }
}

void SudokuBoard::update() {}

void Button::update() {}
void Button::draw() {}
bool Button::isClicked() { return false; }

UIManager::UIManager() {
    timerSeconds = 0;
    mistakes = 0;
    hintsRemaining = 3;
}

void UIManager::drawMenuScreen() {
    sf::RectangleShape bg(sf::Vector2f(800.0f, 600.0f));
    bg.setFillColor(sf::Color(40, 40, 40)); 
    window.draw(bg);

    sf::Text titleText(mainFont);
    titleText.setString("SUDOKU");
    titleText.setCharacterSize(80);
    titleText.setFillColor(sf::Color::White); 
    titleText.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleRect = titleText.getLocalBounds();
    titleText.setOrigin(sf::Vector2f(titleRect.position.x + titleRect.size.x / 2.0f, titleRect.position.y + titleRect.size.y / 2.0f));
    titleText.setPosition(sf::Vector2f(800.0f / 2.0f, 150.0f)); 
    window.draw(titleText);

    std::vector<std::string> buttonNames = {"New Game", "Continue", "Leaderboard", "Exit"};
    float startY = 300.0f; 
    float buttonSpacing = 70.0f; 

    for (size_t i = 0; i < buttonNames.size(); ++i) {
        sf::RectangleShape btnBox(sf::Vector2f(250.0f, 50.0f));
        btnBox.setOrigin(sf::Vector2f(btnBox.getSize().x / 2.0f, btnBox.getSize().y / 2.0f));
        btnBox.setPosition(sf::Vector2f(800.0f / 2.0f, startY + i * buttonSpacing));
        
        btnBox.setFillColor(sf::Color(100, 149, 237)); 
        btnBox.setOutlineThickness(2.0f);
        btnBox.setOutlineColor(sf::Color::Black);

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (btnBox.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
            btnBox.setFillColor(sf::Color(65, 105, 225)); 
        }

        window.draw(btnBox);

        sf::Text btnText(mainFont); 
        btnText.setString(buttonNames[i]);
        btnText.setCharacterSize(24);
        btnText.setFillColor(sf::Color::White);
        
        sf::FloatRect textRect = btnText.getLocalBounds();
        btnText.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f));
        btnText.setPosition(btnBox.getPosition());
        window.draw(btnText);
    }
}

void UIManager::drawPauseScreen() {
    sf::RectangleShape overlay(sf::Vector2f(800.0f, 600.0f));
    overlay.setFillColor(sf::Color(0, 0, 0, 150)); 
    window.draw(overlay);

    sf::Text pauseText(mainFont);
    pauseText.setString("PAUSED");
    pauseText.setCharacterSize(60);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setStyle(sf::Text::Bold);
    
    sf::FloatRect textRect = pauseText.getLocalBounds();
    pauseText.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f));
    pauseText.setPosition(sf::Vector2f(800.0f / 2.0f, 200.0f));
    window.draw(pauseText);

    sf::RectangleShape btnBox(sf::Vector2f(200.0f, 50.0f));
    btnBox.setOrigin(sf::Vector2f(100.0f, 25.0f));
    btnBox.setPosition(sf::Vector2f(800.0f / 2.0f, 350.0f));
    btnBox.setFillColor(sf::Color(46, 139, 87)); 
    window.draw(btnBox);

    sf::Text btnText(mainFont);
    btnText.setString("Resume");
    btnText.setCharacterSize(24);
    btnText.setFillColor(sf::Color::White);
    
    sf::FloatRect btnTextRect = btnText.getLocalBounds();
    btnText.setOrigin(sf::Vector2f(btnTextRect.position.x + btnTextRect.size.x / 2.0f, btnTextRect.position.y + btnTextRect.size.y / 2.0f));
    btnText.setPosition(btnBox.getPosition());
    window.draw(btnText);
}

void UIManager::drawWinLoseScreen(bool isWin) {
    sf::RectangleShape overlay(sf::Vector2f(800.0f, 600.0f));
    overlay.setFillColor(sf::Color(0, 0, 0, 200)); 
    window.draw(overlay);

    sf::Text resultText(mainFont);
    resultText.setString(isWin ? "YOU WIN!" : "GAME OVER");
    resultText.setCharacterSize(70);
    resultText.setFillColor(isWin ? sf::Color::Green : sf::Color::Red);
    resultText.setStyle(sf::Text::Bold);
    
    sf::FloatRect textRect = resultText.getLocalBounds();
    resultText.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f));
    resultText.setPosition(sf::Vector2f(800.0f / 2.0f, 250.0f));
    window.draw(resultText);
}

void UIManager::drawLeaderboard() {}

void UIManager::drawTimer() {
    int minutes = timerSeconds / 60;
    int seconds = timerSeconds % 60;
    
    std::string timeStr = "Time: " + 
                          (minutes < 10 ? std::string("0") : "") + std::to_string(minutes) + ":" + 
                          (seconds < 10 ? std::string("0") : "") + std::to_string(seconds);

    sf::Text timeText(mainFont);
    timeText.setString(timeStr);
    timeText.setCharacterSize(24);
    timeText.setFillColor(sf::Color::Black); 
    timeText.setPosition(sf::Vector2f(650.0f, 20.0f)); 
    window.draw(timeText);
}

void UIManager::drawMistakeCounter() {
    std::string mistakeStr = "Mistakes: " + std::to_string(mistakes) + "/3";

    sf::Text mistakeText(mainFont);
    mistakeText.setString(mistakeStr);
    mistakeText.setCharacterSize(24);
    mistakeText.setFillColor(sf::Color::Red); 
    mistakeText.setPosition(sf::Vector2f(20.0f, 20.0f)); 
    window.draw(mistakeText);
}

void UIManager::drawHintIcon() {
    std::string hintStr = "Hints: " + std::to_string(hintsRemaining);

    sf::Text hintText(mainFont);
    hintText.setString(hintStr);
    hintText.setCharacterSize(24);
    hintText.setFillColor(sf::Color(255, 140, 0)); 
    hintText.setPosition(sf::Vector2f(650.0f, 550.0f)); 
    window.draw(hintText);
}

void UIManager::handleMouseInput() {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        if (mousePos.x >= 175 && mousePos.x < 175 + 450 &&
            mousePos.y >= 75 && mousePos.y < 75 + 450) {
            
            selectedCol = (mousePos.x - 175) / 50;
            selectedRow = (mousePos.y - 75) / 50;
        } else {
            selectedRow = -1;
            selectedCol = -1;
        }
    }
}

void UIManager::handleKeyboardInput(SudokuBoard& board) {
    if (selectedRow == -1 || selectedCol == -1) return;

    int inputVal = -1;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad1)) inputVal = 1;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad2)) inputVal = 2;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad3)) inputVal = 3;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad4)) inputVal = 4;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad5)) inputVal = 5;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num6) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad6)) inputVal = 6;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num7) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad7)) inputVal = 7;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num8) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad8)) inputVal = 8;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num9) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad9)) inputVal = 9;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Delete)) {
        inputVal = 0; 
    }

    if (inputVal != -1) {
        board.setCell(selectedRow, selectedCol, inputVal);
    }
}