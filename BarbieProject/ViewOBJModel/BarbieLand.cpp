// ViewOBJModel.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <Windows.h>
#include <locale>
#include <codecvt>

#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h> 
#include "SoundManager.h"
#include <GL/glew.h>
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
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
float M_PI = 3.14159265358979323846f;
float rotationInterval = 0.1f;
float degrees = 356.f;
float rotationAngle = glm::radians(degrees);
float timeSinceLastRotation = 0.0f;
float carPathAngle = 0.0f;
const float lapTime = 10.0f;
const float angularSpeedPath = (2.0f * M_PI) / lapTime;
const float carRadius = 8.1f;
const glm::vec3 circleCenter = glm::vec3(-2.0f, 0.0f, -4.0f);
const float carSpeed = 0.5f;
float carAngle = 0.5f;
float totalTime = 0.0f;
float resetInterval = 10.0f;
const int numSpheres = 10;
const float radius = 10.0f;
const float Speed = 2.0f;
const float Amplitude = 3.0f;
std::unique_ptr<Mesh> floorObj;
std::unique_ptr<Model> poolObjModel;
std::unique_ptr<Model> sphereObjModel;
std::unique_ptr<Model> horseObjModel;
std::unique_ptr<Model> treeObjModel;
std::unique_ptr<Model> palmTreeObjModel;
std::unique_ptr<Model> houseObjModel;
std::unique_ptr<Model> houseMainObjModel;
std::unique_ptr<Model> womenObjModel;
std::unique_ptr<Model> women2ObjModel;
std::unique_ptr<Model> fenceMainObjModel;
std::unique_ptr<Model> firObjModel;
std::unique_ptr<Model> streetLampObjModel;
std::unique_ptr<Model> groundObj;
std::unique_ptr<Model>catObjModel;
std::unique_ptr<Model> signObjModel;
std::unique_ptr<Model> carObjModel;
std::unique_ptr<Model> landscapeObjModel;
std::unique_ptr<Model> benchObjModel;
std::unique_ptr<Model> musicObjModel;
float startX = -50.0f;
float startY = 0.0f;
float startZ = 2.0f;

float degreesY = 0.0f;
float degreesZ = 0.0f;

bool start = false;


enum class CameraType
{
	Free,
	SceneUp,
	EnterTown
};

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
void LoadScene()
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
	wchar_t buffer[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, buffer);

	std::wstring executablePath(buffer);
	std::wstring wscurrentPath = executablePath.substr(0, executablePath.find_last_of(L"\\/"));

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string currentPath = converter.to_bytes(wscurrentPath);

	std::string PoolFileName = (currentPath + "\\Models\\Object\\fountain\\MarbleFountain1.obj");
	poolObjModel = std::make_unique<Model>(PoolFileName, false);

	std::string horseObjFileName = (currentPath + "\\Models\\Animals\\Horse.obj");
	horseObjModel = std::make_unique<Model>(horseObjFileName, false);

	std::string treeFileName = (currentPath + "\\Models\\plants\\tree\\tree.obj");
	treeObjModel = std::make_unique<Model>(treeFileName, false);

	std::string palmtreeFileName = (currentPath + "\\Models\\plants\\palmTree\\palmTree.obj");
	palmTreeObjModel = std::make_unique<Model>(palmtreeFileName, false);

	std::string sphereFileName = (currentPath + "\\Models\\Object\\waterPearl\\webtrcc.obj");
	sphereObjModel = std::make_unique<Model>(sphereFileName, false);

	std::string houseMainFileName = (currentPath + "\\Models\\Object\\MainHouse\\mainHouse.obj");
	houseMainObjModel = std::make_unique<Model>(houseMainFileName, false);

	std::string womenMainFileName = (currentPath + "\\Models\\Object\\women\\women.obj");
	womenObjModel = std::make_unique<Model>(womenMainFileName, false);

	std::string houseFileName = (currentPath + "\\Models\\Object\\house\\export.obj");
	houseObjModel = std::make_unique<Model>(houseFileName, false);

	std::string women2MainFileName = (currentPath + "\\Models\\Object\\women2\\Girl With Black & Pink Dress.obj");
	women2ObjModel = std::make_unique<Model>(women2MainFileName, false);

	std::string fenceFileName = (currentPath + "\\Models\\Object\\fence\\fence.obj");
	fenceMainObjModel = std::make_unique<Model>(fenceFileName, false);

	std::string firFileName = (currentPath + "\\Models\\plants\\pineTree\\Tree2.obj");
	firObjModel = std::make_unique<Model>(firFileName, false);

	std::string streetLampFileName = (currentPath + "\\Models\\Object\\streetLamp\\streetLamp.obj");
	Texture streetLampTexture("../Models/Object/streetLamp/StreetLamp.jpg");
	streetLampObjModel = std::make_unique<Model>(streetLampFileName, false);
	streetLampTextureId = streetLampTexture.id;

	std::string groundFileName = (currentPath + "\\Models\\objects\\ground\\BarbieLandFloor.obj");
	groundObj = std::make_unique<Model>(groundFileName, false);

	std::string catObjFileName = (currentPath + "\\Models\\Animals\\Cat\\cat.obj");
	catObjModel = std::make_unique<Model>(catObjFileName, false);

	std::string benchFilename = (currentPath + "\\Models\\Object\\bench\\Bench_HighRes.obj");
	benchObjModel = std::make_unique<Model>(benchFilename, false);

	std::string signObjFilename = (currentPath + "\\Models\\Object\\sign\\BarbieLandsign.obj");
	signObjModel = std::make_unique<Model>(signObjFilename, false);

	std::string carObjFilename = (currentPath + "\\Models\\Object\\car\\sportsCar.obj");
	carObjModel = std::make_unique<Model>(carObjFilename, false);

	std::string landscapeFilename = (currentPath + "\\Models\\objects\\landscape\\landscape.obj");
	landscapeObjModel = std::make_unique<Model>(landscapeFilename, false);

	std::string musicObjFilename = (currentPath + "\\Models\\Object\\music\\3d-model.obj");
	musicObjModel = std::make_unique<Model>(musicObjFilename, false);
}

