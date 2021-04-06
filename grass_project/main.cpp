/* This application renders dynamic grass that can be influenced
 * by varying types of wind that can be configured by the user.
 *
 * This project was created as part of the Graphics Programming course
 * at the IT University of Copenhagen.
*/
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cmath>

#include <vector>
#include <chrono>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "patch.h"
#include "shader.h"
#include "shader_program.h"
#include "glmutils.h"
#include "primitives.h"
#include "camera.h"
#include "debug.h"
#include "scene.h"
#include "scene_object.h"
#include "scene_object_instanced.h"
#include "scene_object_indexed.h"
#include "scene_object_arrays.h"
#include "perlin_noise.h"

// Constants
const unsigned int INIT_SCR_WIDTH = 1000;
const unsigned int INIT_SCR_HEIGHT = 1000;
// Maximum amount of grass blades per patch
const unsigned int MAX_PATCH_DENSITY_BLADES = 40000;
const unsigned int MAX_PATCHES = 81;

Scene scene;

Patch patch;

glm::mat4 lightMatrix;

Shader* bladesVertexShader;
Shader* patchVertexShader;
Shader* skyboxVertexShader;
Shader* lightVertexShader;
Shader* bladesFragmentShader;
Shader* patchFragmentShader;
Shader* skyboxFragmentShader;
Shader* lightFragmentShader;
Shader* computeShader;
Shader* computeFragmentShader;

ShaderProgram* bladesShaderProgram;
ShaderProgram* patchShaderProgram;
ShaderProgram* skyboxShaderProgram;
ShaderProgram* lightShaderProgram;
ShaderProgram* computeShaderProgram;
ShaderProgram* computeFragmentShaderProgram;
unsigned int instanceMatrixBuffer;

Texture billboardGrassNoise1;
Texture billboardGrassNoise2;

Texture cubemapTextureDay;
Texture cubemapTextureNight;


Texture perlinNoiseTexture;
Texture seedTexture;

float* perlinNoiseTextureData;
float* seedTextureData;

// Camera setup
Camera camera;
bool isPaused = false;  // Used to stop camera movement when GUI is open
bool firstMouse = false;
float lastX = (float)INIT_SCR_WIDTH / 2.0;
float lastY = (float)INIT_SCR_HEIGHT / 2.0;

// Timing 
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

GLFWwindow* window;

GLFWwindow* initGLFWWindow();
void initIMGUI(GLFWwindow* window);
void initShadersAndTextures();
glm::vec2 calculateSpiralPosition(int n);
void initSceneObjects(Patch& patch);
void generatePerlinNoise();
void createInstanceMatrixBuffer(glm::mat4* modelMatrices, const unsigned int MAX_PATCH_DENSITY_BLADES);

void drawGui();

void cursorInRange(float screenX, float screenY, int screenW, int screenH, float min, float max, float& x, float& y);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void keyInputCallback(GLFWwindow* window, int button, int other, int action, int mods);
void cursorInputCallback(GLFWwindow* window, double posX, double posY);
void cleanUp();

