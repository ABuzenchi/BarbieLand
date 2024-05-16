#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <irrKlang.h>
#include <string>
#include <glm.hpp>

class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    void playSound(const std::string& soundFile, bool loop = false);
    void play3DSound(const std::string& soundFile, float x, float y, float z, bool loop = false, bool startPaused = false, bool track = true);
    void stopAllSounds();
    void updateListenerPosition(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& upVector, const glm::vec3& velocity);

private:
    irrklang::ISoundEngine* engine;
};

#endif // SOUNDMANAGER_H
