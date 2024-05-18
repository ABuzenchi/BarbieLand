
#include <Windows.h>
#include <locale>
#include <codecvt>

#include <stdlib.h> 
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
glm::vec3 carPosition;
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
std::unique_ptr<Model>sittingCatObjModel;
std::unique_ptr<Model>gateObjModel;
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
	EnterTown,
	Car,
	House
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
double deltaTime = 0.0f;	
double lastFrame = 0.0f;

//day->night transition
float mixValue = 0.0f;
bool transitioning = false;
bool nightMode = false;
float factorAmbienta = 2.0f; 
float factorDifuzie = 2.0f;
float factorSpecular = 2.0f;

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
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, textureId);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0); 
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

	std::string gateObjFileName = (currentPath + "\\Models\\Object\\gate\\gate.obj");
	gateObjModel = std::make_unique<Model>(gateObjFileName, false);


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

	std::string groundFileName = (currentPath + "\\Models\\objects\\ground\\paths.obj");
	groundObj = std::make_unique<Model>(groundFileName, false);

	std::string catObjFileName = (currentPath + "\\Models\\Animals\\Cat\\cat.obj");
	catObjModel = std::make_unique<Model>(catObjFileName, false);
	std::string sittingCatObjFileName = (currentPath + "\\Models\\Animals\\sitting-cat\\sitting-cat.obj");
	sittingCatObjModel = std::make_unique<Model>(sittingCatObjFileName, false);

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



void RenderScene(Shader& shader, bool shadowPass = false) {
	glDisable(GL_CULL_FACE);  

	if (!shadowPass) {
		shader.setFloat("shininess", 32.0f);
		shader.setInt("diffuse", 0);
		shader.setInt("specular", 1);
	}
}

