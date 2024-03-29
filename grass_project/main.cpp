/* This application renders dynamic grass that can be influenced
 * by varying types of wind that can be configured by the user.
 *
 * This project was created as part of the Graphics Programming course
 * at the IT University of Copenhagen.
*/

/*
NICE VALUES:
Fan:
	Active: true
	Position: 0.2, 0.2
	Density: 250
	Velocity: 300, 0

Diffusion Ratio: 0.56
Viscosity: 0
Velocity: 2.7
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
#include "grass_math.h"

#define CHECKER_PATTERN_TEXTURE_WIDTH 512

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
const int MAX_BLADES_PER_PATCH = 4000;

/**
 * \brief Max number patches
 */
const int MAX_PATCHES = 81;

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
Shader* bladesVertexShader;

/**
 * \brief Fragment shader for the blades
*/
Shader* bladesFragmentShader;

/**
 * \brief Shader program for blades rendering
*/
ShaderProgram* bladesShaderProgram;

/**
 * \brief Vertex shader for the patch
*/
Shader* patchVertexShader;

/**
 * \brief Fragment shader for the patch.
 */
Shader* patchFragmentShader;

/**
 * \brief Shader program for patch rendering
*/
ShaderProgram* patchShaderProgram;

/**
 * \brief Vertex shader for the Skybox
*/
Shader* skyboxVertexShader;

/**
 * \brief Fragment shader for the skybox
*/
Shader* skyboxFragmentShader;

/**
 * \brief Shader program for Skybox rendering
*/
ShaderProgram* skyboxShaderProgram;

/**
 * \brief Vertex shader for the Light objects
*/
Shader* lightVertexShader;

/**
 * \brief Fragment shader for the blades
*/
Shader* lightFragmentShader;

/**
 * \brief Shader program for light object rendering
*/
ShaderProgram* lightShaderProgram;

/**
 * \brief Perlin noise compute shader
*/
Shader* perlinNoiseComputeShader;

/**
 * \brief Perlin noise compute shader program
*/
ShaderProgram* perlinNoiseComputeShaderProgram;

/**
 * \brief Checker pattern compute shader
*/
Shader* checkerPatternComputeShader;

/**
 * \brief Checker pattern compute shader program
*/
ShaderProgram* checkerPatternComputeShaderProgram;

/**
 * \brief Fluid grid
*/
FluidGrid* fluidGrid;

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
Texture* cubemapTextureDay;

/**
 * \brief Skybox night texture
 */
Texture* cubemapTextureNight;


/**
 * \brief Checker pattern texture
*/
Texture* checkerPatternTexture;

/**
 * \brief Perlin noise texture
*/
Texture* perlinNoiseTexture;


/**
 * \brief PerlinSeed texture
*/
Texture* perlinNoiseSeedTexture;

/**
 * \brief Perlin noise texture data, used for uploading perlin noise data
*/
float* perlinNoiseTextureData;

/**
 * \brief Perlin noise seed texture data, used for uploading perlin noise
 * seed data.
*/
float* perlinNoiseSeedTextureData;

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
 * \brief Time between current frame and last frame
 */
float gameDeltaTime = 0.0f;

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
GLFWwindow* window;

/**
 * \brief Initialize the window
 * \return Return the created window
*/
GLFWwindow* initGLFWWindow();

/**
 * \brief Initialize ImGUI
 * \param window Window to initialize for
*/
void initIMGUI(GLFWwindow* window);

/**
 * \brief Initialize shaders, textures, and the Z buffer
 */
void initShadersAndTextures();

/**
 * \brief Generates a checker pattern texture.
 */
void generateCheckerPatternTexture();

/**
* Generates a checker pattern on the GPU.
*/
void checkerPattern2DGPU(ShaderProgram* computeShaderProgram, GLuint computeShaderTexture, int checkerSize);

/**
 * \brief Set the wind textures based on the simulation mode that is used.
 */
void setWindTexturesForSimulationMode();

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
void initSceneObjects(Patch& patch);

/**
 * \brief Generate perlin noise.
*/
void generatePerlinNoiseTexture();

/**
 * @brief Creates the blades instance buffer.
 * @param modelMatrices The matrix data.
 * @param maxBlades The maximum number of blades.
*/
void createInstanceMatrixBuffer(glm::mat4* modelMatrices, const unsigned int maxBlades);

/**
 * @brief Transfers the instance matrix buffer to the GPU.
 * @param modelMatrices The matrix data to transfer.
 * @param numInstances The number of instances.
*/
void transferInstanceMatrixBuffer(glm::mat4* modelMatrices, const unsigned int numInstances);

glm::vec2 getNormalizedMousePos();

Ray getMouseRay();

std::optional<glm::vec3> mouseHitsGround();

/**
 * \brief Draws the GUI
*/
void drawGui();

/**
 * \brief Draws the tooltip
*/
void drawTooltip(const char* desc);

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
void cursorInRange(float screenX, float screenY, int screenW, int screenH, float min, float max, float& x, float& y);

/**
 * @brief The GLFW callback for screen resize. Resizes the viewport.
 * @param window The resized window.
 * @param width New width.
 * @param height New height.
*/
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

/**
 * @brief Process input for a window
 * @param window Window to process input for
*/
void processInput(GLFWwindow* window);

