/* This application renders dynamic grass that can be influenced
 * by varying types of wind that can be configured by the user.
 *
 * This project was created as part of the Graphics Programming course
 * at the IT University of Copenhagen.
*/
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <vector>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
#include "fluid_grid.h"
#include "util.h"

// Constants
/**
 * \brief Initial screen width
 */
const unsigned int INIT_SCR_WIDTH = 1000;

/**
 * \brief Initial screen height
 */
const unsigned int INIT_SCR_HEIGHT = 1000;

/**
 * \brief Blades density
 */
const unsigned int MAX_PATCH_DENSITY_BLADES = 4000;

/**
 * \brief Max number patches
 */
const unsigned int MAX_PATCHES = 81;

/**
 * \brief The scene currently loaded (We don't support multiple scenes, but we
 * could with this.)
 */
Scene scene;

/**
 * \brief The patch creator
 */
Patch patch;

//
// Shaders
//

/**
 * \brief Vertex shader for the blades
*/
Shader *bladesVertexShader;

/**
 * \brief Fragment shader for the blades
*/
Shader *bladesFragmentShader;

/**
 * \brief Shader program for blades rendering
*/
ShaderProgram *bladesShaderProgram;

/**
 * \brief Vertex shader for the patch
*/
Shader *patchVertexShader;

/**
 * \brief Fragment shader for the patch.
 */
Shader *patchFragmentShader;

/**
 * \brief Shader program for patch rendering
*/
ShaderProgram *patchShaderProgram;

/**
 * \brief Vertex shader for the Skybox
*/
Shader *skyboxVertexShader;

/**
 * \brief Fragment shader for the skybox
*/
Shader *skyboxFragmentShader;

/**
 * \brief Shader program for Skybox rendering
*/
ShaderProgram *skyboxShaderProgram;

/**
 * \brief Vertex shader for the Light objects
*/
Shader *lightVertexShader;

/**
 * \brief Fragment shader for the blades
*/
Shader *lightFragmentShader;

/**
 * \brief Shader program for light object rendering
*/
ShaderProgram *lightShaderProgram;

/**
 * \brief Perlin noise compute shader
*/
Shader *perlinNoiseComputeShader;

/**
 * \brief Perlin noise compute shader program
*/
ShaderProgram *perlinNoiseComputeShaderProgram;


FluidGrid *fluidGrid;

/**
 * \brief Id of the instance matrix buffer for rendering blade-instances
*/
unsigned int instanceMatrixBuffer;

bool clearNextSimulate = true;
//
// Textures
//

/**
 * \brief Skybox day texture
*/
Texture *cubemapTextureDay;

/**
 * \brief Skybox night texture
 */
Texture *cubemapTextureNight;


/**
 * \brief Perlin noise texture
*/
Texture *perlinNoiseTexture;

/**
 * \brief PerlinSeed texture
*/
Texture *perlinNoiseSeedTexture;

/**
 * \brief Perlin noise texture data, used for uploading perlin noise data
*/
float *perlinNoiseTextureData;

/**
 * \brief Perlin noise seed texture data, used for uploading perlin noise
 * seed data.
*/
float *perlinNoiseSeedTextureData;

//
// Camera setup
//

/**
 * \brief The camera used for scene rendering
*/
Camera camera;


/**
 * \brief Used to stop camera movement when GUI is open
*/
bool isPaused = false;

/**
 * \brief Should the mouse start enabled?
*/
bool firstMouse = false;

/**
 * \brief mouse position X last frame. Initialized to center.
*/
float lastX = (float)INIT_SCR_WIDTH / 2.0;

/**
 * \brief mouse position Y last frame. Initialized to center.
*/
float lastY = (float)INIT_SCR_HEIGHT / 2.0;

//
// Timing 
//

/**
 * \brief Time between current frame and last frame
 */
float deltaTime = 0.0f;

/**
 * \brief Time of last frame
*/
float lastFrame = 0.0f;

/**
 * \brief Number of frames
*/
long numFrames = 0;

/**
 * \brief The window being rendered to
*/
GLFWwindow *window;

/**
 * \brief Initialize the window
 * \return Return the created window
*/
GLFWwindow *initGLFWWindow();