int main()
{
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	
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

	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	SoundManager soundManager;

	
	glm::vec3 soundPosition = glm::vec3(-1.0f, 0.0f, -6.5f);
	irrklang::ISound* sound1 = soundManager.play3DSound("../Audio/barbie.mp3", soundPosition.x, soundPosition.y, soundPosition.z, true, false, true);

	glm::vec3 soundPosition2 = glm::vec3(28.0f, 0.0f, 35.0f);
	irrklang::ISound* sound2 = soundManager.play3DSound("../Audio/Crickets.mp3", soundPosition2.x, soundPosition2.y, soundPosition2.z, true, false, true);


	
	soundManager.setVolume(sound2, 0.0f);
	float sound1Volume = 1.0f;
	float sound2Volume = 0.0f; 
	glewInit();

	glEnable(GL_DEPTH_TEST);

	
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
	
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	wchar_t buffer[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, buffer);

	std::wstring executablePath(buffer);
	std::wstring wscurrentPath = executablePath.substr(0, executablePath.find_last_of(L"\\/"));

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string currentPath = converter.to_bytes(wscurrentPath);

	//-----------SKYBOX--SETUP------------------------
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

	
	skybox.shaderConfiguration(currentPath);

	// Camera
	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 1.0, 3.0));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shadowMappingShader((currentPath + "\\Shaders\\ShadowMapping.vs").c_str(), (currentPath + "\\Shaders\\ShadowMapping.fs").c_str());
	Shader shadowMappingDepthShader((currentPath + "\\Shaders\\ShadowMappingDepth.vs").c_str(), (currentPath + "\\Shaders\\ShadowMappingDepth.fs").c_str());

	const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	
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
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		timeSinceLastRotation += deltaTime;
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view;
		glm::mat4 projection = glm::perspective(glm::radians(pCamera->FoVy), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::vec3 camPos = pCamera->GetPosition();
		glm::vec3 camLookAt = pCamera->GetPosition() + pCamera->forward;
		glm::vec3 camUp = pCamera->up;
		glm::vec3 camVelocity = glm::vec3(0.0f);
		soundManager.updateListenerPosition(camPos, camLookAt, camUp, camVelocity);

		if (transitioning) {
			const float transitionSpeed = 0.3f;

			if (nightMode) {
				mixValue += deltaTime * transitionSpeed;
				factorAmbienta -= deltaTime * transitionSpeed;
				if (factorAmbienta <= 0.2f) factorAmbienta = 0.0f;
				factorDifuzie -= deltaTime * transitionSpeed;
				if (factorDifuzie <= 0.5f) factorDifuzie = 0.2f;
				factorSpecular -= deltaTime * transitionSpeed;
				if (factorSpecular <= 0.5f) factorSpecular = 0.2f;
				if (mixValue >= 1.0f) {
					mixValue = 1.0f;
					transitioning = false; 
				}

				
				sound1Volume -= deltaTime * transitionSpeed;
				if (sound1Volume <= 0.0f) sound1Volume = 0.0f;
				sound2Volume += deltaTime * transitionSpeed * 5;
				if (sound2Volume >= 20.0f) sound2Volume = 20.0f;
			}
			else {
				mixValue -= deltaTime * transitionSpeed;
				factorAmbienta += deltaTime * transitionSpeed;
				if (factorAmbienta >= 2.0f) factorAmbienta = 2.0f;
				factorDifuzie += deltaTime * transitionSpeed;
				if (factorDifuzie >= 2.0f) factorDifuzie = 2.0f;
				factorSpecular += deltaTime * transitionSpeed;
				if (factorSpecular >= 2.0f) factorSpecular = 2.0f;
				if (mixValue <= 0.0f) {
					mixValue = 0.0f;
					transitioning = false; 
				}

				
				sound1Volume += deltaTime * transitionSpeed;
				if (sound1Volume >= 1.0f) sound1Volume = 1.0f;
				sound2Volume -= deltaTime * transitionSpeed*5;
				if (sound2Volume <= 0.0f) sound2Volume = 0.0f;
			}

			
			soundManager.setVolume(sound1, sound1Volume);
			soundManager.setVolume(sound2, sound2Volume);
		}



		//Render skybox
		skybox.renderSkybox(mixValue, pCamera, SCR_WIDTH, SCR_HEIGHT);

		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 5.0f, far_plane = 50.f;
		lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;

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
		glBindTexture(GL_TEXTURE_2D, floorTextureId);  
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap); 
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

		glm::mat4 treeModel = glm::scale(glm::mat4(1.0), glm::vec3(0.03f));
		treeModel = glm::translate(treeModel, glm::vec3(490.0, 00.0, 80.0));
		treeModel = glm::rotate(treeModel, glm::radians(70.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMappingShader.setMat4("model", treeModel);
		gateObjModel->RenderModel(shadowMappingShader, treeModel);
		gateObjModel->RenderModel(shadowMappingDepthShader, treeModel);
#pragma region Trees

		treeModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		treeModel = glm::translate(treeModel, glm::vec3(-10.0, 0.0, 10.0));
		shadowMappingShader.setMat4("model", treeModel);
		treeObjModel->RenderModel(shadowMappingShader, treeModel);
		treeObjModel->RenderModel(shadowMappingDepthShader, treeModel);


		// Trees
		const float radius = 10.0f;
		const float circleRadius = 18.0f;
		const float circleRadius2 = 8.0f;
		const float circleRadius3 = 40.0f;

		for (int i = 0; i < 10; ++i) {
			float angle = i * (2 * M_PI / numSpheres);

			// Big circle of trees
			float x1 = -45.0f + circleRadius * cos(angle);
			float z1 = -29.5f + circleRadius * sin(angle);
			glm::mat4 sphereModel1 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
			sphereModel1 = glm::translate(sphereModel1, glm::vec3(x1, 0.0, z1));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel1);
			treeObjModel->RenderModel(shadowMappingShader, sphereModel1);
			treeObjModel->RenderModel(shadowMappingDepthShader, sphereModel1);

			// Big circle of trees
			x1 = 0.0f + circleRadius3 * cos(angle);
			z1 = 0.0f + circleRadius3 * sin(angle);
			sphereModel1 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
			sphereModel1 = glm::translate(sphereModel1, glm::vec3(x1, 0.0, z1));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel1);
			treeObjModel->RenderModel(shadowMappingShader, sphereModel1);
			treeObjModel->RenderModel(shadowMappingDepthShader, sphereModel1);

			// Big circle of trees
			x1 = -45.0f + circleRadius * cos(angle);
			z1 = 15.5f + circleRadius * sin(angle); 
			sphereModel1 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
			sphereModel1 = glm::translate(sphereModel1, glm::vec3(x1, 0.0, z1));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel1);
			treeObjModel->RenderModel(shadowMappingShader, sphereModel1);
			treeObjModel->RenderModel(shadowMappingDepthShader, sphereModel1);

			// Small circle of trees
			float x2 = -45.0f + circleRadius2 * cos(angle);
			float z2 = -29.5f + circleRadius2 * sin(angle); 
			glm::mat4 sphereModel2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)); 
			sphereModel2 = glm::translate(sphereModel2, glm::vec3(x2, 0.0, z2));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel2);
			treeObjModel->RenderModel(shadowMappingShader, sphereModel2);
			treeObjModel->RenderModel(shadowMappingDepthShader, sphereModel2);

			// Small circle of trees
			x2 = -45.0f + circleRadius2 * cos(angle); 
			z2 = 15.5f + circleRadius2 * sin(angle);
			sphereModel2 = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
			sphereModel2 = glm::translate(sphereModel2, glm::vec3(x2, 0.0, z2));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel2);
			treeObjModel->RenderModel(shadowMappingShader, sphereModel2);
			treeObjModel->RenderModel(shadowMappingDepthShader, sphereModel2);
		}


		
		// Static trees outside the loop

		glm::vec3 treePositions[8] = {
		glm::vec3(-15.0, 0.0, 1.0),
		glm::vec3(-15.0, 0.0, -10.0),
		glm::vec3(0.0, 0.0, -35.0),
		glm::vec3(14.0, 0.0, -35.0),
		glm::vec3(30.0, 0.0, 0.0),
		glm::vec3(35.0, 0.0, 10.0),
		glm::vec3(25.0, 0.0, 20.0),
		glm::vec3(-10.0, 0.0, 10.0)
		};


		for (int i = 0; i < 8; ++i) {
			glm::mat4 treeModel = glm::scale(glm::mat4(1.0f), glm::vec3(1.f));
			treeModel = glm::translate(treeModel, treePositions[i]);
			shadowMappingShader.setMat4("model", treeModel);
			treeObjModel->RenderModel(shadowMappingShader, treeModel);
			treeObjModel->RenderModel(shadowMappingDepthShader, treeModel);
		}


