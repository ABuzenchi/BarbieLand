// ViewOBJModel.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <Windows.h>
#include <locale>
#include <codecvt>

#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h> 

#include <GL/glew.h>
#define GLM_FORCE_CTOR_INIT
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "Shader.h"
#include "Model.h"
#include "Skybox.h"

#include "ECameraMovementType.h"
//#include "Camera.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

std::unique_ptr<Mesh> floorObj;
std::unique_ptr<Model> poolObjModel;
std::unique_ptr<Model> horseObjModel;
std::unique_ptr<Model> treeObjModel;
std::unique_ptr<Model> houseObjModel;
std::unique_ptr<Model> houseMainObjModel;
std::unique_ptr<Model> fenceMainObjModel;
std::unique_ptr<Model> firObjModel;
std::unique_ptr<Model> streetLampObjModel;

GLuint floorTextureId;
GLuint streetLampTextureId;
GLuint ProjMatrixLocation, ViewMatrixLocation, WorldMatrixLocation;
Camera* pCamera = nullptr;



void Cleanup()
{
	delete pCamera;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// timing
double deltaTime = 0.0f;	// time between current frame and last frame
double lastFrame = 0.0f;

//day->night transition
float mixValue = 0.0f;
bool transitioning = false;
bool nightMode = false;
float factorAmbienta = 1.0f; //pentru modificarea intensitatii luminii noapte-zi
float factorDifuzie = 1.0f;
float factorSpecular = 1.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {

	}
}
unsigned int planeVAO = 0;
void renderFloor(unsigned int textureId) {
	unsigned int planeVBO;

	if (planeVAO == 0) {
		float planeVertices[] = {
			// positions            // normals         // texcoords
			200.0f, -0.0f,  200.0f,  0.0f, 1.0f, 0.0f,  200.0f,  0.0f,
			-200.0f, -0.0f,  200.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-200.0f, -0.0f, -200.0f,  0.0f, 1.0f, 0.0f,   0.0f, 200.0f,
			200.0f, -0.0f,  200.0f,  0.0f, 1.0f, 0.0f,  200.0f,  0.0f,
			-200.0f, -0.0f, -200.0f,  0.0f, 1.0f, 0.0f,   0.0f, 200.0f,
			200.0f, -0.0f, -200.0f,  0.0f, 1.0f, 0.0f,  200.0f, 200.0f
		};
		glGenVertexArrays(1, &planeVAO);
		glGenBuffers(1, &planeVBO);
		glBindVertexArray(planeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	}

	glBindVertexArray(planeVAO);
	glActiveTexture(GL_TEXTURE0); // Activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, textureId); // Bind the floor texture only when drawing the floor
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
}
void LoadFloor()
{

	Texture floorTexture("../Textures/ColoredFloor.png");

	floorTextureId = floorTexture.id;

	const float floorSize = 120.0f;
	std::vector<Vertex> floorVertices({
		// positions            // normals           // texcoords
	   { floorSize, 0.0f,  floorSize,  0.0f, 1.0f, 0.0f,    floorSize,  0.0f},
	   {-floorSize, 0.0f,  floorSize,  0.0f, 1.0f, 0.0f,    0.0f,  0.0f},
	   {-floorSize, 0.0f, -floorSize,  0.0f, 1.0f, 0.0f,    0.0f, floorSize},

	   { floorSize, 0.0f,  floorSize,  0.0f, 1.0f, 0.0f,    floorSize,  0.0f},
	   {-floorSize, 0.0f, -floorSize,  0.0f, 1.0f, 0.0f,    0.0f, floorSize},
	   { floorSize, 0.0f, -floorSize,  0.0f, 1.0f, 0.0f,    floorSize, floorSize}
		});



	floorObj = std::make_unique<Mesh>(floorVertices, std::vector<unsigned int>(), std::vector<Texture>{floorTexture});


}

void RenderScene(Shader& shader, bool shadowPass = false) {
	glDisable(GL_CULL_FACE);  // Disabling face culling to avoid missing triangles in shadow rendering

	if (!shadowPass) {
		shader.setFloat("shininess", 32.0f);
		shader.setInt("diffuse", 0);
		shader.setInt("specular", 1);
	}

	// Floor rendering
	floorObj->RenderMesh(shader);
}

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 7", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	glEnable(GL_DEPTH_TEST);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	wchar_t buffer[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, buffer);

	std::wstring executablePath(buffer);
	std::wstring wscurrentPath = executablePath.substr(0, executablePath.find_last_of(L"\\/"));

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string currentPath = converter.to_bytes(wscurrentPath);

	//-----------SKYBOX--SETUP------------------------
	//VBO & VAO
	Skybox skybox;
	skybox.createSkyboxVBO();

	//Texture loading
	std::vector<std::string> dayTex
	{
		"../Textures/skybox/right.jpg",
		"../Textures/skybox/left.jpg",
		"../Textures/skybox/top.jpg",
		"../Textures/skybox/bottom.jpg",
		"../Textures/skybox/front.jpg",
		"../Textures/skybox/back.jpg"
	};
	std::vector<std::string> nightTex
	{
		"../Textures/skybox2/right.jpg",
		"../Textures/skybox2/left.jpg",
		"../Textures/skybox2/top.jpg",
		"../Textures/skybox2/bottom.jpg",
		"../Textures/skybox2/front.jpg",
		"../Textures/skybox2/back.jpg"
	};
	skybox.loadSkyboxTextures(dayTex, nightTex);

	//Shader config
	skybox.shaderConfiguration(currentPath);
	//--------------------------------------------------------------

	// Create camera
	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 1.0, 3.0));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shadowMappingShader((currentPath + "\\Shaders\\ShadowMapping.vs").c_str(), (currentPath + "\\Shaders\\ShadowMapping.fs").c_str());
	Shader shadowMappingDepthShader((currentPath + "\\Shaders\\ShadowMappingDepth.vs").c_str(), (currentPath + "\\Shaders\\ShadowMappingDepth.fs").c_str());

	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	shadowMappingShader.use();
	shadowMappingShader.setInt("diffuseTexture", 0);
	shadowMappingShader.setInt("shadowMap", 1);

	glEnable(GL_CULL_FACE);

	LoadFloor();


	glm::vec3 lightPos(0.0f, 2.0f, 1.0f);
	float hue = 1.0;
	float floorHue = 0.9;
	// load textures
	// -------------

	std::string PoolFileName = (currentPath + "\\Models\\objects\\pool\\pool.obj");
	Model poolObjModel(PoolFileName, false);

	std::string horseObjFileName = (currentPath + "\\Models\\Animals\\Horse.obj");
	Model horseObjModel(horseObjFileName, false);

	std::string treeFileName = (currentPath + "\\Models\\plants\\tree\\tree.obj");
	Model treeObjModel(treeFileName, false);

	std::string houseFileName = (currentPath + "\\Models\\objects\\House.obj");
	Model houseObjModel(houseFileName, false);

	std::string houseMainFileName = (currentPath + "\\Models\\objects\\MainHouse\\mainHouse.obj");
	Model houseMainObjModel(houseMainFileName, false);

	std::string fenceFileName = (currentPath + "\\Models\\objects\\fence\\fence.obj");
	Model fenceMainObjModel(fenceFileName, false);

	std::string firFileName = (currentPath + "\\Models\\plants\\fir.obj");
	Model firObjModel(firFileName, false);

	std::string streetLampFileName = (currentPath + "\\Models\\objects\\streetLamp.obj");
	Texture streetLampTexture("../Models/Objects/StreetLamp.jpg");
	Model streetLampObjModel(streetLampFileName, false);


	// render loop
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view;
		glm::mat4 projection = glm::perspective(glm::radians(pCamera->FoVy), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		//Day-Night transitions (to refactor)
		if (transitioning) {
			const float transitionSpeed = 0.3f; // Transition speed factor
			if (nightMode) {
				mixValue += deltaTime * transitionSpeed;
				factorAmbienta -= deltaTime * transitionSpeed;
				if (factorAmbienta <= 0.0f)
					factorAmbienta = 0.0f;
				factorDifuzie -= deltaTime * transitionSpeed;
				if (factorDifuzie <= 0.2f)
					factorDifuzie = 0.2f;
				factorSpecular -= deltaTime * transitionSpeed;
				if (factorSpecular <= 0.2f)
					factorSpecular = 0.2f;
				if (mixValue >= 1.0f) {
					mixValue = 1.0f;
					transitioning = false;  // Stop transitioning
				}
			}
			else {
				mixValue -= deltaTime * transitionSpeed;
				factorAmbienta += deltaTime * transitionSpeed;
				if (factorAmbienta >= 1.0f)
					factorAmbienta = 1.0f;
				factorDifuzie += deltaTime * transitionSpeed;
				if (factorDifuzie >= 1.0f)
					factorDifuzie = 1.0f;
				factorSpecular += deltaTime * transitionSpeed;
				if (factorSpecular >= 1.0f)
					factorSpecular = 1.0f;
				
				if (mixValue <= 0.0f) {
					mixValue = 0.0f;
					transitioning = false;  // Stop transitioning
				}
			}
		}

		//Render skybox
		skybox.renderSkybox(mixValue, pCamera, SCR_WIDTH, SCR_HEIGHT);

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 5.0f, far_plane = 50.f;
		lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

		//shadowMappingDepthShader.use();
		//shadowMappingDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		//glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		//glClear(GL_DEPTH_BUFFER_BIT);
		//RenderScene(shadowMappingDepthShader, true);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//// Reset viewport
		//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render scene with shadows

		shadowMappingShader.use();
		shadowMappingShader.setMat4("projection", pCamera->GetProjectionMatrix());
		shadowMappingShader.setMat4("view", pCamera->GetViewMatrix());
		shadowMappingShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shadowMappingShader.SetVec3("lightPos", lightPos);
		shadowMappingShader.SetVec3("viewPos", pCamera->GetPosition());

		shadowMappingShader.setFloat("factorAmbienta", factorAmbienta);
		shadowMappingShader.setFloat("factorDifuzie", factorDifuzie);
		shadowMappingShader.setFloat("factorSpecular", factorSpecular);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTextureId);  // Bind floor texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);  // Bind shadow map
		RenderScene(shadowMappingShader, false);


		//NATURE
		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		glm::mat4 treeModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel = glm::translate(treeModel, glm::vec3(-3.0, 0.0, 0.0));
		shadowMappingShader.setMat4("model", treeModel);
		treeObjModel.RenderModel(shadowMappingShader, treeModel);
		treeObjModel.RenderModel(shadowMappingDepthShader, treeModel);


		glm::mat4 treeModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel2 = glm::translate(treeModel, glm::vec3(-6.0, 0.0, 1.0));
		shadowMappingShader.setMat4("model", treeModel2);
		treeObjModel.RenderModel(shadowMappingShader, treeModel2);
		treeObjModel.RenderModel(shadowMappingDepthShader, treeModel2);

		//ANIMALS
		float horseSpeed = 2.0f; // Viteza de deplasare a calului
		float horseAmplitude = 5.0f; // Amplitudinea mișcării înainte și înapoi
		glm::vec3 horsePositionOffset = glm::vec3(20.0f, 0.0f, 64.0f); // Offsetul poziției calului
		glm::mat4 horseModel1 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));

		//Calculăm poziția calului pe axa X și Z folosind funcția sinusoidală
		float horseX = horseAmplitude * sin(glfwGetTime() * horseSpeed); // Mișcare pe axa X
		float horseZ = horseAmplitude * cos(glfwGetTime() * horseSpeed); // Mișcare pe axa Z
		horsePositionOffset.x += horseX; // Actualizăm poziția pe axa X
		horsePositionOffset.z += horseZ; // Actualizăm poziția pe axa Z

		horseModel1 = glm::translate(glm::mat4(1.0f), horsePositionOffset); // Aplicăm noul offset de poziție
		shadowMappingShader.setMat4("model", horseModel1);
		horseObjModel.RenderModel(shadowMappingShader, horseModel1);
		horseObjModel.RenderModel(shadowMappingDepthShader, horseModel1);

		//ANIMALS
		float horseSpeedZ = 2.0f; // Viteza de deplasare a calului pe axa Z
		float horseAmplitudeZ = 2.0f; // Amplitudinea mișcării pe axa Z
		glm::vec3 horsePositionOffset2 = glm::vec3(25.0f, 0.0f, 60.0f); // Offsetul poziției calului
		glm::mat4 horseModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.0f));

		//Calculăm poziția calului pe axa Z folosind funcția sinusoidală
		float horseZ2 = horseAmplitudeZ * sin(glfwGetTime() * horseSpeedZ); // Mișcare pe axa Z
		horsePositionOffset2.z += horseZ; // Actualizăm poziția pe axa Z

		horseModel2 = glm::translate(glm::mat4(1.0f), horsePositionOffset2); // Aplicăm noul offset de poziție
		shadowMappingShader.setMat4("model", horseModel2);
		horseObjModel.RenderModel(shadowMappingShader, horseModel2);
		horseObjModel.RenderModel(shadowMappingDepthShader, horseModel2);


		//OBJECTS
		shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
		glm::mat4 poolModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		poolModel = glm::translate(poolModel, glm::vec3(15.0, 0.0, 0.0));
		shadowMappingShader.setMat4("model", poolModel);
		poolObjModel.RenderModel(shadowMappingShader, poolModel);
		poolObjModel.RenderModel(shadowMappingDepthShader, poolModel);

		

		//HOUSES
		shadowMappingShader.SetVec3("color", 1.0f, 0.3f, 0.20f);
		glm::mat4 houseModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		houseModel = glm::translate(houseModel, glm::vec3(-15.0, 0.0, 0.0));
		shadowMappingShader.setMat4("model", houseModel);
		houseObjModel.RenderModel(shadowMappingShader, houseModel);
		houseObjModel.RenderModel(shadowMappingDepthShader, houseModel);

		shadowMappingShader.SetVec3("color", 0.8f, 0.40f, 0.40f);
		glm::mat4 houseMainModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		houseMainModel = glm::translate(houseMainModel, glm::vec3(20.0, 0.0, 12.0));
		houseMainModel = glm::rotate(houseMainModel, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMappingShader.setMat4("model", houseMainModel);
		houseMainObjModel.RenderModel(shadowMappingShader, houseMainModel);
		houseMainObjModel.RenderModel(shadowMappingDepthShader, houseMainModel);

		//fence

		glm::vec3 initialFenceTranslationLine0(0.0f, 0.5f, 50.0f);
		glm::vec3 initialFenceTranslationLine15(0.0f, 0.5f, 70.0f);
		glm::vec3 initialFenceTranslationLeft(-1.0f, 0.5f, 50.0f);
		glm::vec3 initialFenceTranslationRight(32.0f, 0.0f, 0.0f);
		for (int coloana = 0; coloana < 16; coloana++)
		{

			shadowMappingShader.SetVec3("color", 0.30f, 0.40f, 0.40f);
			glm::mat4 fenceModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
			fenceModel = glm::translate(fenceModel, initialFenceTranslationLine0);
			fenceModel = glm::rotate(fenceModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			fenceModel = glm::scale(fenceModel, glm::vec3(0.01, 0.01, 0.01));
			shadowMappingShader.setMat4("model", fenceModel);
			fenceMainObjModel.RenderModel(shadowMappingShader, fenceModel);
			fenceMainObjModel.RenderModel(shadowMappingDepthShader, fenceModel);

			shadowMappingShader.SetVec3("color", 0.30f, 0.40f, 0.40f);
			glm::mat4 fenceModelLine15 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
			fenceModelLine15 = glm::translate(fenceModelLine15, initialFenceTranslationLine15);
			fenceModelLine15 = glm::rotate(fenceModelLine15, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			fenceModelLine15 = glm::scale(fenceModelLine15, glm::vec3(0.01, 0.01, 0.01));
			shadowMappingShader.setMat4("model", fenceModelLine15);
			fenceMainObjModel.RenderModel(shadowMappingShader, fenceModelLine15);
			fenceMainObjModel.RenderModel(shadowMappingDepthShader, fenceModelLine15);

			initialFenceTranslationLine0.x += 2.0f;
			initialFenceTranslationLine15.x += 2.0f;
		}


		for (int rand = 0; rand < 6; rand++)
		{
			glm::mat4 firModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
			firModel = glm::translate(firModel, initialFenceTranslationLeft);
			shadowMappingShader.setMat4("model", firModel);
			firObjModel.RenderModel(shadowMappingShader, firModel);
			firObjModel.RenderModel(shadowMappingDepthShader, firModel);
			

			glm::mat4 firModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
			firModel2 = glm::translate(firModel, initialFenceTranslationRight);
			shadowMappingShader.setMat4("model", firModel2);
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			firObjModel.Draw(shadowMappingShader);

			initialFenceTranslationLeft.z += 3.5f;
			initialFenceTranslationRight.z += 0.7f;
		}

		//streetLamp
		glActiveTexture(GL_TEXTURE0); // Activate the texture unit 0
		glBindTexture(GL_TEXTURE_2D, streetLampTexture.id); // Bind the texture
		shadowMappingShader.use(); // Activate the shader
		shadowMappingShader.setInt("diffuseTexture", 0); // Bind street lamp texture
		//shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 1.0f);
		glm::mat4 streetLampModel = glm::scale(glm::mat4(0.5), glm::vec3(0.5f));
		streetLampModel = glm::translate(streetLampModel, glm::vec3(2.0, -7.0, 0.0));
		shadowMappingShader.setMat4("model", streetLampModel);
		//streetLampObjModel.Draw(shadowMappingShader);
		streetLampObjModel.RenderModel(shadowMappingShader, streetLampModel);
		streetLampObjModel.RenderModel(shadowMappingDepthShader, streetLampModel);


		//Floor
		//shadowMappingShader.SetVec3("color", 1.0f, 0.0f, 0.6f);

		//RenderScene(shadowMappingShader);
		glBindVertexArray(lightVAO);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Cleanup();

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(ECameraMovementType::FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(ECameraMovementType::BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(ECameraMovementType::LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(ECameraMovementType::RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(ECameraMovementType::UP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(ECameraMovementType::DOWN, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
		transitioning = true;
		nightMode = !nightMode;  // Toggle mode
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);

	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
	pCamera->ProcessMouseScroll((float)yOffset);
}
