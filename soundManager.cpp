#include "SoundManager.h"

SoundManager::SoundManager() : clickSound(clickBuffer) {
    // Tải file âm thanh vào bộ nhớ
    if (!clickBuffer.loadFromFile("../assets/click.wav")) {
        std::cerr << "[Warning] Khong the tai file am thanh hieu ung click!\n";
    } else {
        // Cập nhật lại dữ liệu cho Sound sau khi tải file thành công
        clickSound.setBuffer(clickBuffer);
    }
}

void SoundManager::setVolume(float volume) {
    // SFML nhận giá trị volume từ 0.f đến 100.f
    bgMusic.setVolume(volume);
    clickSound.setVolume(volume);
}

void SoundManager::playBGM(const std::string& filepath) {
    if (!bgMusic.openFromFile(filepath)) {
        std::cerr << "[Warning] Khong the tai nhac nen: " << filepath << "\n";
        return;
    }
    bgMusic.setLooping(true); // Lặp lại nhạc nền
    bgMusic.play();
}

void SoundManager::playWM(const std::string& filepath) {
    if (!winMusic.openFromFile(filepath)) {
        std::cerr << "[Warning] Khong the tai nhac win: " << filepath << "\n";
        return;
    }
    winMusic.play();
}

void SoundManager::playLM(const std::string& filepath) {
    if (!looseMusic.openFromFile(filepath)) {
        std::cerr << "[Warning] Khong the tai nhac loose: " << filepath << "\n";
        return;
    }
    looseMusic.play();
}

void SoundManager::stopBGM() {
    bgMusic.stop();
}

void SoundManager::playClickSound() {
    // Phát tiếng động (có thể chèn đè lên nhau nếu bấm nhanh)
    clickSound.play();
}