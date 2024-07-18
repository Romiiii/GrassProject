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

#include "grass_simulation/grass_simulation.h"

#include "rendering/primitives.h"
#include "rendering/scene_object_arrays.h"
#include "rendering/scene_object_indexed.h"
#include "rendering/scene_object_instanced.h"
#include "rendering/shader.h"
#include "rendering/shader_program.h"
#include "rendering/glmutils.h"

#include "camera.h"
#include "debug.h"
#include "scene.h"
#include "util.h"
#include "logger.h"
#include "input.h"

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
 * \brief The scene currently loaded (We don't support multiple scenes, but we
 * could with this.)
 */
Scene scene;


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
void initCommonSceneObjects();

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

//
// Timing 
//

/**
 * \brief Time between current frame and last frame
 */
float elapsed = 0.0f;

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
 * \brief Draws the GUI
*/
void drawGui();

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
 * @brief Cleans up the scene.
*/
void cleanUp();

const bool INCLUDE_GRASS_SIMULATION = false;

void initCommonShadersAndTextures();
bool setup();
Inputs& inputs = Inputs::instance();

#define TRUEFALSE(expr) ((!!expr) ? "true" : "false")

int main()
{
	Logger::init("log.log");
	LOG_INFO("Starting");
	LOG_INFO("Config: \n\tINCLUDE_GRASS_SIMULATION = %s", TRUEFALSE(INCLUDE_GRASS_SIMULATION));
	auto startTime = glfwGetTime();
	setup();

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	scene.config.isPaused = false;
	scene.config.currentTime = (float)glfwGetTime();

	LOG_INFO("Initializing application took %.2fms", (glfwGetTime() - startTime) * 100);
	LOG_INFO("Starting main loop");
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
		elapsed = currentFrame - lastFrame;
		gameDeltaTime = elapsed;

		if (scene.config.isPaused)
		{
			gameDeltaTime = 0;
		}

		processInput(window);

		if (INCLUDE_GRASS_SIMULATION)
		{
			GrassSimulation::update(gameDeltaTime);
		}

		// Clear the color depth buffer (aka z-buffer) every new frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		// Render loop : render every loopInterval seconds
		float loopInterval = 0.02f;

		// Control render loop frequency
		float elapsed = (float)glfwGetTime() - lastFrame;
		while (loopInterval > elapsed)
		{
			elapsed = (float)glfwGetTime() - lastFrame;
		}
		lastFrame = currentFrame;
		numFrames++;
		g_debugStrings.clear();
		inputs.advanceFrame();
	}

	cleanUp();
	Logger::deinit();
	return 0;
}

bool setup()
{
	initGLFWWindow();
	inputs.init(window, 1);

	// Hide the cursor and capture it
	inputs.disableCursor();

	// GLAD: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LOG_FATAL("Failed to initialize GLAD");
		return false;
	}

	// check OpenGL version
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	if (!(major >= 4 && minor >= 3))
	{
		LOG_FATAL("Wrong OpenGL version");
		return false;
	}

	// Initialize camera
	camera.camPosition = { 0, 20, 0 };
	camera.yaw = 0;
	camera.pitch = -89.0f;
	camera.updateCameraVectors();

	// Set seed for random numbers
	srand((unsigned)time(0));

	initIMGUI(window);

	initCommonShadersAndTextures();
	initCommonSceneObjects();

	if (INCLUDE_GRASS_SIMULATION && !GrassSimulation::setup(&scene))
	{
		return false;
	}

	return true;
}