/**
 * @brief GLFW key input callback
 *
 * @param window The window to process input for
 * @param button The pressed button
 * @param other Dunno
 * @param action Was the key pressed or released?
 * @param mods Any key modifiers?
*/
void keyInputCallback(GLFWwindow* window, int button, int other, int action, int mods);

/**
 * @brief GLFW cursor input callback
 * @param window Window to process input for.
 * @param posX The new x position of the cursor
 * @param posY The new y position of the cursor
*/
void cursorInputCallback(GLFWwindow* window, double posX, double posY);

/**
 * @brief Cleans up the scene.
*/
void cleanUp();


// INPUT STUFF
bool g_ctrlIsPressed = false;

glm::vec3 g_fanDragStart = {};
bool g_fanIsDragging = false;

void setWorldMinMax()
{
	/* Since we're making a spiral pattern, we can find the leftmost edge by taking the sqrt, giving us the width.
		 Dividing that in two, so we have what would be to either side, taking the floor of that, since we know we
		 start at (0, 0), so the left will have fewer patches for odd widths. We then negate it, because it's in the
		 negative halfspace.
	 */
	scene.config.worldMin = (float)(-floor(sqrt(MAX_PATCHES) / 2) * (double)scene.config.patchSize);

	/* The process for the max is similar, though we take the ceiling rather than the floor. Because since we start at (0, 0) there are more
		 patches to the right.
	 */
	scene.config.worldMax = (float)(ceil(sqrt(MAX_PATCHES) / 2) * (double)scene.config.patchSize);
}

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

	if (!(major >= 4 && minor >= 3))
	{
		std::cout << "ERROR:: Wrong OpenGL version" << std::endl;
		return -1;
	}

	fluidGrid = new FluidGrid(128, 0, 0, &scene.config.fluidGridConfig);

	initShadersAndTextures();
	initSceneObjects(patch);

	if (scene.config.simulationMode == SimulationMode::CHECKER_PATTERN)
	{
		generateCheckerPatternTexture();
	}
	else if (scene.config.simulationMode == SimulationMode::PERLIN_NOISE)
	{
		generatePerlinNoiseTexture();
	}


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

	scene.config.isPaused = false;
	scene.config.currentTime = (float)glfwGetTime();
	setWorldMinMax();

	float worldWidth = scene.config.worldMax - scene.config.worldMin;
	float center = (scene.config.worldMax + scene.config.worldMin) / 2.0f;
	scene.worldRekt.center = { center, 0.0f, center };
	scene.worldRekt.height = scene.worldRekt.width = worldWidth;

	Fan fan{};
	fan.active = true;
	fan.position.x = 0.2f;
	fan.position.y = 0.2f;
	fan.density = 250.0f;
	fan.velocity.x = 150.0f;
	fan.velocity.y = 0.0f;
	scene.config.fluidGridConfig.selectedFanIndex = 0;

	scene.config.fluidGridConfig.fans.push_back(fan);

	// Set num blades per patch
	if (scene.config.numBladesPerPatch < 0)
	{
		scene.config.numBladesPerPatch = MAX_BLADES_PER_PATCH;
	}

	if (scene.config.numBladesPerPatch > MAX_BLADES_PER_PATCH)
	{
		std::cout << "[WARNING]: Num blades per patch should be between 0 and " << MAX_BLADES_PER_PATCH << " but was " << scene.config.numBladesPerPatch << ". Clamping." << std::endl;
		scene.config.numBladesPerPatch = glm::clamp(scene.config.numBladesPerPatch, 0, MAX_BLADES_PER_PATCH);
	}

	if (scene.config.numPatches < 0)
	{
		scene.config.numPatches = MAX_PATCHES;
	}
	if (scene.config.numPatches > MAX_PATCHES)
	{
		std::cout << "[WARNING]: Num patches should be between 0 and " << MAX_PATCHES << " but was " << scene.config.numPatches << ". Clamping." << std::endl;
		scene.config.numPatches = glm::clamp(scene.config.numPatches, 0, MAX_PATCHES);
	}


	while (!glfwWindowShouldClose(window))
	{
		if (!scene.config.isPaused)
		{
			scene.config.currentTime = (float)glfwGetTime();
		}

		// Poll events at start so you have the newest inputs
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		// For correcting the camera input in processInput
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		gameDeltaTime = deltaTime;

		if (scene.config.isPaused)
		{
			gameDeltaTime = 0;
		}

		processInput(window);

		if (clearNextSimulate)
		{
			fluidGrid->clearCurrent();
		}

		// FAN!
		for (size_t fanIndex = 0; fanIndex < scene.config.fluidGridConfig.fans.size(); fanIndex++)
		{
			Fan& fan = scene.config.fluidGridConfig.fans[fanIndex];
			if (!fan.active)
			{
				continue;
			}

			int x = (int)(fluidGrid->getN() * fan.position.x);
			int y = (int)(fluidGrid->getN() * fan.position.y);

			fluidGrid->addVelocityAt(x, y, fan.velocity.x, -fan.velocity.y);
			fluidGrid->addDensityAt(x, y, fan.density);
		}

		clearNextSimulate = true;

		if (!scene.config.isPaused)
		{
			fluidGrid->simulate(gameDeltaTime);
		}

		// Clear the color depth buffer (aka z-buffer) every new frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TODO: Check if this is needed
		bladesShaderProgram->use();

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		width = glm::max(width, 1);
		height = glm::max(height, 1);

		glm::mat4 projection = glm::perspective(70.0f, (float)width / (float)height, .01f, 1000.0f);
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
		while (loopInterval > elapsed)
		{
			elapsed = (float)glfwGetTime() - lastFrame;
		}
		lastFrame = currentFrame;
		numFrames++;
		g_debugStrings.clear();
	}

	cleanUp();
	return 0;
}