#pragma endregion


		//ANIMALS


		glm::mat4 horseModel;
		float positions[10][3] = {
			{28.0f, 0.0f, 35.0f},
			{24.0f, 0.0f, 40.0f},
			{16.0f, 0.0f, 40.0f},
			{16.0f, 0.0f, 33.0f},
			{12.0f, 0.0f, 37.0f},
			{1.0f, 0.0f, 40.0f},
			{9.0f, 0.0f, 42.0f},
			{5.0f, 0.0f, 31.0f},
			{7.0f, 0.0f, 39.0f},
			{4.0f, 0.0f, 34.0f}
		};
		float rotations[10] = {
			0.0f,
			15.0f,
			30.0f,
			45.0f,
			60.0f,
			75.0f,
			90.0f,
			105.0f,
			120.0f,
			180.0f
		};

		for (int i = 0; i < 10; ++i) {
			horseModel = glm::mat4(1.0f);
			horseModel = glm::translate(horseModel, glm::vec3(positions[i][0], positions[i][1], positions[i][2]));
			horseModel = glm::rotate(horseModel, glm::radians(rotations[i]), glm::vec3(0.0f, 1.0f, 0.0f));
			horseModel = glm::scale(horseModel, glm::vec3(1.0f));
			shadowMappingShader.setMat4("model", horseModel);
			horseObjModel->RenderModel(shadowMappingShader, horseModel);
			horseObjModel->RenderModel(shadowMappingDepthShader, horseModel);
		}



		//OBJECTS
		shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
		glm::mat4 poolModel = glm::scale(glm::mat4(1.0), glm::vec3(0.3f));
		poolModel = glm::translate(poolModel, glm::vec3(-5.0, 0.0, -5.0));
		shadowMappingShader.setMat4("model", poolModel);
		poolObjModel->RenderModel(shadowMappingShader, poolModel);
		poolObjModel->RenderModel(shadowMappingDepthShader, poolModel);
 
		
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
			float angle = i * (2 * M_PI / numSpheres);
			float x = -29.0 + radius * cos(angle);
			float z = -29.5 + radius * sin(angle); 

			glm::mat4 sphereModel = glm::scale(glm::mat4(1.0), glm::vec3(0.05f));
			sphereModel = glm::translate(sphereModel, glm::vec3(x, 60.0 + verticalOffset, z));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel);
			sphereObjModel->RenderModel(shadowMappingShader, sphereModel);
			sphereObjModel->RenderModel(shadowMappingDepthShader, sphereModel);
		}

		for (int i = 0; i < 12; ++i) {
			float angle = i * (2 * M_PI / 12); 
			float x = -29.0 + 15.0 * cos(angle); 
			float z = -29.5 + 15.0 * sin(angle);

			glm::mat4 sphereModel = glm::scale(glm::mat4(1.0), glm::vec3(0.05f));
			sphereModel = glm::translate(sphereModel, glm::vec3(x, 40.0 + verticalOffset, z));
			shadowMappingShader.SetVec3("color", 1.0f, 1.0f, 0.6f);
			shadowMappingShader.setMat4("model", sphereModel);
			sphereObjModel->RenderModel(shadowMappingShader, sphereModel);
			sphereObjModel->RenderModel(shadowMappingDepthShader, sphereModel);
		}

