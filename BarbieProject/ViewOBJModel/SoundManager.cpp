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

void SoundManager::play3DSound(const std::string& soundFile, float x, float y, float z, bool loop, bool startPaused, bool track) {
    vec3df position(x, y, z);
    engine->play3D(soundFile.c_str(), position, loop, startPaused, track);
}

void SoundManager::stopAllSounds() {
    engine->stopAllSounds();
}
void SoundManager::updateListenerPosition(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& upVector, const glm::vec3& velocity) {
    vec3df irrPosition(position.x, position.y, position.z);
    vec3df irrLookAt(lookAt.x, lookAt.y, lookAt.z);
    vec3df irrUpVector(upVector.x, upVector.y, upVector.z);
    vec3df irrVelocity(velocity.x, velocity.y, velocity.z);
    engine->setListenerPosition(irrPosition, irrLookAt, irrVelocity, irrUpVector);
}