float horseSpeed = 1.0f; // Horse movement speed
float horseAmplitude = 5.0f; // Amplitude of movement forward and backward

float horseX = 0.0f; // Initial movement on the X axis
float horseY = 0.0f; // Initial movement on the Y axis


void RenderScene(Shader& shader, bool shadowPass = false) {
	glDisable(GL_CULL_FACE);  // Disabling face culling to avoid missing triangles in shadow rendering

	if (!shadowPass) {
		shader.setFloat("shininess", 32.0f);
		shader.setInt("diffuse", 0);
		shader.setInt("specular", 1);
	}

	// Floor rendering
	//floorObj->RenderMesh(shader);
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


	SoundManager soundManager;
	try {
		soundManager.playSound("../Audio/barbie.mp3", true);  // Replace with the actual path and set loop to true or false as needed
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		glfwTerminate();
		return -1;
	}

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

	LoadScene();


	glm::vec3 lightPos(0.0f, 2.0f, 1.0f);
	float hue = 1.0;
	float floorHue = 0.9;



	// render loop
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		timeSinceLastRotation += deltaTime;
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


		//sign
		glm::mat4 signModel = glm::mat4(1.0);
		signModel = glm::translate(signModel, glm::vec3(-50.0, 0.0, -4.0));
		signModel = glm::rotate(signModel, glm::radians(270.0f), glm::vec3(0, 1, 0));
		signModel = glm::scale(signModel, glm::vec3(1.2f));
		shadowMappingShader.setMat4("model", signModel);
		signObjModel->RenderModel(shadowMappingShader, signModel);
		signObjModel->RenderModel(shadowMappingDepthShader, signModel);

		//ground
		glm::mat4 groundModel = glm::scale(glm::mat4(1.0), glm::vec3(3.f));
		groundModel = glm::translate(groundModel, glm::vec3(0.0, 1.2, 0.0));
		shadowMappingShader.setMat4("model", groundModel);
		groundObj->RenderModel(shadowMappingShader, groundModel);
		groundObj->RenderModel(shadowMappingDepthShader, groundModel);

		//landscape
		glm::mat4 landscapeModel = glm::scale(glm::mat4(1.0), glm::vec3(10.f));
		landscapeModel = glm::translate(landscapeModel, glm::vec3(0.0, 0.0, 0.0));
		shadowMappingShader.setMat4("model", landscapeModel);
		landscapeObjModel->RenderModel(shadowMappingShader, landscapeModel);
		landscapeObjModel->RenderModel(shadowMappingDepthShader, landscapeModel);

#pragma region Trees



		glm::mat4 treeModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel = glm::translate(treeModel, glm::vec3(-10.0, 0.0, 10.0));
		shadowMappingShader.setMat4("model", treeModel);
		treeObjModel->RenderModel(shadowMappingShader, treeModel);
		treeObjModel->RenderModel(shadowMappingDepthShader, treeModel);


		// Number of trees
		const float radius = 10.0f;  // Radius of the circle
		const float circleRadius = 25.0f;  // The radius of the circle on which trees will be placed
		const float circleRadius2 = 8.0f;// The radius of the circle on which trees will be placed
		const float circleRadius3 = 40.0f;

		for (int i = 0; i < 10; ++i) {
			float angle = i * (2 * M_PI / numSpheres); // Calculate the angle for each tree

			// Big circle of trees
			float x1 = -45.0f + circleRadius * cos(angle); // Calculate the x position
			float z1 = -29.5f + circleRadius * sin(angle); // Calculate the z position
			glm::mat4 sphereModel1 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)); // Include scaling if needed
			sphereModel1 = glm::translate(sphereModel1, glm::vec3(x1, 0.0, z1));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel1);
			treeObjModel->RenderModel(shadowMappingShader, sphereModel1);
			treeObjModel->RenderModel(shadowMappingDepthShader, sphereModel1);

			// Big circle of trees
			x1 = 0.0f + circleRadius3 * cos(angle); // Calculate the x position
			z1 = 0.0f + circleRadius3 * sin(angle); // Calculate the z position
			sphereModel1 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)); // Include scaling if needed
			sphereModel1 = glm::translate(sphereModel1, glm::vec3(x1, 0.0, z1));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel1);
			treeObjModel->RenderModel(shadowMappingShader, sphereModel1);
			treeObjModel->RenderModel(shadowMappingDepthShader, sphereModel1);

			// Big circle of trees
			x1 = -45.0f + circleRadius * cos(angle); // Calculate the x position
			z1 = 15.5f + circleRadius * sin(angle); // Calculate the z position
			sphereModel1 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)); // Include scaling if needed
			sphereModel1 = glm::translate(sphereModel1, glm::vec3(x1, 0.0, z1));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel1);
			treeObjModel->RenderModel(shadowMappingShader, sphereModel1);
			treeObjModel->RenderModel(shadowMappingDepthShader, sphereModel1);

			// Small circle of trees
			float x2 = -45.0f + circleRadius2 * cos(angle); // Calculate the x position
			float z2 = -29.5f + circleRadius2 * sin(angle); // Calculate the z position
			glm::mat4 sphereModel2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)); // Include scaling if needed
			sphereModel2 = glm::translate(sphereModel2, glm::vec3(x2, 0.0, z2));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel2);
			treeObjModel->RenderModel(shadowMappingShader, sphereModel2);
			treeObjModel->RenderModel(shadowMappingDepthShader, sphereModel2);

			// Small circle of trees
			x2 = -45.0f + circleRadius2 * cos(angle); // Calculate the x position
			z2 = 15.5f + circleRadius2 * sin(angle); // Calculate the z position
			sphereModel2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)); // Include scaling if needed
			sphereModel2 = glm::translate(sphereModel2, glm::vec3(x2, 0.0, z2));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel2);
			treeObjModel->RenderModel(shadowMappingShader, sphereModel2);
			treeObjModel->RenderModel(shadowMappingDepthShader, sphereModel2);
		}


		// Static trees outside the loop
		glm::mat4 treeModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel2 = glm::translate(treeModel2, glm::vec3(-15.0, 0.0, 1.0));
		shadowMappingShader.setMat4("model", treeModel2);
		treeObjModel->RenderModel(shadowMappingShader, treeModel2);
		treeObjModel->RenderModel(shadowMappingDepthShader, treeModel2);

		treeModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel2 = glm::translate(treeModel2, glm::vec3(-15.0, 0.0, -10.0));
		shadowMappingShader.setMat4("model", treeModel2);
		treeObjModel->RenderModel(shadowMappingShader, treeModel2);
		treeObjModel->RenderModel(shadowMappingDepthShader, treeModel2);

		treeModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel2 = glm::translate(treeModel2, glm::vec3(0.0, 0.0, -35.0));
		shadowMappingShader.setMat4("model", treeModel2);
		treeObjModel->RenderModel(shadowMappingDepthShader, treeModel2);
		treeObjModel->RenderModel(shadowMappingShader, treeModel2);

		treeModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel2 = glm::translate(treeModel2, glm::vec3(14.0, 0.0, -35.0));
		shadowMappingShader.setMat4("model", treeModel2);
		treeObjModel->RenderModel(shadowMappingDepthShader, treeModel2);
		treeObjModel->RenderModel(shadowMappingShader, treeModel2);

		treeModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel2 = glm::translate(treeModel2, glm::vec3(30.0, 0.0, -30.0));
		shadowMappingShader.setMat4("model", treeModel2);
		treeObjModel->RenderModel(shadowMappingDepthShader, treeModel2);
		treeObjModel->RenderModel(shadowMappingShader, treeModel2);

		treeModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel2 = glm::translate(treeModel2, glm::vec3(30.0, 0.0, 0.0));
		shadowMappingShader.setMat4("model", treeModel2);
		treeObjModel->RenderModel(shadowMappingDepthShader, treeModel2);
		treeObjModel->RenderModel(shadowMappingShader, treeModel2);

		treeModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel2 = glm::translate(treeModel2, glm::vec3(35.0, 0.0, 10.0));
		shadowMappingShader.setMat4("model", treeModel2);
		treeObjModel->RenderModel(shadowMappingDepthShader, treeModel2);
		treeObjModel->RenderModel(shadowMappingShader, treeModel2);

		treeModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel2 = glm::translate(treeModel2, glm::vec3(25.0, 0.0, 20.0));
		shadowMappingShader.setMat4("model", treeModel2);
		treeObjModel->RenderModel(shadowMappingDepthShader, treeModel2);
		treeObjModel->RenderModel(shadowMappingShader, treeModel2);