int main()
{
	window = initGLFWWindow();

	assert(window != NULL);

	// GLAD: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "ERROR:: Failed to initialize GLAD" << std::endl;
		return -1;
	}

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	if (!(major >= 4 && minor >= 3)) {
		std::cout << "ERROR:: Wrong OpenGL version" << std::endl;
		return -1;
	}

	initShadersAndTextures();
	initSceneObjects(patch);
	generatePerlinNoise();

	camera.camPosition = { -15, 20, 0 };
	camera.yaw = 0;
	camera.pitch = -50;
	camera.updateCameraVectors();

	// Render loop : render every loopInterval seconds
	float loopInterval = 0.02f;

	// Set seed for random numbers
	srand((unsigned)time(0));

	initIMGUI(window);


	while (!glfwWindowShouldClose(window))
	{
		// Poll events at start so you have the newest inputs
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		// For correcting the camera input in processInput
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;

		processInput(window);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		// Clear the depth buffer (aka z-buffer) every new frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bladesShaderProgram->use();

		int width, height;
		glfwGetWindowSize(window, &width, &height); 
		glm::mat4 projection = glm::perspectiveFovRH_NO(70.0f, (float)width,
			(float)height, .01f, 1000.0f); // FIX: not every frame
		glm::mat4 view = glm::lookAt(
			camera.getCamPosition(),
			camera.getCamPosition() + camera.getCamForward(), glm::vec3(0, 1, 0));
		scene.projection = projection;
		scene.view = view;


		//drawScene();
		scene.updateDynamic();
		scene.render();

		//GLCall(glActiveTexture(GL_TEXTURE0 + computeShaderTexture));
		//GLCall(glBindTexture(GL_TEXTURE_2D, computeShaderTexture));


		//computeFragmentShaderProgram->use();


		



		if (isPaused) {
			drawGui();
		}

		glfwSwapBuffers(window);

		// Control render loop frequency
		float elapsed = deltaTime;
		while (loopInterval > elapsed) {
			elapsed = (float)glfwGetTime() - lastFrame;
		}
		lastFrame = currentFrame;
	}

	cleanUp();
	return 0;
}

GLFWwindow* initGLFWWindow() {
	// GLFW: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Fix compilation on OS X
	#endif

	// GLFW window creation
	GLFWwindow* window = glfwCreateWindow(INIT_SCR_WIDTH, INIT_SCR_HEIGHT, "GrassProject", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, cursorInputCallback);
	// Hide the cursor and capture it
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, keyInputCallback);

	return window;
}


/* Initializes shaders. Loads the information about the vertices, colors, normals
 * and indices (and uvs) from the primitives file. Loads textures.
 * Sets up the z-buffer.
 */