GLFWwindow* initGLFWWindow()
{
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

	glfwSwapInterval(0);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, cursorInputCallback);
	// Hide the cursor and capture it
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, keyInputCallback);

	return window;
}

void initShadersAndTextures()
{
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

	scene.config.checkerPatternTexture = new Texture("Checker Pattern", GL_TEXTURE_2D);
	scene.config.checkerPatternTexture->loadTextureSingleChannel(CHECKER_PATTERN_TEXTURE_WIDTH);

	checkerPatternComputeShader = new Shader("assets/shaders/checker_pattern.comp", GL_COMPUTE_SHADER);
	checkerPatternComputeShaderProgram = new ShaderProgram({ checkerPatternComputeShader }, "CHECKER PATTERN COMPUTE SHADER");

	scene.cubemapTextureDay = cubemapTextureDay;
	scene.cubemapTextureNight = cubemapTextureNight;
	scene.currentSkyboxTexture = scene.cubemapTextureNight;

	setWindTexturesForSimulationMode();

	int width = PERLIN_NOISE_TEXTURE_WIDTH;
	int height = PERLIN_NOISE_TEXTURE_WIDTH;

	// Should this not be set to random?
	perlinNoiseSeedTextureData = new float[width * height];
	perlinNoiseSeedTexture = new Texture("Perlin Seed Texture", GL_TEXTURE_2D);
	perlinNoiseSeedTexture->generateTexture(perlinNoiseSeedTextureData, width, height, GL_RED);

	// Set up the z-buffer
	glDepthRange(-1, 1); // Make the NDC a right handed coordinate system, 
	// with the camera pointing towards -z
	glEnable(GL_DEPTH_TEST); // Turn on z-buffer depth perlinNoiseTexture
	glDepthFunc(GL_LESS);    // Draws fragments that are closer to the screen in NDC
	glEnable(GL_MULTISAMPLE);
}

void setWindTexturesForSimulationMode()
{
	if (scene.config.simulationMode == SimulationMode::FLUID_GRID)
	{
		scene.config.windX = scene.config.fluidGridConfig.velX;
		scene.config.windY = scene.config.fluidGridConfig.velY;
	}
	else if (scene.config.simulationMode == SimulationMode::PERLIN_NOISE)
	{
		scene.config.windX = scene.config.perlinConfig.texture;
		scene.config.windY = nullptr;
	}
	else if (scene.config.simulationMode == SimulationMode::CHECKER_PATTERN)
	{
		scene.config.windX = scene.config.checkerPatternTexture;
		scene.config.windY = nullptr;
	}

}

glm::vec2 calculateSpiralPosition(int n)
{
	n++;
	int k = (int)std::ceil((std::sqrt(n) - 1.0) / 2.0);
	int t = 2 * k + 1;
	int m = t * t;

	t -= 1;

	if (n >= m - t)
	{
		return { k - (m - n), -k };
	}

	m -= t;

	if (n >= m - t)
	{
		return { -k, -k + (m - n) };
	}

	m -= t;

	if (n >= m - t)
	{
		return { -k + (m - n), k };
	}

	return { k, k - (m - n - t) };
}

void initSceneObjects(Patch& patch)
{
	patch.init(MAX_BLADES_PER_PATCH, patchShaderProgram);

	if (scene.config.bladeDistribution == BladeDistribution::HARRY_STYLES_WITH_RANDOS)
	{
		patch.initHarryEdwardStylesBladeMatrices(scene.config.patchSize);
	}
	else if (scene.config.bladeDistribution == BladeDistribution::HARRY_STYLES)
	{
		patch.initHarryEdwardStylesBladeMatrices(scene.config.patchSize, false);
	}
	else if (scene.config.bladeDistribution == BladeDistribution::ONE_DIRECTION)
	{
		patch.initOneDirectionBladeMatrices(scene.config.patchSize);
	}

	createInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_BLADES_PER_PATCH);

	SceneObjectArrays* skybox = new SceneObjectArrays(cubePositions, *skyboxShaderProgram);
	scene.sceneObjects.push_back(skybox);

	SceneObjectArrays* light = new SceneObjectArrays(cubePositions, *lightShaderProgram);
	scene.sceneObjects.push_back(light);
	scene.light = light;
	scene.light->isVisible = false;

	for (int i = 0; i < MAX_PATCHES; i++)
	{
		// Position of the entire thing
		glm::vec2 position = calculateSpiralPosition(i) * scene.config.patchSize;
		SceneObjectIndexed* patchSceneObject = new SceneObjectIndexed(
			grassPatchPositions, grassPatchColors, grassPatchIndices, grassPatchNormals, *patchShaderProgram);
		glm::mat4 translation = glm::translate(position.x, 0, position.y);
		// Set the patch object
		patchSceneObject->model = translation * glm::scale(scene.config.patchSize, scene.config.patchSize, scene.config.patchSize);
		scene.patches.push_back(patchSceneObject);

		// Set the blades on the patch
		SceneObjectInstanced* blades = new SceneObjectInstanced(
			grassPositions, grassColors, grassIndices, grassNormals, instanceMatrixBuffer, *bladesShaderProgram, &grassUVs);
		// Do not scale the blades
		blades->model = translation * glm::scale(1, scene.config.bladeHeight, 1);
		scene.blades.push_back(blades);
	}

	SceneObjectArrays* fanDebugIcon = new SceneObjectArrays(fanDebugIconVertexPositions, *lightShaderProgram);
	scene.fanDebugIcon = fanDebugIcon;
}