#pragma endregion




		////ANIMALS

		glm::mat4 horseModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.0f));
		horseModel2 = glm::translate(horseModel2, glm::vec3(28.0f, 0.0f, 35.0f));
		shadowMappingShader.setMat4("model", horseModel2);
		horseObjModel->RenderModel(shadowMappingShader, horseModel2);
		horseObjModel->RenderModel(shadowMappingDepthShader, horseModel2);

		horseModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.0f));
		horseModel2 = glm::translate(horseModel2, glm::vec3(24.0f, 0.0f, 35.0f));
		shadowMappingShader.setMat4("model", horseModel2);
		horseObjModel->RenderModel(shadowMappingShader, horseModel2);
		horseObjModel->RenderModel(shadowMappingDepthShader, horseModel2);


		//OBJECTS
		shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
		glm::mat4 poolModel = glm::scale(glm::mat4(1.0), glm::vec3(0.3f));
		poolModel = glm::translate(poolModel, glm::vec3(-5.0, 0.0, -5.0));
		shadowMappingShader.setMat4("model", poolModel);
		poolObjModel->RenderModel(shadowMappingShader, poolModel);
		poolObjModel->RenderModel(shadowMappingDepthShader, poolModel);

		//shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		 
		
		glm::mat4 musicModel = glm::scale(glm::mat4(1.0), glm::vec3(0.001f));
		musicModel = glm::translate(musicModel, glm::vec3(-1000.0, 0.0, -6500.0));
		shadowMappingShader.setMat4("model", musicModel);
		musicObjModel->RenderModel(shadowMappingShader, musicModel);
		musicObjModel->RenderModel(shadowMappingDepthShader, musicModel);

		//BENCHES
		glm::mat4 benchModel = glm::scale(glm::mat4(1.0), glm::vec3(0.01f));
		benchModel = glm::translate(benchModel, glm::vec3(200.0, 0.0, -100.0));
		benchModel = glm::rotate(benchModel, glm::radians(-130.0f), glm::vec3(0, 1, 0));
		shadowMappingDepthShader.setMat4("model", benchModel);
		benchObjModel->RenderModel(shadowMappingShader, benchModel);
		benchObjModel->RenderModel(shadowMappingDepthShader, benchModel);

		benchModel = glm::scale(glm::mat4(1.0), glm::vec3(0.01f));
		benchModel = glm::translate(benchModel, glm::vec3(-500.0, 0.0, -40.0));
		benchModel = glm::rotate(benchModel, glm::radians(140.0f), glm::vec3(0, 1, 0));
		shadowMappingDepthShader.setMat4("model", benchModel);
		benchObjModel->RenderModel(shadowMappingShader, benchModel);
		benchObjModel->RenderModel(shadowMappingDepthShader, benchModel);