void initShadersAndTextures() {
	bladesVertexShader = new Shader("assets/shaders/blades.vert", GL_VERTEX_SHADER);
	bladesFragmentShader = new Shader("assets/shaders/blades.frag", GL_FRAGMENT_SHADER);

	bladesShaderProgram = new ShaderProgram({ bladesVertexShader, bladesFragmentShader }, "BLADES SHADER");

	patchVertexShader = new Shader("assets/shaders/patch.vert", GL_VERTEX_SHADER);
	patchFragmentShader = new Shader("assets/shaders/patch.frag", GL_FRAGMENT_SHADER);

	patchShaderProgram = new ShaderProgram({ patchVertexShader, patchFragmentShader }, "PATCH SHADER");

	std::string billboardGrassFileNameNoise1 = "assets/textures/misc/perlin_noise_1.tga";
	std::string billboardGrassFileNameNoise2 = "assets/textures/misc/perlin_noise_2.tga";
	billboardGrassNoise1.loadTexture(billboardGrassFileNameNoise1, false);
	billboardGrassNoise2.loadTexture(billboardGrassFileNameNoise2, false);


	// Setup the Skybox Shaders
	skyboxVertexShader = new Shader("assets/shaders/skybox.vert", GL_VERTEX_SHADER);
	skyboxFragmentShader = new Shader("assets/shaders/skybox.frag", GL_FRAGMENT_SHADER);
	skyboxShaderProgram = new ShaderProgram({ skyboxVertexShader, skyboxFragmentShader }, "SKYBOX SHADER");

	//skyboxShader.use();
	cubemapTextureDay.loadTextureCubeMap(facesDay, false);
	cubemapTextureNight.loadTextureCubeMap(facesNight);

	lightVertexShader = new Shader("assets/shaders/light.vert", GL_VERTEX_SHADER);
	lightFragmentShader = new Shader("assets/shaders/light.frag", GL_FRAGMENT_SHADER);
	lightShaderProgram = new ShaderProgram({ lightVertexShader, lightFragmentShader }, "LIGHT SHADER");

	GLCall(glGenTextures(1, &scene.perlinNoiseID));
	GLCall(glObjectLabel(GL_TEXTURE, scene.perlinNoiseID, -1, "perlinNoise"));
	GLCall(glBindTexture(GL_TEXTURE_2D, scene.perlinNoiseID));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, PERLIN_NOISE_TEXTURE_WIDTH, PERLIN_NOISE_TEXTURE_WIDTH, 0, GL_RED, GL_FLOAT, 0));
	GLCall(glBindImageTexture(0, scene.perlinNoiseID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	computeShader = new Shader("assets/shaders/compute.comp", GL_COMPUTE_SHADER);
	computeShaderProgram = new ShaderProgram({ computeShader }, "COMPUTE SHADER");
	
	//computeFragmentShader = new Shader("assets/shaders/compute.frag", GL_FRAGMENT_SHADER);
	//computeFragmentShaderProgram = new ShaderProgram({ computeFragmentShader }, "COMPUTE FRAGMENT SHADER");



	scene.cubemapTextureDay = &cubemapTextureDay;
	scene.cubemapTextureNight = &cubemapTextureNight;
	scene.currentTexture = scene.cubemapTextureNight;

	int width = PERLIN_NOISE_TEXTURE_WIDTH;
	int height = PERLIN_NOISE_TEXTURE_WIDTH;

	//perlinNoiseTextureData = new float[(long)width * height];



	seedTextureData = new float[width * height];
	//for (int i = 0; i < PERLIN_NOISE_TEXTURE_WIDTH * PERLIN_NOISE_TEXTURE_WIDTH; i++) seedTextureData[i] = (float)rand() / (float)RAND_MAX;


	/*perlinNoiseTexture.generateTexture(perlinNoiseTextureData, width, height, GL_RED);

	GLCall(glBindTexture(GL_TEXTURE_2D, seedTexture.getTextureID()));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, PERLIN_NOISE_TEXTURE_WIDTH, PERLIN_NOISE_TEXTURE_WIDTH, 0, GL_RED, GL_FLOAT, 0));*/

	//PerlinNoise2DCPU(width, height, 9, 2.0f, perlinNoiseTextureData, seedTextureData);

	seedTexture.generateTexture(seedTextureData, width, height, GL_RED);

	// Set up the z-buffer
	glDepthRange(-1, 1);  // Make the NDC a right handed coordinate system, 
						  // with the camera pointing towards -z
	glEnable(GL_DEPTH_TEST);  // Turn on z-buffer depth perlinNoiseTexture
	glDepthFunc(GL_LESS);  // Draws fragments that are closer to the screen in NDC
	glEnable(GL_MULTISAMPLE);
}

/**
 * Finds coordinates (position) of the number
 *
 * @param {Number} n - number to find position/coordinates for
 * @return {Number[]} - x and y coordinates of the number
 */
glm::vec2 calculateSpiralPosition(int n) {
	n++;
	int k = (int)std::ceil((std::sqrt(n) - 1.0) / 2.0);
	int t = 2 * k + 1;
	int m = t*t;

	t -= 1;

	if (n >= m - t) {
		return { k - (m - n), -k };
	}

	m -= t;

	if (n >= m - t) {
		return { -k, -k + (m - n) };
	}

	m -= t;

	if (n >= m - t) {
		return { -k + (m - n), k };
	}

	return { k, k - (m - n - t) };
}

void initSceneObjects(Patch& patch) {

	patch.init(MAX_PATCH_DENSITY_BLADES, patchShaderProgram);
	createInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_PATCH_DENSITY_BLADES);

	SceneObjectArrays* skybox = new SceneObjectArrays(cubePositions, *skyboxShaderProgram);
	scene.sceneObjects.push_back(skybox);

	SceneObjectArrays* light = new SceneObjectArrays(cubePositions, *lightShaderProgram);
	scene.sceneObjects.push_back(light);
	scene.light = light;

	for (int i = 0; i < MAX_PATCHES; i++) {
		
		glm::vec2 position = calculateSpiralPosition(i) * 10.0f;
		SceneObjectIndexed* patchSceneObject = new SceneObjectIndexed(grassPatchPositions, grassPatchColors,
			grassPatchIndices, grassPatchNormals, *patchShaderProgram, grassPatchUVs);
		patchSceneObject->model = glm::translate(position.x, 0, position.y);
		scene.patches.push_back(patchSceneObject);

		SceneObjectInstanced* blades = new SceneObjectInstanced(grassPositions, grassColors,
			grassIndices, grassNormals, instanceMatrixBuffer, *bladesShaderProgram);
		blades->model = patchSceneObject->model;
		scene.blades.push_back(blades);
	}
}

