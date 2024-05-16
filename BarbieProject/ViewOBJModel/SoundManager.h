#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <GLM.hpp>
#include <irrKlang.h>
#include <string>


class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    void playSound(const std::string& soundFile, bool loop = false);
    void play3DSound(const std::string& soundFile, float x, float y, float z, bool loop = false);
    void stopAllSounds();
    void updateListenerPosition(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& upVector);

private:
    irrklang::ISoundEngine* engine;
};

#endif // SOUNDMANAGER_H
