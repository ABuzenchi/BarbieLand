#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <irrKlang.h>
#include <string>
#include <glm.hpp>

class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    irrklang::ISound* playSound(const std::string& soundFile, bool loop = false);

    irrklang::ISound* play3DSound(const std::string& soundFile, float x, float y, float z, bool loop, bool startPaused, bool track);
    void stopAllSounds();
    void updateListenerPosition(const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& upVector, const glm::vec3& velocity);
    void setVolume(irrklang::ISound* sound, float volume);

private:
    irrklang::ISoundEngine* engine;
};

#endif // SOUNDMANAGER_H
