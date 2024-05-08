#pragma once
#include <iostream>
#include <vector>

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

class Skybox
{
public:
    Skybox() {};
    void setVAO(unsigned int value);
    void setVBO(unsigned int value);

    void setDayTexture(unsigned int id);
    void setNightTexture(unsigned int id);

    unsigned int getVAO();
    unsigned int getVBO();
    Shader getShader();

    void setFaces(std::vector<std::string> faces);
    std::vector<std::string> getFaces();

    void createSkyboxVBO();
    void loadSkyboxTextures(std::vector<std::string> dayTex, std::vector<std::string> nightTex);

    void shaderConfiguration(std::string currentPath);

    void renderSkybox(float mixValue, Camera* pCamera, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT);

public:
    //vertices here
    std::vector<std::string> faces;
    Shader skyboxShader;
    unsigned int skyboxVAO;
    unsigned int skyboxVBO;
    unsigned int dayTextureID;
    unsigned int nightTextureID;
};