/**
 * \brief Initialize ImGUI
 * \param window Window to initialize for
*/
void initIMGUI(GLFWwindow *window);

/**
 * \brief Initialize shaders, textures, and the Z buffer
 */
void initShadersAndTextures();

/**
 * \brief Calculate the spiral position given an index. See this documentation
 * for details.
 * \details The spiral position is defined as starting in the middle, and
 * spiraling right-around as shown below:
 * 6 7 8
 * 5 0 1
 * 4 3 2
 * \param n Index to calculate position for
 * \return The coordinates of the calculated position
*/
glm::vec2 calculateSpiralPosition(int n);

/**
 * \brief Initialize the scene.
 * \param patch Patch object to use to creates patches.
*/
void initSceneObjects(Patch &patch);

/**
 * \brief Generate perlin noise.
*/
void generatePerlinNoise();

/**
 * @brief Creates the blades instance buffer.
 * @param modelMatrices The matrix data.
 * @param maxBlades The maximum number of blades.
*/
void createInstanceMatrixBuffer(glm::mat4 *modelMatrices, const unsigned int maxBlades);

/**
 * @brief Transfers the instance matrix buffer to the GPU.
 * @param modelMatrices The matrix data to transfer.
 * @param numInstances The number of instances.
*/
void transferInstanceMatrixBuffer(glm::mat4 *modelMatrices, const unsigned int numInstances);

/**
 * \brief Draws the GUI
*/
void drawGui();

/**
 * \brief Draws the tooltip
*/
void drawTooltip(const char *desc);

/**
 * @brief Restrict the cursor to a range.
 * @param screenX Cursor X position.
 * @param screenY Cursor Y Position.
 * @param screenW Width of the screen.
 * @param screenH Height of the screen.
 * @param min Minimum x and y value.
 * @param max Maximum x and y value.
 * @param x The restricted x.
 * @param y The restricted y.
*/
void cursorInRange(float screenX, float screenY, int screenW, int screenH, float min, float max, float &x, float &y);

/**
 * @brief The GLFW callback for screen resize. Resizes the viewport.
 * @param window The resized window.
 * @param width New width.
 * @param height New height.
*/
void framebufferSizeCallback(GLFWwindow *window, int width, int height);

/**
 * @brief Process input for a window
 * @param window Window to process input for
*/
void processInput(GLFWwindow *window);

/**
 * @brief GLFW key input callback
 *
 * @param window The window to process input for
 * @param button The pressed button
 * @param other Dunno
 * @param action Was the key pressed or released?
 * @param mods Any key modifiers?
*/
void keyInputCallback(GLFWwindow *window, int button, int other, int action, int mods);

/**
 * @brief GLFW cursor input callback
 * @param window Window to process input for.
 * @param posX The new x position of the cursor
 * @param posY The new y position of the cursor
*/
void cursorInputCallback(GLFWwindow *window, double posX, double posY);

/**
 * @brief Cleans up the scene.
*/
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

	// check OpenGL version
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	if (!(major >= 4 && minor >= 3)) {
		std::cout << "ERROR:: Wrong OpenGL version" << std::endl;
		return -1;
	}


	fluidGrid = new FluidGrid(128, 0, 0, 0.1f, &scene.config.fluidGridConfig);

	initShadersAndTextures();
	initSceneObjects(patch);
	generatePerlinNoise();

	// Initialize camera
	camera.camPosition = { 0, 20, 0 };
	camera.yaw = 0;
	camera.pitch = -89.0f;
	camera.updateCameraVectors();

	// Render loop : render every loopInterval seconds
	float loopInterval = 0.02f;

	// Set seed for random numbers
	srand((unsigned)time(0));

	initIMGUI(window);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

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

		if (clearNextSimulate)
		{
			fluidGrid->clearCurrent();
		}
		clearNextSimulate = true;
		fluidGrid->simulate();


		// Clear the color depth buffer (aka z-buffer) every new frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TODO: Check if this is needed
		bladesShaderProgram->use();

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		width = glm::max(width, 1);
		height = glm::max(height, 1);

		glm::mat4 projection = glm::perspective(70.0f,
			(float)width / (float)height, .01f, 1000.0f);
		glm::mat4 view = glm::lookAt(
			camera.getCamPosition(),
			camera.getCamPosition() + camera.getCamForward(), glm::vec3(0, 1, 0));
		scene.projection = projection;
		scene.view = view;
		scene.updateDynamic();
		scene.render();

		drawGui();

		glfwSwapBuffers(window);

		// Control render loop frequency
		float elapsed = deltaTime;
		while (loopInterval > elapsed) {
			elapsed = (float)glfwGetTime() - lastFrame;
		}
		lastFrame = currentFrame;
		numFrames++;
	}

	cleanUp();
	return 0;
}