#pragma endregion


		//car
		totalTime += deltaTime; 

		if (totalTime >= resetInterval) {
			float excessTime = totalTime - resetInterval;
			float requiredRotation = -fmod(carAngle, 2.0f * M_PI);  

			if (excessTime < deltaTime) {
				carAngle += requiredRotation;
			}

			totalTime = excessTime;  
			carAngle = 0.0f;  
		}

		glm::mat4 carModel = glm::mat4(1.0f);
		
		carPathAngle += angularSpeedPath * deltaTime;
		carPathAngle = fmod(carPathAngle, 2.0f * M_PI);

		if (timeSinceLastRotation >= rotationInterval && totalTime + deltaTime < resetInterval) {
			carAngle += rotationAngle;
			timeSinceLastRotation = 0.0f;
		}

		carPosition = circleCenter + glm::vec3(cos(carPathAngle) * carRadius, 0.0f, sin(carPathAngle) * carRadius);
		carModel = glm::translate(glm::mat4(1.0f), carPosition);
		carModel = glm::rotate(carModel, carAngle, glm::vec3(0, 1, 0)); 

		shadowMappingShader.setMat4("model", carModel);
		carObjModel->RenderModel(shadowMappingShader, carModel);
		carObjModel->RenderModel(shadowMappingDepthShader, carModel);



		float carPositions[6][3] = {
	    {10.0f, 0.0f, 10.0f},
	    {-8.0f, 0.0f, 12.0f},
	    {-5.0f, 0.0f, -20.0f},
	    {11.5f, 0.0f, -20.5f},
	    {18.5f, 0.0f, -1.5f}
		};
		float carRotations[6] = {
			0.0f,
			0.0f,
			0.0f,
			0.0f,
			270.0f
		};

		for (int i = 0; i < 5; ++i) {
			carModel = glm::mat4(1.0f);
			carModel = glm::translate(carModel, glm::vec3(carPositions[i][0], carPositions[i][1], carPositions[i][2]));
			carModel = glm::rotate(carModel, glm::radians(carRotations[i]), glm::vec3(0.0f, 1.0f, 0.0f));
			carModel = glm::scale(carModel, glm::vec3(1.0f));
			shadowMappingShader.setMat4("model", carModel);
			carObjModel->RenderModel(shadowMappingShader, carModel);
			carObjModel->RenderModel(shadowMappingDepthShader, carModel);
		}

		//women


		glm::mat4 womenModel;
		float womenPositionss[2][3] = {
		{0.0f, 0.0f, 0.0f},
		{-30.0f, 0.3f, -4.0f}
		};
		float womenRotationss[2] = {
			0.0f,
			90.0f
		};

		for (int i = 0; i < 2; ++i) {
			womenModel = glm::mat4(1.0f);
			womenModel = glm::translate(womenModel, glm::vec3(womenPositionss[i][0], womenPositionss[i][1], womenPositionss[i][2]));
			womenModel = glm::rotate(womenModel, glm::radians(womenRotationss[i]), glm::vec3(0.0f, 1.0f, 0.0f));
			womenModel = glm::scale(womenModel, glm::vec3(1.0f));
			shadowMappingShader.setMat4("model", womenModel);
			womenObjModel->RenderModel(shadowMappingShader, womenModel);
			womenObjModel->RenderModel(shadowMappingDepthShader, womenModel);
		}

		glm::mat4 womenModel2;
		float womenPositions[5][3] = {
		{30.0f, 0.0f, 35.0f},
		{35.0f, 0.0f, -40.0f},
		{-9.0f, 0.0f, -18.0f},
		{13.0f, 4.0f, 13.3f},
		{35.0f,0.5f,-20.0f}
		};
		float womenRotations[5] = {
			0.0f,
			0.0f,
			0.0f,
			180.0f,
			320.0f
		};

		for (int i = 0; i < 5; ++i) {
			womenModel2 = glm::mat4(1.0f);
			womenModel2 = glm::translate(womenModel2, glm::vec3(womenPositions[i][0], womenPositions[i][1], womenPositions[i][2]));
			womenModel2 = glm::rotate(womenModel2, glm::radians(womenRotations[i]), glm::vec3(0.0f, 1.0f, 0.0f));
			womenModel2 = glm::scale(womenModel2, glm::vec3(1.0f));
			shadowMappingShader.setMat4("model", womenModel2);
			women2ObjModel->RenderModel(shadowMappingShader, womenModel2);
			women2ObjModel->RenderModel(shadowMappingDepthShader, womenModel2);
		}