void initCommonShadersAndTextures()
{
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
	scene.lightShaderProgram = lightShaderProgram;

	scene.cubemapTextureDay = cubemapTextureDay;
	scene.cubemapTextureNight = cubemapTextureNight;
	scene.currentSkyboxTexture = scene.cubemapTextureNight;

	// Set up the z-buffer
	glDepthRange(-1, 1); // Make the NDC a right handed coordinate system, 
	// with the camera pointing towards -z
	glEnable(GL_DEPTH_TEST); // Turn on z-buffer depth perlinNoiseTexture
	glDepthFunc(GL_LESS);    // Draws fragments that are closer to the screen in NDC
	glEnable(GL_MULTISAMPLE);
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
	window = glfwCreateWindow(INIT_SCR_WIDTH, INIT_SCR_HEIGHT, "GrassProject", NULL, NULL);
	if (window == NULL)
	{
		LOG_FATAL("Failed to create GLFW window");
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(0);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	return window;
}


void initCommonSceneObjects()
{
	SceneObjectArrays* skybox = new SceneObjectArrays(cubePositions, *skyboxShaderProgram);
	scene.sceneObjects.push_back(skybox);

	SceneObjectArrays* light = new SceneObjectArrays(cubePositions, *lightShaderProgram);
	scene.sceneObjects.push_back(light);
	scene.light = light;
	scene.light->isVisible = false;

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


void drawSettingsWindow()
{
	ImGui::Begin("Settings");

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS) Framecount %lld Time %.0f",
		1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate, numFrames, glfwGetTime());

	if (INCLUDE_GRASS_SIMULATION)
	{
		GrassSimulation::drawGui();
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
	ImGui::NewFrame();

	// Debug text window
	{
		ImGui::Begin("Debug text", nullptr, 0);

		for (const auto& str : g_debugStrings)
		{
			ImGui::Text(str.c_str());
		}
		ImGui::End();
	}

	if (INCLUDE_GRASS_SIMULATION)
	{
		GrassSimulation::drawGui();
	}
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

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	// Makes sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window)
{
	if (inputs.keyIsPressed(GLFW_KEY_ESCAPE))
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (INCLUDE_GRASS_SIMULATION) {
		GrassSimulation::handleInputs();
	}

	if (inputs.keyIsPressed(GLFW_KEY_SPACE))
	{
		isPaused = !isPaused;
		inputs.toggleCursorEnabled();
	}

	// Stop camera movement if GUI is opened
	if (isPaused)
		return;

	camera.processMouseMovement(inputs.getMouseDelta());

	// Input below here is disabled when GUI is open:
	if (inputs.keyIsHeld(GLFW_KEY_E))
	{
		camera.processKeyboard(cameraMovement::GLOBAL_UP, elapsed);
	}
	if (inputs.keyIsHeld(GLFW_KEY_Q))
	{
		camera.processKeyboard(cameraMovement::GLOBAL_DOWN, elapsed);
	}
	if (inputs.keyIsHeld(GLFW_KEY_W))
	{
		camera.processKeyboard(cameraMovement::FORWARD, elapsed);
	}
	if (inputs.keyIsHeld(GLFW_KEY_S))
	{
		camera.processKeyboard(cameraMovement::BACKWARD, elapsed);
	}
	if (inputs.keyIsHeld(GLFW_KEY_A))
	{
		camera.processKeyboard(cameraMovement::LEFT, elapsed);
	}
	if (inputs.keyIsHeld(GLFW_KEY_D))
	{
		camera.processKeyboard(cameraMovement::RIGHT, elapsed);
	}
	if (inputs.keyIsPressed(GLFW_KEY_P))
	{
		scene.config.isPaused = !scene.config.isPaused;
	}

	if (inputs.keyIsPressed(GLFW_KEY_V))
	{
		scene.config.isPaused = !scene.config.isPaused;
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
	}

	if (inputs.keyIsPressed(GLFW_KEY_T))
	{
		camera.sprinting = !camera.sprinting;
	}

	if (inputs.keyIsPressed(GLFW_KEY_R))
	{
		skyboxShaderProgram->reloadShaders();
		lightShaderProgram->reloadShaders();
		if (INCLUDE_GRASS_SIMULATION)
		{
			GrassSimulation::reloadShaders();
		}
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

	delete skyboxVertexShader;
	delete skyboxFragmentShader;
	delete skyboxShaderProgram;
	delete lightVertexShader;
	delete lightFragmentShader;
	delete lightShaderProgram;

	GrassSimulation::cleanup();
}