void generatePerlinNoiseTexture()
{
	using namespace std::chrono;
	// Initialize seed data
	for (int i = 0; i < PERLIN_NOISE_TEXTURE_WIDTH * PERLIN_NOISE_TEXTURE_WIDTH; i++)
		perlinNoiseSeedTextureData[i] = (float)rand() / (float)RAND_MAX;

	perlinNoise2DGPU(*perlinNoiseSeedTexture, perlinNoiseSeedTextureData, perlinNoiseComputeShaderProgram,
		scene.config.perlinConfig.texture->getTextureID(), scene.config.perlinConfig.octaves,
		scene.config.perlinConfig.bias);
}

void generateCheckerPatternTexture()
{
	checkerPattern2DGPU(checkerPatternComputeShaderProgram, scene.config.checkerPatternTexture->getTextureID(), scene.config.checkerSize);
}

void checkerPattern2DGPU(ShaderProgram* computeShaderProgram, GLuint computeShaderTexture, int checkerSize)
{
	computeShaderProgram->use();

	GLCall(glBindImageTexture(0, computeShaderTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8));

	computeShaderProgram->setInt("checkerSize", checkerSize);

	GLCall(glDispatchCompute(CHECKER_PATTERN_TEXTURE_WIDTH / 16, CHECKER_PATTERN_TEXTURE_WIDTH / 16, 1));
}

void initIMGUI(GLFWwindow* window)
{
	// IMGUI init
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

void createInstanceMatrixBuffer(glm::mat4* modelMatrices, const unsigned int numInstances)
{
	GLCall(glGenBuffers(1, &instanceMatrixBuffer));
	transferInstanceMatrixBuffer(modelMatrices, numInstances);
}

void transferInstanceMatrixBuffer(glm::mat4* modelMatrices, const unsigned int numInstances)
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), modelMatrices, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

glm::vec2 getNormalizedMousePos() {
	glm::vec2 cursorPos = { 0, 0 };
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glm::vec2 windowSize = { (float)width, (float)height };

		// If it's not disabled, use the cursor position
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		cursorPos = { (float)x, (float)y };

		cursorPos.x = (2.0f * (float)x) / width - 1.0f;
		cursorPos.y = 1.0f - (2.0f * (float)y) / height;
	}

	cursorPos = glm::clamp(cursorPos, -1.0f, 1.0f);
	return cursorPos;
}

Ray getMouseRay()
{
	Ray ray = {};

	glm::vec2 cursorPos = getNormalizedMousePos();

	glm::mat4 inverseProjection = glm::inverse(scene.projection);
	glm::mat4 inverseView = glm::inverse(scene.view);

	// Direction
	glm::vec4 clipCoords = glm::vec4(cursorPos.x, cursorPos.y, -1.0f, 1.0f);
	{
		glm::vec4 eyeCoords = inverseProjection * clipCoords;
		eyeCoords = { eyeCoords.x, eyeCoords.y, -1.0f, 0.0f };
		ray.direction = glm::normalize(glm::vec3(inverseView * eyeCoords));
	}

	// Origin
	{
		glm::vec4 eyeCoords = (inverseProjection * clipCoords);
		eyeCoords = { eyeCoords.x, eyeCoords.y, 0, 1.0f };
		ray.origin = glm::vec3(inverseView * eyeCoords);
	}

	return ray;
}

std::optional<glm::vec3> mouseHitsGround()
{
	Ray ray = getMouseRay();

	return ray.intersectsRectangle(scene.worldRekt);
}

void drawFanWindow()
{
	auto& conf = scene.config;
	auto& fluidConf = conf.fluidGridConfig;

	ImGui::Begin("Fans");
	ImGui::Checkbox("Draw Fans", &scene.config.fluidGridConfig.shouldDrawFans);

	for (int fanIndex = 0; fanIndex < fluidConf.fans.size(); fanIndex++)
	{
		Fan& fan = scene.config.fluidGridConfig.fans[fanIndex];

		ImGui::PushID(fanIndex);
		std::string headerName = "Fan " + std::to_string(fanIndex);

		bool wasSelected = fluidConf.selectedFanIndex == fanIndex;


		if (ImGui::CollapsingHeader(headerName.c_str()))
		{
			ImGui::BeginGroup();

			if (wasSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_FrameBg, { 1, 0, 0, 1 });
			}

			const char* selectBtnText = fluidConf.selectedFanIndex == fanIndex ? "Deselect" : "Select";

			if (ImGui::Button(selectBtnText))
			{
				if (fluidConf.selectedFanIndex == fanIndex)
				{
					fluidConf.selectedFanIndex = -1;
				}
				else
				{
					fluidConf.selectedFanIndex = fanIndex;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				fluidConf.fans.erase(fluidConf.fans.begin() + fanIndex);

				if (fluidConf.fans.size() == 0)
				{
					fluidConf.selectedFanIndex = -1;
				}

				// Are we deleting the selected one?
				if (fluidConf.selectedFanIndex == fanIndex)
				{
					fluidConf.selectedFanIndex = -1;
				}
				// We are deleting something to the left of the selected
				else if (fanIndex < fluidConf.selectedFanIndex)
				{
					fluidConf.selectedFanIndex--;
				}
			}


			ImGui::Checkbox("Fan Active", &fan.active);
			ImGui::DragFloat2("Fan Position", (float*)&fan.position, 0.05f, 0, 1);
			ImGui::InputFloat("Fan Density", &fan.density);
			ImGui::InputFloat2("Fan Velocity", (float*)&fan.velocity);

			if (wasSelected)
			{
				ImGui::PopStyleColor();
			}
			ImGui::EndGroup();
		}

		ImGui::PopID();
	}

	ImGui::End();
}