GLFWwindow *initGLFWWindow() {
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
	GLFWwindow *window = glfwCreateWindow(INIT_SCR_WIDTH, INIT_SCR_HEIGHT, "GrassProject", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(0);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, cursorInputCallback);
	// Hide the cursor and capture it
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, keyInputCallback);

	return window;
}

void initShadersAndTextures() {
	bladesVertexShader = new Shader("assets/shaders/blades.vert", GL_VERTEX_SHADER);
	bladesFragmentShader = new Shader("assets/shaders/blades.frag", GL_FRAGMENT_SHADER);

	bladesShaderProgram = new ShaderProgram({ bladesVertexShader, bladesFragmentShader }, "BLADES SHADER");

	patchVertexShader = new Shader("assets/shaders/patch.vert", GL_VERTEX_SHADER);
	patchFragmentShader = new Shader("assets/shaders/patch.frag", GL_FRAGMENT_SHADER);

	patchShaderProgram = new ShaderProgram({ patchVertexShader, patchFragmentShader }, "PATCH SHADER");


	// Setup the Skybox Shaders
	skyboxVertexShader = new Shader("assets/shaders/skybox.vert", GL_VERTEX_SHADER);
	skyboxFragmentShader = new Shader("assets/shaders/skybox.frag", GL_FRAGMENT_SHADER);
	skyboxShaderProgram = new ShaderProgram({ skyboxVertexShader, skyboxFragmentShader }, "SKYBOX SHADER");

	cubemapTextureDay = new Texture("Day skybox", GL_TEXTURE_CUBE_MAP);
	cubemapTextureDay->loadTextureCubeMap(facesDay, false);

	cubemapTextureNight = new Texture("Night skybox", GL_TEXTURE_CUBE_MAP);
	cubemapTextureNight->loadTextureCubeMap(facesNight);

	lightVertexShader = new Shader("assets/shaders/light.vert", GL_VERTEX_SHADER);
	lightFragmentShader = new Shader("assets/shaders/light.frag", GL_FRAGMENT_SHADER);
	lightShaderProgram = new ShaderProgram({ lightVertexShader, lightFragmentShader }, "LIGHT SHADER");

	scene.config.perlinConfig.texture = new Texture("Perlin Noise", GL_TEXTURE_2D);
	scene.config.perlinConfig.texture->loadTextureSingleChannel(PERLIN_NOISE_TEXTURE_WIDTH);

	perlinNoiseComputeShader = new Shader("assets/shaders/perlin_noise.comp", GL_COMPUTE_SHADER);
	perlinNoiseComputeShaderProgram = new ShaderProgram({ perlinNoiseComputeShader }, "PERLIN NOISE COMPUTE SHADER");

	scene.cubemapTextureDay = cubemapTextureDay;
	scene.cubemapTextureNight = cubemapTextureNight;
	scene.currentSkyboxTexture = scene.cubemapTextureNight;

	scene.config.windX = scene.config.fluidGridConfig.velX;
	scene.config.windY = scene.config.fluidGridConfig.velY;

	int width = PERLIN_NOISE_TEXTURE_WIDTH;
	int height = PERLIN_NOISE_TEXTURE_WIDTH;

	// Should this not be set to random?
	perlinNoiseSeedTextureData = new float[width * height];
	perlinNoiseSeedTexture = new Texture("Perlin Seed Texture", GL_TEXTURE_2D);
	perlinNoiseSeedTexture->generateTexture(perlinNoiseSeedTextureData, width, height, GL_RED);

	// Set up the z-buffer
	glDepthRange(-1, 1);  // Make the NDC a right handed coordinate system, 
						  // with the camera pointing towards -z
	glEnable(GL_DEPTH_TEST);  // Turn on z-buffer depth perlinNoiseTexture
	glDepthFunc(GL_LESS);  // Draws fragments that are closer to the screen in NDC
	glEnable(GL_MULTISAMPLE);

}

