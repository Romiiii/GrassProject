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

// Constants
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;
// Maximum amount of grass blades per patch
const unsigned int MAX_PATCH_DENSITY_BLADES = 20000;
// Maximum amount of billboards per patch
const unsigned int MAX_PATCH_DENSITY_BILLBOARDS = 500;

// Global variables used for rendering
SceneObject grass;
SceneObject patch;
SceneObject billboardSquare;
SceneObject skybox;

float currentTime;
Shader shaderProgram;
Shader skyboxShader;
Shader billboardShader;

// Array of grass coordinates 
std::vector<glm::vec3> grassCoordinates;
std::vector<glm::vec2> grassRotations;
std::vector<glm::mat4> grassMatrixes;

// Textures
Texture billboardGrassTexture;
Texture billboardGrassNoise1;
Texture billboardGrassNoise2;

Texture cubemapTextureDay;
Texture cubemapTextureNight;

// Camera setup
Camera camera;
bool isPaused = false;  // Used to stop camera movement when GUI is open
bool firstMouse = false;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;

// Timing 
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

GLFWwindow* initGLFWWindow();
void setupShadersAndMeshes();
void initIMGUI(GLFWwindow* window);
void drawScene();
void initPatch();
void drawPatch(glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::mat4 rotation = glm::mat4());
void drawGrass(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
void drawBillboardSquare(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
void drawBillboardCollection(glm::mat4 projection, glm::mat4 view, glm::mat4 model);
void drawSkybox(glm::mat4 projection, glm::mat4 view);
void drawGui();

void cursorInRange(float screenX, float screenY, int screenW, int screenH, float min, float max, float &x, float &y);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void keyInputCallback(GLFWwindow* window, int button, int other, int action, int mods);
void cursorInputCallback(GLFWwindow* window, double posX, double posY);

/* All variables that can be configured using the GUI
 */
struct Config {
	int grassType = 1;	 // Type 0 is seperate blades and type 1 is billboards
	int patchDensity = 20;
	int windType = 0;	// Type 0 is simple trigonometric sway,
						// type 1 and 2 are complex trigonometric sway 
						// and type 3 is Perlin sway
	float windStrength = 0.1; // Perlin sway can only go upto 0.1
	float swayReach = 0.1;
	float perlinSampleScale = 0.05;
	int perlinTexture = 1;  // Either 1 or 2
	glm::vec3 lightPosition = glm::vec3(1.0, 3.0, -1.0);
	float ambientStrength = 0.5f;
	int skyboxType = 1;  // Type 0 is day, type 1 is night 
} config;


int main()
{
	GLFWwindow* window = initGLFWWindow();
	assert(window != NULL, "ERROR:: Failed to create GLFW window");
	
    // GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    setupShadersAndMeshes();

    // Render loop : render every loopInterval seconds
    float loopInterval = 0.02f;

	// Set seed for random numbers
	srand((unsigned)time(0));

	// Initialize the random locations of the grass blades 
	initPatch();

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

        shaderProgram.use();

		assert(config.grassType == 0 || config.grassType == 1, "ERROR:: Incorrect grassType"); 
		drawScene();

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GrassProject", NULL, NULL);
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
void setupShadersAndMeshes() {
	// Initialize shader
	shaderProgram.initialize("shader.vert", "shader.frag");

	// Load grass mesh into openGL
	grass.createVertexArray(grassVertices, grassColors, grassIndices,
		grassNormals, shaderProgram);
	// Load patch mesh into openGL
	patch.createVertexArray(grassPatchVertices, grassPatchColors,
		grassPatchIndices, grassPatchNormals, shaderProgram);

	// Initialize billboard grass texture
	billboardShader.initialize("billboard.vert", "billboard.frag");
	std::string billboardGrassFileName = "images/grass_texture.tga";
	std::string billboardGrassFileNameNoise1 = "images/perlin_noise_1.tga";
	std::string billboardGrassFileNameNoise2 = "images/perlin_noise_2.tga";
	billboardGrassTexture.loadTexture(billboardGrassFileName);
	billboardGrassNoise1.loadTexture(billboardGrassFileNameNoise1, false);
	billboardGrassNoise2.loadTexture(billboardGrassFileNameNoise2, false);

	// Load billboard square into openGL
	billboardSquare.createVertexArrayTexture(
		billboardSquareVertices, billboardSquareColors,
		billboardSquareIndices, billboardSquareUVs,
		billboardSquareNormals, billboardShader);

	// Setup the Skybox Shaders
	skyboxShader.initialize("skybox.vert", "skybox.frag");
	cubemapTextureDay.loadTextureCubeMap(facesDay, false);
	cubemapTextureNight.loadTextureCubeMap(facesNight);
	skybox.createVertexArrayFromPositions(skyboxVertices);

	// Set up the z-buffer
	glDepthRange(-1, 1);  // Make the NDC a right handed coordinate system, 
						  // with the camera pointing towards -z
	glEnable(GL_DEPTH_TEST);  // Turn on z-buffer depth test
	glDepthFunc(GL_LESS);  // Draws fragments that are closer to the screen in NDC
	glEnable(GL_MULTISAMPLE);
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

/* Draws the scene with grass blades. 
 */
void drawScene() {
	glm::mat4 scale = glm::scale(1.f, 1.f, 1.f);
	glm::mat4 projection = glm::perspectiveFovRH_NO(70.0f, (float)SCR_WIDTH, 
		(float)SCR_HEIGHT, .01f, 100.0f);
	glm::mat4 view = glm::lookAt(
		camera.getCamPosition(),
		camera.getCamPosition() + camera.getCamForward(), glm::vec3(0, 1, 0));

	drawPatch(projection, view, glm::translate(-0.5 * grassPatchVertices[6], 
		-0.5 * grassPatchVertices[7], -0.5 * grassPatchVertices[8]));
	// Fit the other patch triangle to the other one 
	// The rotation of the second patch should be applied twice to the blades/billboards 
	// (to turn them back in the same direction as those in patch 1), so it is passed seperately
	drawPatch(projection, view, glm::translate(0.5 * grassPatchVertices[6], 
		0.5 * grassPatchVertices[7], 0.5 * grassPatchVertices[8]), glm::rotateY(2 * glm::half_pi<float>()));
	drawSkybox(projection, view);
}

/* Initializes the patch by calculating the coordinates of the grass/billboards.
 * Coordinates are sampled uniformly. A random rotation of the grass is 
 * generated as well. 
 */
void initPatch() {
	float r1;
	float r2;
	float r3;
	float r4;

	int upperBound = 1000;
	int lowerBound = 0;

	// Number is divided by 100 at the end to get the float number 
	int upperBoundRotationX = 6500;
	int lowerBoundRotationX = 5500;

	int upperBoundRotationY = 3150;
	int lowerBoundRotationY = 0;

	// Distribute the grass blades uniformly within the patch
	for (int x = 0; x < MAX_PATCH_DENSITY_BLADES; x += 1) {
		// Get two random numbers between 0 and 100
		r1 = (rand() % ((upperBound - lowerBound) + 1) + lowerBound) / 1000.0f;
		r2 = (rand() % ((upperBound - lowerBound) + 1) + lowerBound) / 1000.0f;

		glm::vec3 point = (1 - sqrt(r1)) * glm::vec3(grassPatchVertices[0], grassPatchVertices[1], grassPatchVertices[2]) + (sqrt(r1) * (1 - r2)) *
			glm::vec3(grassPatchVertices[3], grassPatchVertices[4], grassPatchVertices[5]) + (r2 * sqrt(r1)) *
			glm::vec3(grassPatchVertices[6], grassPatchVertices[7], grassPatchVertices[8]);

		// Add the coordinate to grassCoordinates
		grassCoordinates.push_back(glm::vec3(point[0], point[1], point[2]));

		// Generate the random x and y rotation
		r3 = (rand() % ((upperBoundRotationX - lowerBoundRotationX) + 1) + lowerBoundRotationX) / 1000.0f;
		r4 = (rand() % ((upperBoundRotationY - lowerBoundRotationY) + 1) + lowerBoundRotationY) / 1000.0f;

		grassRotations.push_back(glm::vec2(r3, r4));
	}
}

/* Draws a triangular patch of grass with the specified rotation. 
 */
void drawPatch(glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::mat4 rotation) {
	shaderProgram.use();
	shaderProgram.setMat4("projection", projection);
	shaderProgram.setMat4("view", view);
	if (rotation == glm::mat4()) {
		shaderProgram.setMat4("model", model);
	}
	else {
		// The rotation is applied to the patch, but not to the blades or billboards on the patch
		shaderProgram.setMat4("model", model * rotation);
	}

	shaderProgram.setFloat("ambientStrength", config.ambientStrength);
	shaderProgram.setVec3("lightPos", config.lightPosition);

	patch.drawSceneObject();

	// Only draw single blades when in blades mode 
	if (config.grassType == 0) {
		// Distribute the grass blades uniformly within the patch
		for (int x = 0; x < config.patchDensity; x += 1) {
			if (rotation == glm::mat4()) {
				drawGrass(projection, view, model * glm::translate(grassCoordinates.at(x)) * glm::rotateX(grassRotations.at(x)[0]) * glm::rotateY(grassRotations.at(x)[1]));
			}
			else {
				drawGrass(projection, view, model * rotation * glm::translate(grassCoordinates.at(x)) * glm::rotateX(grassRotations.at(x)[0]) * glm::rotateY(grassRotations.at(x)[1]) * rotation * rotation * rotation);
			}
		}
	}
	else if (config.grassType == 1) {
		// Distribute the billboards uniformly within the patch
		for (int x = 0; x < config.patchDensity; x += 1) {
			if (rotation == glm::mat4()) {
				drawBillboardCollection(projection, view, model * glm::translate(grassCoordinates.at(x)) * glm::translate(0.0, 0.5, 0.0) * glm::rotateY(grassRotations.at(x)[1]));
			}
			else {
				drawBillboardCollection(projection, view, model * rotation * glm::translate(grassCoordinates.at(x)) * glm::translate(0.0, 0.5, 0.0) * glm::rotateY(grassRotations.at(x)[1]) * rotation);
			}
		}
	}
}

/* Draws a single grass blade.
 */
void drawGrass(glm::mat4 projection, glm::mat4 view, glm::mat4 model) {
	shaderProgram.use();
	shaderProgram.setMat4("projection", projection);
	shaderProgram.setMat4("view", view);
	shaderProgram.setMat4("model", model);
	shaderProgram.setFloat("ambientStrength", config.ambientStrength);
	shaderProgram.setVec3("lightPos", config.lightPosition);
	grass.drawSceneObject();
}

/* Draws a single square of the billboard grass.
 */
void drawBillboardSquare(glm::mat4 projection, glm::mat4 view, glm::mat4 model) {
	billboardShader.use();
	billboardGrassTexture.bindTexture();
	billboardShader.setInt("billboardTexture", billboardGrassTexture.getTextureID());

	if (config.perlinTexture == 1) {
		billboardGrassNoise1.bindTexture();
		billboardShader.setInt("billboardNoise", billboardGrassNoise1.getTextureID());
	}
	else if (config.perlinTexture == 2) {
		billboardGrassNoise2.bindTexture();
		billboardShader.setInt("billboardNoise", billboardGrassNoise2.getTextureID());
	}
	billboardShader.setFloat("currentTime", currentTime);
	billboardShader.setMat4("projection", projection);
	billboardShader.setMat4("view", view);
	billboardShader.setMat4("model", model);
	billboardShader.setInt("windType", config.windType);
	billboardShader.setFloat("windStrength", config.windStrength);
	billboardShader.setFloat("swayReach", config.swayReach);
	billboardShader.setFloat("perlinSampleScale", config.perlinSampleScale);
	billboardShader.setVec3("lightPos", config.lightPosition);
	billboardShader.setFloat("ambientStrength", config.ambientStrength);
	billboardSquare.drawSceneObject();
}

/* Draws three billboards squares in an aterisk configuration. 
 */
void drawBillboardCollection(glm::mat4 projection, glm::mat4 view, glm::mat4 model) {

	drawBillboardSquare(projection, view, model);
	drawBillboardSquare(projection, view, model * glm::rotateY(3 * glm::quarter_pi<float>()));
	drawBillboardSquare(projection, view, model * glm::rotateY(6 * glm::quarter_pi<float>()));
}

/* Draws skybox. Should be called first to ensure skybox is drawn behind the
 * rest of the scene. Draws either a day or night skybox depending
 * on what the user sets the GUI to. 
 */
void drawSkybox(glm::mat4 projection, glm::mat4 view) {
	GLCall(glDepthFunc(GL_LEQUAL));  // Change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader.use();
	skyboxShader.setMat4("projection", projection);
	skyboxShader.setMat4("view", view);
	if (config.skyboxType == 0) {
		cubemapTextureDay.bindTextureCubeMap();
		skyboxShader.setInt("skybox", cubemapTextureDay.getTextureID());
	}
	else if (config.skyboxType == 1) {
		cubemapTextureNight.bindTextureCubeMap();
		skyboxShader.setInt("skybox", cubemapTextureNight.getTextureID());
	}
	skybox.drawSceneObjectArrays();
	GLCall(glDepthFunc(GL_LESS));
}

/* Draws the GUI that lets the user control the light, skybox, grass and wind.
 */
void drawGui() {
	// Slider will be 65% of the window width (this is the default)

	ImGui::NewFrame();

	{
		ImGui::Begin("Settings");

		ImGui::Text("Light Settings");
		ImGui::SliderFloat("Ambient Light Strength", &config.ambientStrength, 0.1, 1.0);
		ImGui::DragFloat3("Light Position", (float*)&config.lightPosition, 0.1 ,-100, 100);

		ImGui::Separator();
		ImGui::Text("Skybox Settings");
		if (ImGui::RadioButton("Day", config.skyboxType == 0)) { config.skyboxType = 0; } ImGui::SameLine();
		if (ImGui::RadioButton("Night", config.skyboxType == 1)) { config.skyboxType = 1; }

		ImGui::Separator();
		ImGui::Text("Grass Settings");
		ImGui::Text("Grass Type: ");
		if (ImGui::RadioButton("Blades", config.grassType == 0 )) { config.grassType = 0; } ImGui::SameLine();
		if (ImGui::RadioButton("Billboard", config.grassType == 1)) { config.grassType = 1; }
		if (config.grassType == 0) {
			ImGui::SliderInt("Patch density", &config.patchDensity, 1, MAX_PATCH_DENSITY_BLADES);
		}
		else if (config.grassType == 1) {
			ImGui::SliderInt("Patch density", &config.patchDensity, 1, MAX_PATCH_DENSITY_BILLBOARDS);
		}
	
		if (config.grassType == 1) {
			ImGui::Separator();
			ImGui::Text("Wind Settings");
			ImGui::Text("Wind Type: ");
			if (ImGui::RadioButton("Simple Trigonometric Sway", config.windType == 0)) { config.windType = 0; } ImGui::SameLine();
			if (ImGui::RadioButton("Complex Trigonometric Sway 1", config.windType == 1)) { config.windType = 1; }
			if (ImGui::RadioButton("Complex Trigonometric Sway 2", config.windType == 2)) { config.windType = 2; } ImGui::SameLine();
			if (ImGui::RadioButton("Perlin Noise Sway", config.windType == 3)) { config.windType = 3; }
			if (config.windType == 3) {
				ImGui::SliderFloat("Wind Strength", &config.windStrength, 0.0, 0.1);
				ImGui::SliderFloat("Perlin Sample Scale", &config.perlinSampleScale, 0.05, 1.0);
				ImGui::Text("Perlin Texture: ");
				if (ImGui::RadioButton("1", config.perlinTexture == 1)) { config.perlinTexture = 1; } ImGui::SameLine();
				if (ImGui::RadioButton("2", config.perlinTexture == 2)) { config.perlinTexture = 2; } 
			}
			else {
				ImGui::SliderFloat("Wind Strength", &config.windStrength, 0.0, 10.0);
				ImGui::SliderFloat("Sway Reach", &config.swayReach, 0.01, 0.3);
			}
		}
		ImGui::Separator();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void cursorInRange(float screenX, float screenY, int screenW, int screenH, float min, float max, float &x, float &y){
    float sum = max - min;
    float xInRange = (float) screenX / (float) screenW * sum - sum/2.0f;
    float yInRange = (float) screenY / (float) screenH * sum - sum/2.0f;
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