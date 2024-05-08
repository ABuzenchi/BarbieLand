#include "Skybox.h"

void Skybox::setVAO(unsigned int value)
{
    skyboxVAO = value;
}

void Skybox::setVBO(unsigned int value)
{
    skyboxVBO = value;
}

void Skybox::setDayTexture(unsigned int id)
{
    dayTextureID = id;
}

void Skybox::setNightTexture(unsigned int id)
{
	nightTextureID = id;
}

unsigned int Skybox::getVAO()
{
    return skyboxVAO;
}

unsigned int Skybox::getVBO()
{
    return skyboxVBO;
}

Shader Skybox::getShader()
{
	return skyboxShader;
}

void Skybox::setFaces(std::vector<std::string> faces)
{
    this->faces = faces;
}

std::vector<std::string> Skybox::getFaces()
{
    return faces;
}

void Skybox::createSkyboxVBO()
{
	float skyboxVertices[] =
	{
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	setVAO(skyboxVAO);
	setVBO(skyboxVBO);
}

void Skybox::loadSkyboxTextures(std::vector<std::string> dayTex, std::vector<std::string> nightTex)
{
	Texture dayCubemap(dayTex);
	Texture nightCubemap(nightTex);
	setFaces(dayTex);
	setDayTexture(dayCubemap.id);
	setNightTexture(nightCubemap.id);
}

void Skybox::shaderConfiguration(std::string currentPath)
{
	Shader shader((currentPath + "\\Shaders\\skybox.vs").c_str(), (currentPath + "\\Shaders\\skybox.fs").c_str());
	shader.use();
	shader.setInt("skyboxDay", 0);
	shader.setInt("skyboxNight", 1);
	skyboxShader = shader;
}

void Skybox::renderSkybox(float mixValue, Camera* pCamera, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT)
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glm::mat4 view = glm::mat4(glm::mat3(pCamera->GetViewMatrix()));
	glm::mat4 projection = glm::perspective(glm::radians(pCamera->FoVy), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	skyboxShader.use();
	skyboxShader.setFloat("mixValue", mixValue);
	skyboxShader.setMat4("view", view);
	skyboxShader.setMat4("projection", projection);

	glBindVertexArray(getVAO());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, dayTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, nightTextureID);
	glActiveTexture(GL_TEXTURE1);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