glm::vec2 calculateSpiralPosition(int n) {
	n++;
	int k = (int)std::ceil((std::sqrt(n) - 1.0) / 2.0);
	int t = 2 * k + 1;
	int m = t * t;

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

void initSceneObjects(Patch &patch) {

	patch.init(MAX_PATCH_DENSITY_BLADES, patchShaderProgram);

	if (scene.config.bladeDistribution == BladeDistribution::HARRY_STYLES_WITH_RANDOS) {
		patch.initHarryEdwardStylesBladeMatrices();
	}
	else if (scene.config.bladeDistribution == BladeDistribution::HARRY_STYLES) {
		patch.initHarryEdwardStylesBladeMatrices(false);
	}
	else if (scene.config.bladeDistribution == BladeDistribution::ONE_DIRECTION) {
		patch.initOneDirectionBladeMatrices();
	}

	createInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_PATCH_DENSITY_BLADES);

	SceneObjectArrays *skybox = new SceneObjectArrays(cubePositions, *skyboxShaderProgram);
	scene.sceneObjects.push_back(skybox);

	SceneObjectArrays *light = new SceneObjectArrays(cubePositions, *lightShaderProgram);
	scene.sceneObjects.push_back(light);
	scene.light = light;
	scene.light->isVisible = false;

	for (int i = 0; i < MAX_PATCHES; i++) {

		glm::vec2 position = calculateSpiralPosition(i) * PATCH_SIZE;
		SceneObjectIndexed *patchSceneObject = new SceneObjectIndexed(grassPatchPositions, grassPatchColors,
			grassPatchIndices, grassPatchNormals, *patchShaderProgram, &grassPatchUVs);
		glm::mat4 translation = glm::translate(position.x - 0.5f * PATCH_SIZE, 0, position.y - 0.5f * PATCH_SIZE);
		patchSceneObject->model = translation * glm::scale(PATCH_SIZE, PATCH_SIZE, PATCH_SIZE);
		scene.patches.push_back(patchSceneObject);

		SceneObjectInstanced *blades = new SceneObjectInstanced(grassPositions, grassColors,
			grassIndices, grassNormals, instanceMatrixBuffer, *bladesShaderProgram, &grassUVs);
		// Do not scale the blades
		blades->model = translation;
		scene.blades.push_back(blades);
	}
}

void generatePerlinNoise() {
	using namespace std::chrono;
	// Initialize seed data
	for (int i = 0; i < PERLIN_NOISE_TEXTURE_WIDTH * PERLIN_NOISE_TEXTURE_WIDTH; i++) perlinNoiseSeedTextureData[i] = (float)rand() / (float)RAND_MAX;

	PerlinNoise2DGPU(*perlinNoiseSeedTexture, perlinNoiseSeedTextureData, perlinNoiseComputeShaderProgram, scene.config.perlinConfig.texture->getTextureID(), scene.config.perlinConfig.octaves, scene.config.perlinConfig.bias, scene.config.perlinConfig.makeChecker);

	// Upload texture to IMGUI
	perlinNoiseTexture = new Texture("Perlin Texture", GL_TEXTURE_2D);
	perlinNoiseTexture->loadTextureData(perlinNoiseTextureData, PERLIN_NOISE_TEXTURE_WIDTH, PERLIN_NOISE_TEXTURE_WIDTH, GL_RED);
}

