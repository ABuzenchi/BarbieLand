#pragma once
#include <iostream>
#include <vector>

#include <GL/glew.h>

#include <stb_image.h>


class Texture
{
public:
	Texture() {}
	Texture(char const* path);
	Texture(std::vector<std::string> faces);

	unsigned int loadTexture(char const* path);
	unsigned int loadCubemapTexture(std::vector<std::string> faces);
public:
	unsigned int id;
	std::string type;
	std::string path;
};
