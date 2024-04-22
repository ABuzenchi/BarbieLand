#include "Skybox.h"

void Skybox::setVAO(unsigned int value)
{
	skyboxVAO = value;
}

void Skybox::setVBO(unsigned int value)
{
	skyboxVBO = value;
}

void Skybox::setTexture(unsigned int id)
{
	textureID = id;
}

unsigned int Skybox::getVAO()
{
	return skyboxVAO;
}

unsigned int Skybox::getVBO()
{
	return skyboxVBO;
}

void Skybox::setFaces(std::vector<std::string> faces)
{
	this->faces = faces;
}

std::vector<std::string> Skybox::getFaces()
{
	return faces;
}


