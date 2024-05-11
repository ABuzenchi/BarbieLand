#include "SoundManager.h"
#include <stdexcept>

using namespace irrklang;

SoundManager::SoundManager() {
    engine = createIrrKlangDevice();
    if (!engine) {
        throw std::runtime_error("Could not create sound engine");
    }
}

SoundManager::~SoundManager() {
    if (engine) {
        engine->drop(); // Release the sound engine
    }
}

void SoundManager::playSound(const std::string& soundFile, bool loop) {
    engine->play2D(soundFile.c_str(), loop);
}

void SoundManager::play3DSound(const std::string& soundFile, float x, float y, float z, bool loop) {
    vec3df position(x, y, z);
    engine->play3D(soundFile.c_str(), position, loop);
}

void SoundManager::stopAllSounds() {
    engine->stopAllSounds();
}
