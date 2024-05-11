#pragma once

#include <irrKlang.h>
#include <string>

// Forward declaration of irrklang::ISoundEngine to avoid including it in the header file
namespace irrklang {
    class ISoundEngine;
}

class SoundManager {
public:
    SoundManager(); // Constructor
    ~SoundManager(); // Destructor

    void playSound(const std::string& soundFile, bool loop = false);
    void play3DSound(const std::string& soundFile, float x, float y, float z, bool loop = false);
    void stopAllSounds();

private:
    irrklang::ISoundEngine* engine;
};
