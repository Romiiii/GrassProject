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

#include "shader.h"
#include "glmutils.h"
#include "primitives.h"
#include "camera.h"
#include "scene_object.h"
#include "debug.h"
#include "patch.h"
#include "scene.h"
#include "scene_object_instanced.h"
#include "scene_object_arrays.h"

// Constants
const unsigned int INIT_SCR_WIDTH = 1000;
const unsigned int INIT_SCR_HEIGHT = 1000;
// Maximum amount of grass blades per patch
const unsigned int MAX_PATCH_DENSITY_BLADES = 40000;
const unsigned int MAX_PATCHES = 81;

Scene scene;

Patch patch;

glm::mat4 lightMatrix;

Shader bladesShader;
Shader patchShader;
Shader skyboxShader;
Shader lightShader;
unsigned int instanceMatrixBuffer;


Texture billboardGrassNoise1;
Texture billboardGrassNoise2;

Texture cubemapTextureDay;
Texture cubemapTextureNight;

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
void createInstanceMatrixBuffer(glm::mat4* modelMatrices, const unsigned int MAX_PATCH_DENSITY_BLADES);

void drawGui();

void cursorInRange(float screenX, float screenY, int screenW, int screenH, float min, float max, float& x, float& y);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void keyInputCallback(GLFWwindow* window, int button, int other, int action, int mods);
void cursorInputCallback(GLFWwindow* window, double posX, double posY);

int main()
{
	window = initGLFWWindow();
	assert(window != NULL, "ERROR:: Failed to create GLFW window");

	// GLAD: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	initShadersAndTextures();
	initSceneObjects(patch);

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
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;

		processInput(window);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		// Clear the depth buffer (aka z-buffer) every new frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bladesShader.use();

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

		if (isPaused) {
			drawGui();
		}

		glfwSwapBuffers(window);

		// Control render loop frequency
		float elapsed = deltaTime;
		while (loopInterval > elapsed) {
			elapsed = glfwGetTime() - lastFrame;
		}
		lastFrame = currentFrame;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// GLFW: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

GLFWwindow* initGLFWWindow() {
	// GLFW: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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
	// Initialize shader
	bladesShader.initialize("assets/shaders/blades.vert", "assets/shaders/blades.frag");

	patchShader.initialize("assets/shaders/patch.vert", "assets/shaders/patch.frag");

	std::string billboardGrassFileNameNoise1 = "assets/textures/misc/perlin_noise_1.tga";
	std::string billboardGrassFileNameNoise2 = "assets/textures/misc/perlin_noise_2.tga";
	billboardGrassNoise1.loadTexture(billboardGrassFileNameNoise1, false);
	billboardGrassNoise2.loadTexture(billboardGrassFileNameNoise2, false);


	// Setup the Skybox Shaders
	skyboxShader.initialize("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
	//skyboxShader.use();
	cubemapTextureDay.loadTextureCubeMap(facesDay, false);
	cubemapTextureNight.loadTextureCubeMap(facesNight);

	lightShader.initialize("assets/shaders/light.vert", "assets/shaders/light.frag");

	scene.cubemapTextureDay = &cubemapTextureDay;
	scene.cubemapTextureNight = &cubemapTextureNight;
	scene.currentTexture = scene.cubemapTextureNight;
	// Set up the z-buffer
	glDepthRange(-1, 1);  // Make the NDC a right handed coordinate system, 
						  // with the camera pointing towards -z
	glEnable(GL_DEPTH_TEST);  // Turn on z-buffer depth test
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

	patch.init(MAX_PATCH_DENSITY_BLADES, &patchShader);
	createInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_PATCH_DENSITY_BLADES);

	SceneObjectArrays* skybox = new SceneObjectArrays(cubePositions, skyboxShader);
	scene.sceneObjects.push_back(skybox);

	SceneObjectArrays* light = new SceneObjectArrays(cubePositions, lightShader);
	scene.sceneObjects.push_back(light);
	scene.light = light;

	for (int i = 0; i < MAX_PATCHES; i++) {
		
		glm::vec2 position = calculateSpiralPosition(i) * 10.0f;
		SceneObjectIndexed* patchSceneObject = new SceneObjectIndexed(grassPatchPositions, grassPatchColors,
			grassPatchIndices, grassPatchNormals, patchShader);
		patchSceneObject->model = glm::translate(position.x, 0, position.y);
		scene.patches.push_back(patchSceneObject);

		SceneObjectInstanced* blades = new SceneObjectInstanced(grassPositions, grassColors,
			grassIndices, grassNormals, instanceMatrixBuffer, bladesShader);
		blades->model = patchSceneObject->model;
		scene.blades.push_back(blades);
	}
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

		ImGui::Text("Light Settings");
		ImGui::SliderFloat("Ambient Light Strength", &scene.config.ambientStrength, 0.1, 1.0);
		ImGui::DragFloat3("Light Position", (float*)&scene.config.lightPosition, 0.1, -100, 100);
		ImGui::ColorEdit4("Light Color", (float*)&scene.config.lightColor);
		ImGui::SliderFloat("Light Intensity", &scene.config.lightIntensity, 0.0, 10.0);
	
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
		ImGui::SliderInt("Patch density", &scene.config.patchDensity, 1, MAX_PATCH_DENSITY_BLADES);
		//ImGui::InputInt("Patch density value:", &scene.config.patchDensity, 100, 1000);
		scene.config.patchDensity = glm::clamp(scene.config.patchDensity, 0, (int)MAX_PATCH_DENSITY_BLADES);

		ImGui::Text("Wind Settings");
		ImGui::SliderFloat("Sway Reach", &scene.config.swayReach, 0.01, 0.3);
		ImGui::SliderFloat("Wind Strength", &scene.config.windStrength, 0.0, 10.0);
		ImGui::SliderFloat2("Wind Direction", (float*)&scene.config.windDirection, -1.0, 1.0);
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

void cursorInRange(float screenX, float screenY, int screenW, int screenH, float min, float max, float& x, float& y) {
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

void processInput(GLFWwindow* window) {
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
			bladesShader.compile();
			patchShader.compile();
			skyboxShader.compile();
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
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	// Stop camera movement if the GUI is opened
	if (isPaused)
		return;

	camera.processMouseMovement(xoffset, yoffset);
}

void keyInputCallback(GLFWwindow* window, int button, int other, int action, int mods) {

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		isPaused = !isPaused;
		glfwSetInputMode(window, GLFW_CURSOR, isPaused ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
}