#pragma region Water

		float time = glfwGetTime();
		float verticalOffset = Amplitude * sin(Speed * time);
		glm::mat4 sphereModel = glm::scale(glm::mat4(1.0), glm::vec3(0.05f));
		sphereModel = glm::translate(sphereModel, glm::vec3(-29.0, 80.0 + verticalOffset, -29.5));
		shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
		shadowMappingShader.setMat4("model", sphereModel);
		sphereObjModel->RenderModel(shadowMappingShader, sphereModel);
		sphereObjModel->RenderModel(shadowMappingDepthShader, sphereModel);
		for (int i = 0; i < numSpheres; ++i) {
			float angle = i * (2 * M_PI / numSpheres); // Calculate the angle for each sphere
			float x = -29.0 + radius * cos(angle); // Calculate the x position
			float z = -29.5 + radius * sin(angle); // Calculate the z position

			glm::mat4 sphereModel = glm::scale(glm::mat4(1.0), glm::vec3(0.05f));
			sphereModel = glm::translate(sphereModel, glm::vec3(x, 60.0 + verticalOffset, z));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel);
			sphereObjModel->RenderModel(shadowMappingShader, sphereModel);
			sphereObjModel->RenderModel(shadowMappingDepthShader, sphereModel);
		}

#pragma endregion


		//car
		//car
		totalTime += deltaTime;  // Increment the total time by the elapsed time since the last frame

		if (totalTime >= resetInterval) {
			float excessTime = totalTime - resetInterval;
			float requiredRotation = -fmod(carAngle, 2.0f * M_PI);  // Negative because we want to reverse the accumulated rotation

			// Apply the correction in the current frame if the excess time is less than your frame interval
			if (excessTime < deltaTime) {
				carAngle += requiredRotation;
			}

			totalTime = excessTime;  // Reset the timer, keep the overflow to maintain accuracy
			carAngle = 0.0f;  // Reset car angle or set to initial orientation if not originally zero
		}

		glm::mat4 carModel = glm::mat4(1.0f);
		//carModel = glm::rotate(carModel, glm::radians(180.0f), glm::vec3(0, 1, 0));

		carPathAngle += angularSpeedPath * deltaTime;
		carPathAngle = fmod(carPathAngle, 2.0f * M_PI);

		if (timeSinceLastRotation >= rotationInterval && totalTime + deltaTime < resetInterval) {
			carAngle += rotationAngle;
			timeSinceLastRotation = 0.0f;
		}

		//carAngle = fmod(carAngle, 2.0f * M_PI);

		// Calculate the position of the car on the circular path
		glm::vec3 carPosition = circleCenter + glm::vec3(cos(carPathAngle) * carRadius, 0.0f, sin(carPathAngle) * carRadius);

		carModel = glm::translate(glm::mat4(1.0f), carPosition);

		//carModel = glm::rotate(carModel, carPathAngle, glm::vec3(0, 1, 0)); /
		carModel = glm::rotate(carModel, carAngle, glm::vec3(0, 1, 0)); // Add fixed 30-degree rotation every 2 seconds

		shadowMappingShader.setMat4("model", carModel);
		carObjModel->RenderModel(shadowMappingShader, carModel);
		carObjModel->RenderModel(shadowMappingDepthShader, carModel);



		//car2
		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		glm::mat4 carModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		carModel2 = glm::translate(carModel2, glm::vec3(10.0, 0.0, 10.0));

		shadowMappingShader.setMat4("model", carModel2);
		carObjModel->RenderModel(shadowMappingShader, carModel2);
		carObjModel->RenderModel(shadowMappingDepthShader, carModel2);

		//car3
		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		glm::mat4 carModel3 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		carModel3 = glm::translate(carModel3, glm::vec3(-8.0, 0.0, 12.0));

		shadowMappingShader.setMat4("model", carModel3);
		carObjModel->RenderModel(shadowMappingShader, carModel3);
		carObjModel->RenderModel(shadowMappingDepthShader, carModel3);

		//car4
		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		glm::mat4 carModel4 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		carModel4 = glm::translate(carModel4, glm::vec3(-5.0, 0.0, -20.0));

		shadowMappingShader.setMat4("model", carModel4);
		carObjModel->RenderModel(shadowMappingShader, carModel4);
		carObjModel->RenderModel(shadowMappingDepthShader, carModel4);

		//car5
		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		glm::mat4 carModel5 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		carModel5 = glm::translate(carModel5, glm::vec3(11.5, 0.0, -20.5));

		shadowMappingShader.setMat4("model", carModel5);
		carObjModel->RenderModel(shadowMappingShader, carModel5);
		carObjModel->RenderModel(shadowMappingDepthShader, carModel5);

		//car6
		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		glm::mat4 carModel6 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		
		carModel6 = glm::translate(carModel6, glm::vec3(18.5, 0.0, -1.5));
		carModel6 = glm::rotate(carModel6, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		shadowMappingShader.setMat4("model", carModel6);
		carObjModel->RenderModel(shadowMappingShader, carModel6);
		carObjModel->RenderModel(shadowMappingDepthShader, carModel6);

		//women

		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		glm::mat4 womenModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		womenModel = glm::translate(womenModel, glm::vec3(0.0, 0.0, 0.0));

		shadowMappingShader.setMat4("model", womenModel);
		womenObjModel->RenderModel(shadowMappingShader, womenModel);
		womenObjModel->RenderModel(shadowMappingDepthShader, womenModel);

		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		glm::mat4 womenModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		womenModel2 = glm::translate(womenModel2, glm::vec3(30.0, 0.0, 35.0));

		shadowMappingShader.setMat4("model", womenModel2);
		women2ObjModel->RenderModel(shadowMappingShader, womenModel2);
		women2ObjModel->RenderModel(shadowMappingDepthShader, womenModel2);

#pragma region MainHouses
		shadowMappingShader.SetVec3("color", 0.8f, 0.40f, 0.40f);
		glm::mat4 houseMainModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		houseMainModel = glm::translate(houseMainModel, glm::vec3(10.0, 0.0, 15.3));
		houseMainModel = glm::rotate(houseMainModel, glm::radians(215.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMappingShader.setMat4("model", houseMainModel);
		houseMainObjModel->RenderModel(shadowMappingShader, houseMainModel);
		houseMainObjModel->RenderModel(shadowMappingDepthShader, houseMainModel);

		glm::mat4 houseMainModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		houseMainModel2 = glm::translate(houseMainModel2, glm::vec3(-7.0, 0.0, 18.0));
		houseMainModel2 = glm::rotate(houseMainModel2, glm::radians(170.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMappingShader.setMat4("model", houseMainModel2);
		houseMainObjModel->RenderModel(shadowMappingShader, houseMainModel2);
		houseMainObjModel->RenderModel(shadowMappingDepthShader, houseMainModel2);

		glm::mat4 houseMainModel3 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		houseMainModel3 = glm::translate(houseMainModel3, glm::vec3(-9.8, 0.0, -26.0));
		houseMainModel3 = glm::rotate(houseMainModel3, glm::radians(17.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMappingShader.setMat4("model", houseMainModel3);
		houseMainObjModel->RenderModel(shadowMappingShader, houseMainModel3);
		houseMainObjModel->RenderModel(shadowMappingDepthShader, houseMainModel3);

		glm::mat4 houseMainModel4 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		houseMainModel4 = glm::translate(houseMainModel4, glm::vec3(9.5, 0.0, -25.5));
		houseMainModel4 = glm::rotate(houseMainModel4, glm::radians(339.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMappingShader.setMat4("model", houseMainModel4);
		houseMainObjModel->RenderModel(shadowMappingShader, houseMainModel4);
		houseMainObjModel->RenderModel(shadowMappingDepthShader, houseMainModel4);

		glm::mat4 houseMainModel5 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		houseMainModel5 = glm::translate(houseMainModel5, glm::vec3(21.5, 0.0, -7.5));
		houseMainModel5 = glm::rotate(houseMainModel5, glm::radians(280.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMappingShader.setMat4("model", houseMainModel5);
		houseMainObjModel->RenderModel(shadowMappingShader, houseMainModel5);
		houseMainObjModel->RenderModel(shadowMappingDepthShader, houseMainModel5);
#pragma endregion
		


#pragma region fence
		glm::vec3 initialFenceTranslationLine0(0.0f, 0.5f, 30.0f);
		glm::vec3 initialFenceTranslationLine15(0.0f, 0.5f, 50.0f);
		glm::vec3 initialFenceTranslationLeft(-1.0f, 0.5f, 30.0f);
		glm::vec3 initialFenceTranslationRight(32.0f, 0.0f, 0.0f);
		for (int coloana = 0; coloana < 16; coloana++)
		{

			shadowMappingShader.SetVec3("color", 0.30f, 0.40f, 0.40f);
			glm::mat4 fenceModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
			fenceModel = glm::translate(fenceModel, initialFenceTranslationLine0);
			fenceModel = glm::rotate(fenceModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			fenceModel = glm::scale(fenceModel, glm::vec3(0.01, 0.01, 0.01));
			shadowMappingShader.setMat4("model", fenceModel);
			fenceMainObjModel->RenderModel(shadowMappingShader, fenceModel);
			fenceMainObjModel->RenderModel(shadowMappingDepthShader, fenceModel);

			shadowMappingShader.SetVec3("color", 0.30f, 0.40f, 0.40f);
			glm::mat4 fenceModelLine15 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
			fenceModelLine15 = glm::translate(fenceModelLine15, initialFenceTranslationLine15);
			fenceModelLine15 = glm::rotate(fenceModelLine15, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			fenceModelLine15 = glm::scale(fenceModelLine15, glm::vec3(0.01, 0.01, 0.01));
			shadowMappingShader.setMat4("model", fenceModelLine15);
			fenceMainObjModel->RenderModel(shadowMappingShader, fenceModelLine15);
			fenceMainObjModel->RenderModel(shadowMappingDepthShader, fenceModelLine15);

			initialFenceTranslationLine0.x += 2.0f;
			initialFenceTranslationLine15.x += 2.0f;
		}


		for (int rand = 0; rand < 6; rand++)
		{
			glm::mat4 firModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
			firModel = glm::translate(firModel, initialFenceTranslationLeft);
			shadowMappingShader.setMat4("model", firModel);
			firObjModel->RenderModel(shadowMappingShader, firModel);
			firObjModel->RenderModel(shadowMappingDepthShader, firModel);


			glm::mat4 firModel2 = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
			firModel2 = glm::translate(firModel, initialFenceTranslationRight);
			shadowMappingShader.setMat4("model", firModel2);
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			firObjModel->Draw(shadowMappingShader);

			initialFenceTranslationLeft.z += 3.5f;
			initialFenceTranslationRight.z += 0.7f;
		}

#pragma endregion

		//cat
		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		glm::mat4 catModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		catModel = glm::translate(catModel, glm::vec3(7.0, 0.0, 0.0));
		catModel = glm::rotate(catModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		catModel = glm::scale(catModel, glm::vec3(0.01, 0.01, 0.01));
		shadowMappingShader.setMat4("model", catModel);
		catObjModel->RenderModel(shadowMappingShader, catModel);
		catObjModel->RenderModel(shadowMappingDepthShader, catModel);

		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		catModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		catModel = glm::translate(catModel, glm::vec3(35.0, 0.0, -40.0));
		catModel = glm::rotate(catModel, glm::radians(320.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMappingShader.setMat4("model", catModel);
		houseObjModel->RenderModel(shadowMappingShader, catModel);
		houseObjModel->RenderModel(shadowMappingDepthShader, catModel);

#pragma region StreetLamps
		//streetLamp
		glActiveTexture(GL_TEXTURE0); // Activate the texture unit 0
		glBindTexture(GL_TEXTURE_2D, streetLampTextureId); // Bind the texture
		shadowMappingShader.use(); // Activate the shader
		shadowMappingShader.setInt("diffuseTexture", 0); // Bind street lamp texture

		glm::mat4 streetLampModel = glm::scale(glm::mat4(0.5), glm::vec3(0.3f));
		streetLampModel = glm::translate(streetLampModel, glm::vec3(50.0, -8.0, 15.0));
		shadowMappingShader.setMat4("model", streetLampModel);
		streetLampObjModel->RenderModel(shadowMappingShader, streetLampModel);
		streetLampObjModel->RenderModel(shadowMappingDepthShader, streetLampModel);

		glm::mat4 streetLampModel8 = glm::scale(glm::mat4(0.5), glm::vec3(0.3f));
		streetLampModel8 = glm::translate(streetLampModel8, glm::vec3(5.0, -8.0, 50.0));
		shadowMappingShader.setMat4("model", streetLampModel8);
		streetLampObjModel->RenderModel(shadowMappingShader, streetLampModel8);
		streetLampObjModel->RenderModel(shadowMappingDepthShader, streetLampModel8);

		glm::mat4 streetLampModel9 = glm::scale(glm::mat4(0.5), glm::vec3(0.3f));
		streetLampModel9 = glm::translate(streetLampModel9, glm::vec3(-9.0, -8.0, 45.0));
		shadowMappingShader.setMat4("model", streetLampModel9);
		streetLampObjModel->RenderModel(shadowMappingShader, streetLampModel9);
		streetLampObjModel->RenderModel(shadowMappingDepthShader, streetLampModel9);

		glm::mat4 streetLampModel10 = glm::scale(glm::mat4(0.5), glm::vec3(0.3f));
		streetLampModel10 = glm::translate(streetLampModel10, glm::vec3(-35.0, -8.0, 45.0));
		shadowMappingShader.setMat4("model", streetLampModel10);
		streetLampObjModel->RenderModel(shadowMappingShader, streetLampModel10);
		streetLampObjModel->RenderModel(shadowMappingDepthShader, streetLampModel10);

		glm::mat4 streetLampModel2 = glm::scale(glm::mat4(0.5), glm::vec3(0.3f));
		streetLampModel2 = glm::translate(streetLampModel2, glm::vec3(35.0, -8.0, 35.0));
		shadowMappingShader.setMat4("model", streetLampModel2);
		streetLampObjModel->RenderModel(shadowMappingShader, streetLampModel2);
		streetLampObjModel->RenderModel(shadowMappingDepthShader, streetLampModel2);

		glm::mat4 streetLampModel3 = glm::scale(glm::mat4(0.5), glm::vec3(0.3f));
		streetLampModel3 = glm::translate(streetLampModel3, glm::vec3(56.0, -8.0, -30.0));
		shadowMappingShader.setMat4("model", streetLampModel3);
		streetLampObjModel->RenderModel(shadowMappingShader, streetLampModel3);
		streetLampObjModel->RenderModel(shadowMappingDepthShader, streetLampModel3);

		glm::mat4 streetLampModel4 = glm::scale(glm::mat4(0.5), glm::vec3(0.3f));
		streetLampModel4 = glm::translate(streetLampModel4, glm::vec3(50.0, -8.0, -52.0));
		shadowMappingShader.setMat4("model", streetLampModel4);
		streetLampObjModel->RenderModel(shadowMappingShader, streetLampModel4);
		streetLampObjModel->RenderModel(shadowMappingDepthShader, streetLampModel4);

		glm::mat4 streetLampModel5 = glm::scale(glm::mat4(0.5), glm::vec3(0.3f));
		streetLampModel5 = glm::translate(streetLampModel5, glm::vec3(20.0, -8.0, -75.0));
		shadowMappingShader.setMat4("model", streetLampModel5);
		streetLampObjModel->RenderModel(shadowMappingShader, streetLampModel5);
		streetLampObjModel->RenderModel(shadowMappingDepthShader, streetLampModel5);

		glm::mat4 streetLampModel6 = glm::scale(glm::mat4(0.5), glm::vec3(0.3f));
		streetLampModel6 = glm::translate(streetLampModel6, glm::vec3(-5.0, -8.0, -90.0));
		shadowMappingShader.setMat4("model", streetLampModel6);
		streetLampObjModel->RenderModel(shadowMappingShader, streetLampModel6);
		streetLampObjModel->RenderModel(shadowMappingDepthShader, streetLampModel6);

		glm::mat4 streetLampModel7 = glm::scale(glm::mat4(0.5), glm::vec3(0.3f));
		streetLampModel7 = glm::translate(streetLampModel7, glm::vec3(-40.0, -8.0, -70.0));
		shadowMappingShader.setMat4("model", streetLampModel7);
		streetLampObjModel->RenderModel(shadowMappingShader, streetLampModel7);
		streetLampObjModel->RenderModel(shadowMappingDepthShader, streetLampModel7);
#pragma endregion



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

CameraType cameraType = CameraType::Free;
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
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) // EnterTown camera
	{
		cameraType = CameraType::EnterTown;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) // 3rd person camera
	{
		cameraType = CameraType::SceneUp;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) // free camera
	{
		cameraType = CameraType::Free;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);

	}
	switch (cameraType)
	{
	case CameraType::Free:
		break;
	case CameraType::SceneUp:
		pCamera->setViewMatrix(glm::vec3(startX - 15, startY + 50, startZ + 100));
		break;
	case CameraType::EnterTown:
		pCamera->setViewMatrix(glm::vec3(startX, startY + 2, startZ - 9.5));
		break;
	default:;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes

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