void initIMGUI(GLFWwindow *window) {
	// IMGUI init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGuiIO &io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

void createInstanceMatrixBuffer(glm::mat4 *modelMatrices, const unsigned int numInstances) {
	GLCall(glGenBuffers(1, &instanceMatrixBuffer));
	transferInstanceMatrixBuffer(modelMatrices, numInstances);
}

void transferInstanceMatrixBuffer(glm::mat4 *modelMatrices, const unsigned int numInstances) {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), modelMatrices, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void drawGui() {
	// Slider will be 65% of the window width (this is the default)
	ImGui::NewFrame();
	//ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize({ 0, 0 });
	{
		if (scene.config.simulationMode == SimulationMode::FLUID_GRID)
		{
			ImGui::Begin("Fluid Grid");
			static glm::vec2 pos = { 0.5f, 0.5f };
			static glm::vec2 vel = {};
			static float den = {};
			ImGui::DragFloat2("Position", (float *)&pos, 0.05f, 0, 1);
			ImGui::InputFloat("Density", &den);
			ImGui::InputFloat2("Velocity", (float *)&vel);
			if (clearNextSimulate)
			{
				fluidGrid->clearCurrent();
			}

			if (ImGui::Button("Add Density"))
			{
				fluidGrid->addDensityAt(pos.x * fluidGrid->getN(), pos.y * fluidGrid->getN(), den);
				clearNextSimulate = false;
			}
			if (ImGui::Button("Add Velocity"))
			{
				clearNextSimulate = false;
				fluidGrid->addVelocityAt(pos.x * fluidGrid->getN(), pos.y * fluidGrid->getN(), vel.x, vel.y);
			}
			if (ImGui::Button("Add Random"))
			{
				clearNextSimulate = false;
				float x = generateRandomNumber(0, (float)fluidGrid->getN());
				float y = generateRandomNumber(0, (float)fluidGrid->getN());
				float d = generateRandomNumber(0, 10000);
				float vx = generateRandomNumber(-1000, 1000);
				float vy = generateRandomNumber(-1000, 1000);

				fluidGrid->addVelocityAt(x, y, vx, vy);
				fluidGrid->addDensityAt(x, y, d);
			}
			ImGui::End();
		}

		ImGui::Begin("Settings");

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS) Framecount %lld Time %.0f", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate, numFrames, glfwGetTime());

		if (ImGui::CollapsingHeader("Camera Settings"))
		{
			ImGui::SliderFloat3("Camera Position", (float *)&camera.camPosition, -50, 50);
			ImGui::SliderFloat("Yaw", &camera.yaw, -180, 180);
			ImGui::SliderFloat("Pitch", &camera.pitch, -89.9f, 89.9f);
		}

		if (ImGui::CollapsingHeader("Grass Settings"))
		{

			ImGui::SliderInt("Number of patches", &scene.config.numPatches, 1, MAX_PATCHES);
			ImGui::SliderInt("Patch density", &scene.config.patchDensity, 0, MAX_PATCH_DENSITY_BLADES);
			scene.config.patchDensity = glm::clamp(scene.config.patchDensity, 0, (int)MAX_PATCH_DENSITY_BLADES);
		}

		if (ImGui::CollapsingHeader("Wind Settings"))
		{
			ImGui::SliderFloat("Sway Reach", &scene.config.swayReach, 0.0f, 1.0f);
			drawTooltip("How far the blades will move in the wind.");
			ImGui::SliderFloat("Wind Strength", &scene.config.windStrength, 0, 0.5f);
			ImGui::DragFloat2("Wind Direction", (float *)&scene.config.windDirection,
				0.1f, -1.0f, 1.0f);
			if (ImGui::Button("Normalize Wind Direction"))
				scene.config.windDirection = glm::normalize(scene.config.windDirection);

		}

		if (ImGui::CollapsingHeader("Debug Settings"))
		{
			ImGui::Text("Simulation Mode Settings");
			if (ImGui::RadioButton("Perlin Noise", scene.config.simulationMode == SimulationMode::PERLIN_NOISE)) {
				scene.config.simulationMode = SimulationMode::PERLIN_NOISE;
				scene.config.perlinConfig.makeChecker = false;
				generatePerlinNoise();

				scene.config.windX = scene.config.perlinConfig.texture;
				scene.config.windY = nullptr;

			} ImGui::SameLine();
			drawTooltip("Blades respond to the generated perlin noise.");
			if (ImGui::RadioButton("Checker Pattern", scene.config.simulationMode == SimulationMode::CHECKER_PATTERN)) {
				scene.config.simulationMode = SimulationMode::CHECKER_PATTERN;
				scene.config.perlinConfig.makeChecker = true;
				generatePerlinNoise();

				scene.config.windX = scene.config.perlinConfig.texture;
				scene.config.windY = nullptr;

			} ImGui::SameLine();
			drawTooltip("Blades respond to the generated checker pattern.");
			if (ImGui::RadioButton("Fluid Grid", scene.config.simulationMode == SimulationMode::FLUID_GRID)) {
				scene.config.simulationMode = SimulationMode::FLUID_GRID;

				scene.config.windX = scene.config.fluidGridConfig.velX;
				scene.config.windY = scene.config.fluidGridConfig.velY;

			}
			drawTooltip("Blades respond to the fluid grid simulation.");


			ImGui::Text("Harry Styles Settings");
			if (ImGui::RadioButton("Harry Styles With Randos", scene.config.bladeDistribution == BladeDistribution::HARRY_STYLES_WITH_RANDOS)) {
				scene.config.bladeDistribution = BladeDistribution::HARRY_STYLES_WITH_RANDOS;
				patch.initHarryEdwardStylesBladeMatrices();
				transferInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_PATCH_DENSITY_BLADES);
			} ImGui::SameLine();
			drawTooltip("Blades are placed uniformly on the patch with random rotations.");
			if (ImGui::RadioButton("Harry Styles", scene.config.bladeDistribution == BladeDistribution::HARRY_STYLES)) {
				scene.config.bladeDistribution = BladeDistribution::HARRY_STYLES;
				patch.initHarryEdwardStylesBladeMatrices(false);
				transferInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_PATCH_DENSITY_BLADES);
			} ImGui::SameLine();
			drawTooltip("Blades are placed uniformly on the patch without random rotations.");
			if (ImGui::RadioButton("One Direction", scene.config.bladeDistribution == BladeDistribution::ONE_DIRECTION)) {
				scene.config.bladeDistribution = BladeDistribution::ONE_DIRECTION;
				patch.initOneDirectionBladeMatrices();
				transferInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_PATCH_DENSITY_BLADES);
			}
			drawTooltip("Blades are placed in a line in the middle of the patch without random rotations.");

			ImGui::Checkbox("Debug Blades", &scene.config.debugBlades);
			ImGui::Checkbox("Visualize Texture On Patch", &scene.config.visualizeTexture);

			if (scene.config.simulationMode == SimulationMode::FLUID_GRID && scene.config.visualizeTexture)
			{
				ImGui::Text("Visualize");
				if (ImGui::RadioButton("Density", scene.config.fluidGridConfig.visualizeDensity))
				{
					scene.config.fluidGridConfig.visualizeDensity = true;
					if(scene.config.windX) scene.config.windX->unbind();
					if(scene.config.windY) scene.config.windY->unbind();

					scene.config.windX = scene.config.fluidGridConfig.density;
					scene.config.windY = nullptr;
				}
				if (ImGui::RadioButton("Velocity", !scene.config.fluidGridConfig.visualizeDensity))
				{
					scene.config.fluidGridConfig.visualizeDensity = false;
					if(scene.config.windX) scene.config.windX->unbind();
					if(scene.config.windY) scene.config.windY->unbind();

					scene.config.windX = scene.config.fluidGridConfig.velX;
					scene.config.windY = scene.config.fluidGridConfig.velY;
				}
			}
		}

		if (scene.config.simulationMode == SimulationMode::PERLIN_NOISE && ImGui::CollapsingHeader("Perlin Noise Settings"))
		{

			ImGui::SliderInt("Octaves", &scene.config.perlinConfig.octaves, 2, 10);
			drawTooltip("Octaves for fun.");

			ImGui::SliderFloat("Bias", &scene.config.perlinConfig.bias, 0.2f, 2.0f);
			drawTooltip("Bias for fun.");



			if (ImGui::Button("Generate Perlin Noise")) {
				generatePerlinNoise();
			}
			float width = PERLIN_NOISE_TEXTURE_WIDTH;

			ImGui::Image((ImTextureID)(long long)scene.config.perlinConfig.texture->getTextureID(), 
				{ width, width }, 
				{ 0.0f,scene.config.perlinConfig.textureScale }, 
				{ scene.config.perlinConfig.textureScale, 0.0f });

			ImGui::SliderFloat("Perlin Sample Scale", &scene.config.perlinConfig.textureScale, 0.05f, 1.0f);
			drawTooltip("Will zoom in or out of the perlin noise texture when sampling it.");
		}

		if (scene.config.simulationMode == SimulationMode::FLUID_GRID && ImGui::CollapsingHeader("Fluid Grid Settings"))
		{
			float width = 512;
			
			ImGui::Image((ImTextureID)(long long)fluidGrid->getTextureDen()->getTextureID(),
				{ width, width }, 
				{ 0.0f, 1 }, 
				{ 1.0f, 0 });


			if (ImGui::Button("Step through fluid simulation")) {
				if (clearNextSimulate)
				{
					fluidGrid->clearCurrent();
				}
				clearNextSimulate = true;
				fluidGrid->simulate();
			}

			if (ImGui::Button("Step through fluid simulation fasssst")) {
				if (clearNextSimulate)
				{
					fluidGrid->clearCurrent();
				}
				clearNextSimulate = true;

				for (int i = 0; i < 9; i++)
				{
					fluidGrid->simulate();
					fluidGrid->clearCurrent();
				}
			}

			if (ImGui::Button("Reset")) {
				fluidGrid->initialize();
			}

			ImGui::DragFloat("Diffusion", fluidGrid->getDiffPointer(), 0.005f, 0.0f, 0.1f);
			ImGui::DragFloat("Viscosity", fluidGrid->getViscPointer(), 0.0001f, 0.0f, 0.005f);
		}

		if (ImGui::CollapsingHeader("Light Settings"))
		{
			ImGui::Checkbox("Show Light", &scene.light->isVisible);
			ImGui::SliderFloat("Ambient Light Strength", &scene.config.ambientStrength, 0.0f, 1.0f);
			ImGui::DragFloat3("Light Position", (float *)&scene.config.lightPosition, 0.1f, -100, 100);
			ImGui::ColorEdit4("Light Color", (float *)&scene.config.lightColor);
			ImGui::SliderFloat("Light Intensity", &scene.config.lightIntensity, 0.0f, 10);

			ImGui::Separator();

			ImGui::Text("Skybox Settings");
			if (ImGui::RadioButton("Day", scene.config.skyboxType == SkyboxType::DAY)) {
				scene.config.skyboxType = SkyboxType::DAY;
				scene.currentSkyboxTexture = scene.cubemapTextureDay;
			} ImGui::SameLine();
			if (ImGui::RadioButton("Night", scene.config.skyboxType == SkyboxType::NIGHT)) {
				scene.config.skyboxType = SkyboxType::NIGHT;
				scene.currentSkyboxTexture = scene.cubemapTextureNight;
			}
		}

		if (ImGui::CollapsingHeader("Controls"))
		{
			ImGui::Text("WASD for camera movement");
			ImGui::Text("T for sprint toggle");
			ImGui::Text("R to reload shaders");
			ImGui::Text("ESC to close");
		}

		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	camera.updateCameraVectors();

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow *backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

}

void drawTooltip(const char *desc)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void cursorInRange(
	float screenX, float screenY, int screenW, int screenH,
	float min, float max, float &x, float &y)
{
	float sum = max - min;
	float xInRange = (float)screenX / (float)screenW * sum - sum / 2.0f;
	float yInRange = (float)screenY / (float)screenH * sum - sum / 2.0f;
	x = xInRange;
	y = -yInRange; // Flip screen space on the y-axis
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	// Makes sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
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

	//// For debugging (so we can see if something happens!)
	//if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
	//	
	//}

}

/* Processes the cursor input and passes it to the camera.
 *  Based on the tutorial on: https://learnopengl.com/Getting-started/Camera
 */
void cursorInputCallback(GLFWwindow *window, double xpos, double ypos)
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

void keyInputCallback(GLFWwindow *window, int button,
	int other, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		isPaused = !isPaused;
		glfwSetInputMode(window, GLFW_CURSOR,
			glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
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
