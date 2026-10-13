#include "GameEngine.h"

void SudokuCell::draw(sf::RenderWindow& window, const sf::Font& font, int selectedRow, int selectedCol) {
    sf::RectangleShape cellBox(sf::Vector2f(50.0f, 50.0f)); 
    float x = col * 50.0f + 175.0f; // Offset X = 175[cite: 7]
    float y = row * 50.0f + 75.0f;  // Offset Y = 75[cite: 7]
    
    cellBox.setPosition(sf::Vector2f(x, y));
    
    // Highlight ô đang được chọn
    if (row == selectedRow && col == selectedCol) {
        cellBox.setFillColor(sf::Color(210, 230, 255)); // Xanh nhạt highlight[cite: 7]
    } else {
        cellBox.setFillColor(sf::Color::White); 
    }
    
    cellBox.setOutlineThickness(1.0f);
    cellBox.setOutlineColor(sf::Color(180, 180, 180));
    window.draw(cellBox);

    if (value != 0) {
        sf::Text text(font, std::to_string(value), 28); 
        
        // --- LOGIC ĐỔI MÀU Ở ĐÂY ---
        if (isFixed) {
            text.setFillColor(sf::Color::Black); // Số đề bài cho[cite: 7]
        } else if (isWrong) {
            text.setFillColor(sf::Color::Red);   // NẾU SAI -> SỐ MÀU ĐỎ
        } else {
            text.setFillColor(sf::Color(30, 100, 220)); // Số người chơi điền (Xanh dương)[cite: 7]
        }

        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f));
        text.setPosition(sf::Vector2f(x + 25.0f, y + 25.0f));

        window.draw(text);
    }
}

void SudokuBoard::draw(sf::RenderWindow& window, const sf::Font& font, int selectedRow, int selectedCol) {
    // 1. Vẽ các ô vuông và chữ số
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            grid[i][j].draw(window, font, selectedRow, selectedCol);
        }
    }

    // 2. Vẽ viền đậm chia các khối 3x3
    float startX = 175.0f;
    float startY = 75.0f;
    float cellSize = 50.0f;
    float boardSize = cellSize * 9.0f;

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

void SudokuBoard::update() {
    // Để trống hoặc gọi update của từng ô
}

void SudokuCell::update() {
    // Với một ô Sudoku cơ bản, thường không có logic update liên tục mỗi frame.
    // Hàm này để trống hoặc dùng để xử lý hiệu ứng nhấp nháy (nếu có sau này).
}