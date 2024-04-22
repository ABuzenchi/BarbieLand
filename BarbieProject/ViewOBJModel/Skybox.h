#pragma once
#include <iostream>
#include <vector>

#include "Shader.h"

class Skybox
{
public:
    void setVAO(unsigned int value);
    void setVBO(unsigned int value);

    void setTexture(unsigned int id);

    unsigned int getVAO();
    unsigned int getVBO();

    void setFaces(std::vector<std::string> faces);
    std::vector<std::string> getFaces();

public:
    //vertices here
    std::vector<std::string> faces;
    unsigned int skyboxVAO;
    unsigned int skyboxVBO;
    unsigned int textureID;
};