void generatePerlinNoise() {
	using namespace std::chrono;
	for (int i = 0; i < PERLIN_NOISE_TEXTURE_WIDTH * PERLIN_NOISE_TEXTURE_WIDTH; i++) seedTextureData[i] = (float)rand() / (float)RAND_MAX;

	//PerlinNoise2DCPU(PERLIN_NOISE_TEXTURE_WIDTH, PERLIN_NOISE_TEXTURE_WIDTH, scene.config.perlinConfig.octaves, scene.config.perlinConfig.bias, perlinNoiseTextureData, seedTextureData);

	// Upload texture to IMGUI
	perlinNoiseTexture.loadTextureData(perlinNoiseTextureData, PERLIN_NOISE_TEXTURE_WIDTH, PERLIN_NOISE_TEXTURE_WIDTH, GL_RED);

	PerlinNoise2DGPU(seedTexture, seedTextureData, computeShaderProgram, scene.perlinNoiseID, scene.config.perlinConfig.octaves, scene.config.perlinConfig.bias);
}

void initIMGUI(GLFWwindow* window) {
	// IMGUI init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

void createInstanceMatrixBuffer(glm::mat4* modelMatrices, const unsigned int numInstances) {
	GLCall(glGenBuffers(1, &instanceMatrixBuffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), modelMatrices, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

/* Draws the GUI that lets the user control the light, skybox, grass and wind.
 */
void drawGui() {
	// Slider will be 65% of the window width (this is the default)

	ImGui::NewFrame();
	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize({ 0, 0 });
	{
		ImGui::Begin("Settings");

		ImGui::Text("Perlin Noise Settings");

		ImGui::SliderInt("Octaves", &scene.config.perlinConfig.octaves, 2, 10);

		ImGui::SliderFloat("Bias", &scene.config.perlinConfig.bias, 0.2f, 2.0f);



		if (ImGui::Button("Generate")) {
			generatePerlinNoise();
		}

		ImGui::Image((ImTextureID)(long long)scene.perlinNoiseID, { PERLIN_NOISE_TEXTURE_WIDTH,PERLIN_NOISE_TEXTURE_WIDTH });

		ImGui::SliderFloat("Perlin Sample Scale", &scene.config.perlinSampleScale, 0.05f, 1.0f);

		ImGui::Text("Light Settings");
		ImGui::SliderFloat("Ambient Light Strength", &scene.config.ambientStrength, 0.1f, 1.0f);
		ImGui::DragFloat3("Light Position", (float*)&scene.config.lightPosition, 0.1f, -100, 100);
		ImGui::ColorEdit4("Light Color", (float*)&scene.config.lightColor);
		ImGui::SliderFloat("Light Intensity", &scene.config.lightIntensity, 0.0f, 10);
	
		ImGui::Separator();
		ImGui::Text("Skybox Settings");
		if (ImGui::RadioButton("Day", scene.config.skyboxType == SkyboxType::DAY)) { 
			scene.config.skyboxType = SkyboxType::DAY;
			scene.currentTexture = scene.cubemapTextureDay;
		} ImGui::SameLine();
		if (ImGui::RadioButton("Night", scene.config.skyboxType == SkyboxType::NIGHT)) {
			scene.config.skyboxType = SkyboxType::NIGHT;
			scene.currentTexture = scene.cubemapTextureNight;
		}

		ImGui::Separator();
		ImGui::Text("Grass Settings");

		ImGui::SliderInt("Number of patches", &scene.config.numPatches, 1, MAX_PATCHES);
		ImGui::SliderInt("Patch density", &scene.config.patchDensity, 0, MAX_PATCH_DENSITY_BLADES);
		//ImGui::InputInt("Patch density value:", &scene.config.patchDensity, 100, 1000);
		scene.config.patchDensity = glm::clamp(scene.config.patchDensity, 0, (int)MAX_PATCH_DENSITY_BLADES);

		ImGui::Text("Wind Settings");
		ImGui::SliderFloat("Sway Reach", &scene.config.swayReach, 0.01f, 0.3f);
		ImGui::SliderFloat("Wind Strength", &scene.config.windStrength, 0, 10);
		ImGui::SliderFloat2("Wind Direction", (float*)&scene.config.windDirection, -1, 1);
		ImGui::Separator();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SliderFloat3("Camera Position", (float*)&camera.camPosition, -50, 50);
		ImGui::SliderFloat("Yaw", &camera.yaw, -180, 180);
		ImGui::SliderFloat("Pitch", &camera.pitch, -180, 180);
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	camera.updateCameraVectors();
}

void cursorInRange(
	float screenX, float screenY, int screenW, int screenH, 
	float min, float max, float& x, float& y) 
{
	float sum = max - min;
	float xInRange = (float)screenX / (float)screenW * sum - sum / 2.0f;
	float yInRange = (float)screenY / (float)screenH * sum - sum / 2.0f;
	x = xInRange;
	y = -yInRange; // Flip screen space on the y-axis
}

/* GLFW: Whenever the window size changed (by OS or user resize) this
 * callback function executes.
 */
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// Makes sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	static bool tWasPressed = false;
	static bool rWasPressed = false;

	// Stop camera movement if GUI is opened
	if (isPaused)
		return;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.processKeyboard(cameraMovement::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.processKeyboard(cameraMovement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.processKeyboard(cameraMovement::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.processKeyboard(cameraMovement::RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		if (!tWasPressed) {
			camera.sprinting = !camera.sprinting;
			tWasPressed = true;
		}

	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
		tWasPressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		if (!rWasPressed) {
			bladesShaderProgram->reloadShaders();
			patchShaderProgram->reloadShaders();
			skyboxShaderProgram->reloadShaders();
			lightShaderProgram->reloadShaders();
			rWasPressed = true;
			
		}

	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) {
		rWasPressed = false;
	}
}

/* Processes the cursor input and passes it to the camera.
 *  Based on the tutorial on: https://learnopengl.com/Getting-started/Camera
 */
void cursorInputCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;
	lastX = (float)xpos;
	lastY = (float)ypos;
	// Stop camera movement if the GUI is opened
	if (isPaused)
		return;

	camera.processMouseMovement(xoffset, yoffset);
}

void keyInputCallback(GLFWwindow* window, int button, 
	int other, int action, int mods) 
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		isPaused = !isPaused;
		glfwSetInputMode(window, GLFW_CURSOR, 
			isPaused ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
}

void cleanUp() 
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// GLFW: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();

	delete bladesVertexShader;
	delete bladesFragmentShader;
	delete bladesShaderProgram;
	delete patchVertexShader;
	delete patchFragmentShader;
	delete patchShaderProgram;
	delete skyboxVertexShader;
	delete skyboxFragmentShader;
	delete skyboxShaderProgram;
	delete lightVertexShader;
	delete lightFragmentShader;
	delete lightShaderProgram;
}