void drawFluidGridWindow()
{
	auto& conf = scene.config;
	auto& fluidConf = conf.fluidGridConfig;
	if (conf.simulationMode == SimulationMode::FLUID_GRID)
	{
		ImGui::Begin("Fluid Grid");

		if (clearNextSimulate)
		{
			fluidGrid->clearCurrent();
		}
		if (ImGui::CollapsingHeader("Add Impulse"))
		{

			static glm::vec2 pos = { 0.5f, 0.5f };
			static glm::vec2 vel = {};
			static float     den = {};
			ImGui::DragFloat2("Position", (float*)&pos, 0.05f, 0, 1);
			ImGui::InputFloat("Density", &den);
			ImGui::SameLine();
			if (ImGui::Button("Add Density"))
			{
				fluidGrid->addDensityAt(
					(int)(pos.x * fluidGrid->getN()),
					(int)(pos.y * fluidGrid->getN()),
					den);
				clearNextSimulate = false;
			}
			ImGui::InputFloat2("Velocity", (float*)&vel);
			ImGui::SameLine();

			if (ImGui::Button("Add Velocity"))
			{
				fluidGrid->addVelocityAt(
					(int)(pos.x * fluidGrid->getN()),
					(int)(pos.y * fluidGrid->getN()),
					vel.x, vel.y);
				clearNextSimulate = false;
			}

		}

		static bool shouldDrawFanWindow = true;
		if (ImGui::Button("Toggle Fan Window"))
		{
			shouldDrawFanWindow = !shouldDrawFanWindow;
		}

		if (shouldDrawFanWindow)
		{
			drawFanWindow();

		}

		ImGui::Text("Randomness Controls");
		static glm::vec2 denRange = { 100, 1000 };
		static glm::vec2 velRange = { 100, 100 };
		ImGui::InputFloat2("Random density", (float*)&denRange);
		ImGui::InputFloat2("Random velocity", (float*)&velRange);
		if (ImGui::Button("Add Random"))
		{
			clearNextSimulate = false;
			float x = generateRandomNumber(0, (float)fluidGrid->getN());
			float y = generateRandomNumber(0, (float)fluidGrid->getN());
			float d = generateRandomNumber(denRange.x, denRange.y);
			float vx = generateRandomNumber(-velRange.x, velRange.x);
			float vy = generateRandomNumber(-velRange.y, velRange.y);

			for (int i = (int)x - 2; i < (int)x + 2; ++i)
			{
				for (int j = (int)y - 2; j < (int)y + 2; ++j)
				{
					fluidGrid->addVelocityAt(i, j, vx, vy);
					fluidGrid->addDensityAt(i, j, d);
				}
			}
		}


		float width = 512;
		if (ImGui::BeginTabBar("Fluid Grid Textures"))
		{
			if (ImGui::BeginTabItem("Density"))
			{
				ImGui::Image((ImTextureID)(long long)fluidGrid->getTextureDen()->getTextureID(),
					{ width, width },
					{ 0.0f, 1 },
					{ 1.0f, 0 });
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Velocity X"))
			{
				ImGui::Image((ImTextureID)(long long)fluidGrid->getTextureVelX()->getTextureID(),
					{ width, width },
					{ 0.0f, 1 },
					{ 1.0f, 0 });
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Velocity Y"))
			{
				ImGui::Image((ImTextureID)(long long)fluidGrid->getTextureVelY()->getTextureID(),
					{ width, width },
					{ 0.0f, 1 },
					{ 1.0f, 0 });
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
#ifdef _DEBUG
		ImGui::Text("Total Density %.1f", fluidGrid->totalDensity());
#endif


		if (ImGui::Button("Reset"))
		{
			fluidGrid->initialize();
		}


		static float diffRatio = 0.56f;
		if (ImGui::DragFloat("Diffusion Ratio", &diffRatio, 0.01f, 0.0f, 1.0f))
		{
			*fluidGrid->getDiffPointer() = glm::mix(0.0f, 0.001f, diffRatio);
		}
		ImGui::LabelText("Diffusion Value", "%.5f", *fluidGrid->getDiffPointer());
		ImGui::DragFloat("Viscosity", fluidGrid->getViscPointer(), 0.0001f, 0.0f, 0.005f);
		ImGui::DragFloat("Velocity Multiplier", &scene.config.fluidGridConfig.velocityMultiplier, 0.1f, 0, 100.0f);
		ImGui::DragFloat2("Velocity Clamp", (float*)&scene.config.fluidGridConfig.velocityClampRange, 0.1f, 0, 2.0f);

		ImGui::End();
	}
}

void drawSettingsWindow()
{
	ImGui::Begin("Settings");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS) Framecount %lld Time %.0f",
		1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate, numFrames, glfwGetTime());
	ImGui::Text("Simulation Mode Settings");
	if (ImGui::RadioButton("Perlin Noise", scene.config.simulationMode == SimulationMode::PERLIN_NOISE))
	{
		scene.config.simulationMode = SimulationMode::PERLIN_NOISE;
		generatePerlinNoiseTexture();

		scene.config.windX = scene.config.perlinConfig.texture;
		scene.config.windY = nullptr;
	}
	ImGui::SameLine();
	drawTooltip("Blades respond to the generated perlin noise.");
	if (ImGui::RadioButton("Checker Pattern", scene.config.simulationMode == SimulationMode::CHECKER_PATTERN))
	{
		scene.config.simulationMode = SimulationMode::CHECKER_PATTERN;
		generateCheckerPatternTexture();

		scene.config.windX = scene.config.checkerPatternTexture;
		scene.config.windY = nullptr;
	}
	ImGui::SameLine();
	drawTooltip("Blades respond to the generated checker pattern.");
	if (ImGui::RadioButton("Fluid Grid", scene.config.simulationMode == SimulationMode::FLUID_GRID))
	{
		scene.config.simulationMode = SimulationMode::FLUID_GRID;
		scene.config.windX = scene.config.fluidGridConfig.velX;
		scene.config.windY = scene.config.fluidGridConfig.velY;
	}
	drawTooltip("Blades respond to the fluid grid simulation.");


	ImGui::Text("Harry Styles Settings");
	if (ImGui::RadioButton("Harry Styles With Randos",
		scene.config.bladeDistribution == BladeDistribution::HARRY_STYLES_WITH_RANDOS))
	{
		scene.config.bladeDistribution = BladeDistribution::HARRY_STYLES_WITH_RANDOS;
		patch.initHarryEdwardStylesBladeMatrices(scene.config.patchSize);
		transferInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_BLADES_PER_PATCH);
	}
	ImGui::SameLine();
	drawTooltip("Blades are placed uniformly on the patch with random rotations.");
	if (ImGui::RadioButton("Harry Styles", scene.config.bladeDistribution == BladeDistribution::HARRY_STYLES))
	{
		scene.config.bladeDistribution = BladeDistribution::HARRY_STYLES;
		patch.initHarryEdwardStylesBladeMatrices(scene.config.patchSize, false);
		transferInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_BLADES_PER_PATCH);
	}
	ImGui::SameLine();
	drawTooltip("Blades are placed uniformly on the patch without random rotations.");
	if (ImGui::RadioButton("One Direction", scene.config.bladeDistribution == BladeDistribution::ONE_DIRECTION))
	{
		scene.config.bladeDistribution = BladeDistribution::ONE_DIRECTION;
		patch.initOneDirectionBladeMatrices(scene.config.patchSize);
		transferInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_BLADES_PER_PATCH);
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
			if (scene.config.windX) scene.config.windX->unbind();
			if (scene.config.windY) scene.config.windY->unbind();

			scene.config.windX = scene.config.fluidGridConfig.density;
			scene.config.windY = nullptr;
		}
		if (ImGui::RadioButton("Velocity", !scene.config.fluidGridConfig.visualizeDensity))
		{
			scene.config.fluidGridConfig.visualizeDensity = false;
			if (scene.config.windX) scene.config.windX->unbind();
			if (scene.config.windY) scene.config.windY->unbind();

			scene.config.windX = scene.config.fluidGridConfig.velX;
			scene.config.windY = scene.config.fluidGridConfig.velY;
		}
	}

	if (ImGui::SliderFloat("Blade height", &scene.config.bladeHeight, 0.1f, 10.0f))
	{
		for (int i = 0; i < MAX_PATCHES; i++)
		{
			glm::vec2 position = calculateSpiralPosition(i) * scene.config.patchSize;
			glm::mat4 translation = glm::translate(position.x, 0, position.y);
			scene.blades[i]->model = translation * glm::scale(1, scene.config.bladeHeight, 1);
		}
	}

	if (ImGui::SliderFloat("PatchSize", &scene.config.patchSize, 1.0f, 100.0f))
	{
		switch (scene.config.bladeDistribution)
		{
		case BladeDistribution::ONE_DIRECTION:
			patch.initOneDirectionBladeMatrices(scene.config.patchSize);
			break;
		case BladeDistribution::HARRY_STYLES_WITH_RANDOS:
			patch.initHarryEdwardStylesBladeMatrices(scene.config.patchSize, true);
			break;
		case BladeDistribution::HARRY_STYLES:
			patch.initHarryEdwardStylesBladeMatrices(scene.config.patchSize, false);
			break;
		}

		transferInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_BLADES_PER_PATCH);

		for (int i = 0; i < MAX_PATCHES; i++)
		{
			glm::vec2 position = calculateSpiralPosition(i) * scene.config.patchSize;
			glm::mat4 translation = glm::translate(position.x, 0, position.y);
			scene.patches[i]->model = translation * glm::scale(scene.config.patchSize, scene.config.patchSize, scene.config.patchSize);
			scene.blades[i]->model = translation * glm::scale(1, scene.config.bladeHeight, 1);
		}

		setWorldMinMax();
	}


	drawTooltip("The sizes of each individual Patch. Same number of blades of grass.");

	ImGui::SliderInt("Number of patches", &scene.config.numPatches, 1, MAX_PATCHES);
	//ImGui::DragInt("Number blades per patch", &scene.config.numBladesPerPatch, 0, MAX_BLADES_PER_PATCH);
	ImGui::DragInt("Number blades per patch", &scene.config.numBladesPerPatch, 1, 0, MAX_BLADES_PER_PATCH);
	scene.config.numBladesPerPatch = glm::clamp(scene.config.numBladesPerPatch, 0, (int)MAX_BLADES_PER_PATCH);
	ImGui::SliderFloat("Sway Reach", &scene.config.swayReach, 0.0f, 2.0f);
	drawTooltip("How far the blades will move in the wind.");

	if (scene.config.simulationMode != SimulationMode::FLUID_GRID)
	{
		ImGui::SliderFloat("Wind Strength", &scene.config.windStrength, 0, 0.5f);
		drawTooltip("Strength of the wind");

		ImGui::DragFloat2("Wind Direction", (float*)&scene.config.windDirection,
			0.1f, -1.0f, 1.0f);
		drawTooltip("Direction of the wind");
	}

	if (glm::epsilonEqual(glm::length(scene.config.windDirection), 0.0f, 0.0001f))
	{
		scene.config.windDirection = { 1, 0 };
	}
	if (ImGui::Button("Normalize Wind Direction"))
		scene.config.windDirection = glm::normalize(scene.config.windDirection);

	drawTooltip("Normalize the wind direction");

	if (scene.config.simulationMode == SimulationMode::PERLIN_NOISE && ImGui::CollapsingHeader(
		"Perlin Noise Settings"))
	{
		ImGui::SliderInt("Octaves", &scene.config.perlinConfig.octaves, 2, 10);
		drawTooltip("Octaves for fun.");

		ImGui::SliderFloat("Bias", &scene.config.perlinConfig.bias, 0.2f, 2.0f);
		drawTooltip("Bias for fun.");

		if (ImGui::Button("Generate Perlin Noise"))
		{
			generatePerlinNoiseTexture();
		}
		float width = PERLIN_NOISE_TEXTURE_WIDTH;

		ImGui::Image((ImTextureID)(long long)scene.config.perlinConfig.texture->getTextureID(),
			{ width, width },
			{ 0.0f, 1.0f },
			{ 1.0f, 0.0f });
	}

	if (scene.config.simulationMode == SimulationMode::CHECKER_PATTERN && ImGui::CollapsingHeader("Checker Pattern Settings"))
	{
		if (ImGui::SliderInt("CheckerSize", &scene.config.checkerSize, 1, 512))
		{
			generateCheckerPatternTexture();
		}
		drawTooltip("Checker size for fun. Only powers of two look nice.");
	}


	if (ImGui::CollapsingHeader("Camera Settings"))
	{
		ImGui::InputFloat3("Camera Position", (float*)&camera.camPosition);
		ImGui::InputFloat("Yaw", &camera.yaw);
		ImGui::InputFloat("Pitch", &camera.pitch);
	}




	if (ImGui::CollapsingHeader("Light Settings"))
	{
		ImGui::Checkbox("Show Light", &scene.light->isVisible);
		ImGui::SliderFloat("Ambient Light Strength", &scene.config.ambientStrength, 0.0f, 1.0f);
		ImGui::DragFloat3("Light Position", (float*)&scene.config.lightPosition, 0.1f, -100, 100);
		ImGui::ColorEdit4("Light Color", (float*)&scene.config.lightColor);
		ImGui::SliderFloat("Light Intensity", &scene.config.lightIntensity, 0.0f, 10);

		ImGui::Separator();

		ImGui::Text("Skybox Settings");
		if (ImGui::RadioButton("Day", scene.config.skyboxType == SkyboxType::DAY))
		{
			scene.config.skyboxType = SkyboxType::DAY;
			scene.currentSkyboxTexture = scene.cubemapTextureDay;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Night", scene.config.skyboxType == SkyboxType::NIGHT))
		{
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

void drawGui()
{
	// Slider will be 65% of the window width (this is the default)
	ImGui::NewFrame();

	//if(scene.config.isPaused)
	//{
	//	ImGui::Begin("IsPaused");

	//	ImGui::Text("Is Paused");

	//	ImGui::End();

	//}
	auto viewport = ImGui::GetMainViewport();

	// Draw a little dot at the center of the screen
	ImVec2 halfScreenSize = { viewport->Size.x / 2.0f, viewport->Size.x / 2.0f };
	ImGui::SetNextWindowPos({ viewport->Pos.x + halfScreenSize.x, viewport->Pos.y + halfScreenSize.y });

	//ImGui::Begin("Target", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
	//ImGui::End();

	// Hitpos debug
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize({ viewport->Size });
	ImGui::Begin("Look info", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

	glm::vec2 mousePos = getNormalizedMousePos();
	Ray mouseRay = getMouseRay();
	auto hitPos = mouseHitsGround();

	ImGui::Text("Mouse Pos: (%.3f, %.3f)", mousePos.x, mousePos.y);
	ImGui::Text("Origin: (%.3f, %.3f, %.3f)", mouseRay.origin.x, mouseRay.origin.y, mouseRay.origin.z);
	ImGui::Text("Direction: (%.3f, %.3f, %.3f)", mouseRay.direction.x, mouseRay.direction.y, mouseRay.direction.z);
	if (hitPos.has_value()) {
		glm::vec3 pos = hitPos.value();
		ImGui::Text("Hits: (%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z);
	}
	else {
		ImGui::Text("Hits: MISS");
	}

	debugText("WorldRekt: (%.2f, %.2f, %.2f)", scene.worldRekt.center.x, scene.worldRekt.center.y, scene.worldRekt.center.z);
	debugText("    Size (% .3f, % .3f)", scene.worldRekt.width, scene.worldRekt.height);

	for (const auto& str : g_debugStrings)
	{
		ImGui::Text(str.c_str());
	}

	ImGui::End();



	drawFluidGridWindow();
	drawSettingsWindow();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	camera.updateCameraVectors();

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void drawTooltip(const char* desc)
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
	float min, float max, float& x, float& y)
{
	float sum = max - min;
	float xInRange = (float)screenX / (float)screenW * sum - sum / 2.0f;
	float yInRange = (float)screenY / (float)screenH * sum - sum / 2.0f;
	x = xInRange;
	y = -yInRange; // Flip screen space on the y-axis
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// Makes sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}



// RETURNS NULL IF NONE IS SELECTED
Fan* getSelectedFan()
{
	// Ensure we have a fan selected
	int fanIndex = scene.config.fluidGridConfig.selectedFanIndex;
	if (fanIndex == -1)
		return nullptr;

	Fan* fan = &scene.config.fluidGridConfig.fans[fanIndex];
	return fan;
}

void handleScroll()
{
	static bool scrollIsPressed = false;

	// Is button down
	if (glfwGetMouseButton(window, 2) == GLFW_PRESS)
	{
		// First button down first time after having released
		if (!scrollIsPressed)
		{
			auto hit = mouseHitsGround();
 			if (hit.has_value())
			{
				// We hit something
				glm::vec3 hitPos = hit.value();
				if (g_ctrlIsPressed)
				{
					// Create new fan and select it
					Fan fan{};
					fan.density = 250.0f;
					fan.position = scene.mapPositionFromWorldSpace({ hitPos.x, hitPos.z });
					scene.config.fluidGridConfig.fans.push_back(fan);
					scene.config.fluidGridConfig.selectedFanIndex = (int)scene.config.fluidGridConfig.fans.size() - 1;
				}
				else if (Fan* fan = getSelectedFan())
				{
					fan->position = scene.mapPositionFromWorldSpace({ hitPos.x, hitPos.z });
				}

				g_fanDragStart = hitPos;
				g_fanIsDragging = true;
			}

			scrollIsPressed = true;
		}
		else
		{
			Fan* fan = getSelectedFan();
			if (fan && scrollIsPressed && g_fanIsDragging)
			{
				auto hit = mouseHitsGround();
				if (hit.has_value()) {
					auto dif = hit.value() - g_fanDragStart;
					fan->velocity = scene.mapVelocityFromWorldSpace({dif.x, dif.z});
				}
			}
		}
	}

	if (glfwGetMouseButton(window, 2) == GLFW_RELEASE)
	{
		scrollIsPressed = false;
		g_fanIsDragging = false;
	}
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	static bool tWasPressed = false;
	static bool rWasPressed = false;
	static bool pWasPressed = false;
	static bool vWasPressed = false;
	static bool upWasPressed = false;
	static bool downWasPressed = false;

	const float densityStep = 25;

	handleScroll();

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		if (!upWasPressed)
		{
			upWasPressed = true;

			auto fan = getSelectedFan();
			if (fan)
			{
				fan->density += densityStep;
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
	{
		upWasPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		if (!downWasPressed)
		{
			downWasPressed = true;

			auto fan = getSelectedFan();
			if (fan)
			{
				fan->density -= densityStep;
			}
		}
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
	{
		downWasPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		if (!g_ctrlIsPressed)
		{
			g_ctrlIsPressed = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
	{
		g_ctrlIsPressed = false;
	}

	// Stop camera movement if GUI is opened
	if (isPaused)
		return;

	// Input below here is disabled when GUI is open:
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera.processKeyboard(cameraMovement::GLOBAL_UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera.processKeyboard(cameraMovement::GLOBAL_DOWN, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.processKeyboard(cameraMovement::FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.processKeyboard(cameraMovement::BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.processKeyboard(cameraMovement::LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.processKeyboard(cameraMovement::RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (!pWasPressed)
		{
			scene.config.isPaused = !scene.config.isPaused;
			pWasPressed = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
	{
		pWasPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		if (!vWasPressed)
		{
			int mode;
			glGetIntegerv(GL_POLYGON_MODE, &mode);
			if (mode == GL_LINE)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			vWasPressed = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE)
	{
		vWasPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		if (!tWasPressed)
		{
			camera.sprinting = !camera.sprinting;
			tWasPressed = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
	{
		tWasPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		if (!rWasPressed)
		{
			bladesShaderProgram->reloadShaders();
			patchShaderProgram->reloadShaders();
			skyboxShaderProgram->reloadShaders();
			lightShaderProgram->reloadShaders();
			rWasPressed = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
	{
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
	int         other, int  action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		isPaused = !isPaused;
		glfwSetInputMode(window, GLFW_CURSOR,
			glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED
			? GLFW_CURSOR_NORMAL
			: GLFW_CURSOR_DISABLED);
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
