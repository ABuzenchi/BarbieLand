#pragma once
#include <iostream>
#include <vector>

#include <glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



class Texture
{
public:
	Texture(char const* path);
	Texture(std::vector<std::string> faces);

	unsigned int loadTexture(char const* path);
	unsigned int loadCubemapTexture(std::vector<std::string> faces);
public:
	unsigned int ID;
	std::string type;
	std::string path;
};

