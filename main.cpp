#include "GameEngine.h"
#include <iostream>

sf::RenderWindow window(sf::VideoMode({800u, 600u}), "Sudoku");
sf::Font mainFont;

int main() {
    if (!mainFont.openFromFile("../assets/arial.ttf")) {
        std::cerr << "[Warning] Unable to load a font file for the Sudoku UI.\n";
    }

    GameEngine a;   
    a.run();
    return 0;
}