#pragma region MainHouses

		glm::vec3 housePositions[5] = {
		glm::vec3(10.0, 0.0, 15.3),
		glm::vec3(-7.0, 0.0, 18.0),
		glm::vec3(-9.8, 0.0, -26.0),
		glm::vec3(9.5, 0.0, -25.5),
		glm::vec3(21.5, 0.0, -7.5)
		};
		float houseRotations[5] = {
			215.0f,
			170.0f,
			17.0f,
			339.0f,
			280.0f
		};

		for (int i = 0; i < 5; ++i) {
			shadowMappingShader.SetVec3("color", 0.8f, 0.40f, 0.40f);
			glm::mat4 houseModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
			houseModel = glm::translate(houseModel, housePositions[i]);
			houseModel = glm::rotate(houseModel, glm::radians(houseRotations[i]), glm::vec3(0.0f, 1.0f, 0.0f));
			shadowMappingShader.setMat4("model", houseModel);
			houseMainObjModel->RenderModel(shadowMappingShader, houseModel);
			houseMainObjModel->RenderModel(shadowMappingDepthShader, houseModel);
		}
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
		catModel = glm::translate(catModel, glm::vec3(1.0, 0.0, 0.0));
		catModel = glm::rotate(catModel, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		catModel = glm::scale(catModel, glm::vec3(0.01, 0.01, 0.01));
		shadowMappingShader.setMat4("model", catModel);
		catObjModel->RenderModel(shadowMappingShader, catModel);
		catObjModel->RenderModel(shadowMappingDepthShader, catModel);
		
		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		glm::mat4 sittingCatModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		sittingCatModel = glm::translate(sittingCatModel, glm::vec3(-5.0, 0.65, -0.4));
		sittingCatModel = glm::rotate(sittingCatModel, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
		shadowMappingShader.setMat4("model", sittingCatModel);
		sittingCatObjModel->RenderModel(shadowMappingShader, sittingCatModel);
		sittingCatObjModel->RenderModel(shadowMappingDepthShader, sittingCatModel);


		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		catModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		catModel = glm::translate(catModel, glm::vec3(35.0, 0.5, -40.0));
		catModel = glm::rotate(catModel, glm::radians(320.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMappingShader.setMat4("model", catModel);
		houseObjModel->RenderModel(shadowMappingShader, catModel);
		houseObjModel->RenderModel(shadowMappingDepthShader, catModel);

		shadowMappingShader.SetVec3("color", 0.76f, 0.64f, 0.6f);
		catModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		catModel = glm::translate(catModel, glm::vec3(35.0, 0.5, -20.0));
		catModel = glm::rotate(catModel, glm::radians(320.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMappingShader.setMat4("model", catModel);
		women2ObjModel->RenderModel(shadowMappingShader, catModel);
		women2ObjModel->RenderModel(shadowMappingDepthShader, catModel);

		catModel = glm::scale(glm::mat4(1.0), glm::vec3(1.f));
		catModel = glm::translate(catModel, glm::vec3(-30.0, 0.3, -4.0));
		catModel = glm::rotate(catModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shadowMappingShader.setMat4("model", catModel);
		womenObjModel->RenderModel(shadowMappingShader, catModel);
		womenObjModel->RenderModel(shadowMappingDepthShader, catModel);


#pragma region StreetLamps
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, streetLampTextureId);
		shadowMappingShader.use();
		shadowMappingShader.setInt("diffuseTexture", 0);

		glm::vec3 streetLampPositions[10] = {
			glm::vec3(50.0, -8.0, 15.0),
			glm::vec3(5.0, -8.0, 50.0),
			glm::vec3(-9.0, -8.0, 45.0),
			glm::vec3(-35.0, -8.0, 45.0),
			glm::vec3(35.0, -8.0, 35.0),
			glm::vec3(56.0, -8.0, -30.0),
			glm::vec3(50.0, -8.0, -52.0),
			glm::vec3(20.0, -8.0, -75.0),
			glm::vec3(-5.0, -8.0, -90.0),
			glm::vec3(-40.0, -8.0, -70.0)
		};

		for (int i = 0; i < 10; ++i) {
			glm::mat4 streetLampModel = glm::scale(glm::mat4(0.5f), glm::vec3(0.3f));
			streetLampModel = glm::translate(streetLampModel, streetLampPositions[i]);
			shadowMappingShader.setMat4("model", streetLampModel);
			streetLampObjModel->RenderModel(shadowMappingShader, streetLampModel);
			streetLampObjModel->RenderModel(shadowMappingDepthShader, streetLampModel);
		}

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
		nightMode = !nightMode;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) 
	{
		cameraType = CameraType::EnterTown;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		cameraType = CameraType::SceneUp;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) 
	{
		cameraType = CameraType::Free;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) 
	{
		cameraType = CameraType::Car;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) 
	{
		cameraType = CameraType::House;
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
	case CameraType::Car:
		pCamera->setViewMatrix(carPosition + glm::vec3(0.0f, 5.0f, 0.0f)); 
		break;
		
	case CameraType::House:
		pCamera->setViewMatrix(glm::vec3(35.0, 2.0, -40.0));
		break;
	default:;
	}
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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
