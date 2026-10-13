#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <iostream>

class SoundManager {
private:
    sf::Music bgMusic;
    sf::SoundBuffer clickBuffer;
    sf::Sound clickSound;
    sf::Music winMusic;
    sf::Music looseMusic;

    // 1. Đưa hàm khởi tạo vào private để không ai được tạo biến SoundManager lung tung
    SoundManager(); 

public:
    // 2. Thêm hàm này: Tạo ra một "Trạm phát thanh" duy nhất cho toàn bộ game
    static SoundManager& getInstance() {
        static SoundManager instance;
        return instance;
    }

    // Không cho phép copy
    SoundManager(SoundManager const&) = delete;
    void operator=(SoundManager const&) = delete;

    ~SoundManager() = default;

    void playBGM(const std::string& filepath);
    void stopBGM();
    void playWM(const std::string& filepath);
    void playLM(const std::string& filepath);
    void playClickSound();

    void setVolume(float volume);
};