#include "grass_simulation.h"
#include <rendering/texture.h>
#include "patch.h"
#include "fluid_grid.h"
#include "logger.h"
#include <rendering/scene_object_indexed.h>
#include <rendering/primitives.h>
#include <rendering/scene_object_instanced.h>
#include <rendering/scene_object_arrays.h>
#include <imgui.h>
#include <gui_helpers.h>

namespace GrassSimulation {
	Scene* g_scene;
	Inputs& inputs = Inputs::instance();

	/**
	 * \brief Id of the instance matrix buffer for rendering blade-instances
	*/
	unsigned int instanceMatrixBuffer;

	bool clearNextSimulate = true;


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


#define CHECKER_PATTERN_TEXTURE_WIDTH 512

	/**
	 * \brief Blades density
	 */
	const int MAX_BLADES_PER_PATCH = 1000;

	/**
	 * \brief Max number patches
	 */
	const int MAX_PATCHES = 81;


	/**
	 * \brief The patch creator
	 */
	Patch patchTemplate;

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
	 * \brief Perlin noise texture data, used for uploading perlin noise data
	*/
	float* perlinNoiseTextureData;

	/**
	 * \brief Perlin noise seed texture data, used for uploading perlin noise
	 * seed data.
	*/
	float* perlinNoiseSeedTextureData;


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
	 * \brief Initialize the scene->
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


	// RETURNS NULL IF NONE IS SELECTED
	Fan* getSelectedFan()
	{
		// Ensure we have a fan selected
		int fanIndex = g_scene->config.fluidGridConfig.selectedFanIndex;
		if (fanIndex == -1)
			return nullptr;

		Fan* fan = &g_scene->config.fluidGridConfig.fans[fanIndex];
		return fan;
	}


	Ray getMouseRay();

	std::optional<glm::vec3> mouseHitsGround();

	glm::vec3 g_fanDragStart = {};
	bool g_fanIsDragging = false;
	void drawFanWindow();

	struct GrassSimullationConfig
	{
		SimulationMode simulationMode = SimulationMode::FLUID_GRID;
		glm::vec2 windDirection = glm::normalize(glm::vec2(0.5f, 0.0f));
		float windStrength = 0.0f;//0.05f;
		float patchSize = 10;
		float bladeHeight = 5;
		float swayReach = 0.5f;
		Texture* windX = nullptr;
		Texture* windY = nullptr;
		PerlinConfig perlinConfig;
		FluidGridConfig fluidGridConfig;
		int checkerSize = 32;
		Texture* checkerPatternTexture = nullptr;

		int numBladesPerPatch = -1;
		int numPatches = -1;

		BladeDistribution bladeDistribution = BladeDistribution::HARRY_STYLES_WITH_RANDOS;

		bool visualizeTexture = false;
		bool debugBlades = false;
		float worldMin = 0.0f;
		float worldMax = 0.0f;

	};

	void handleInputs()
	{
		const float densityStep = 25;

		if (inputs.buttonIsPressed(GLFW_MOUSE_BUTTON_MIDDLE))
		{
			if (auto hit = mouseHitsGround(); hit.has_value())
			{
				// We hit something
				glm::vec3 hitPos = hit.value();
				if (inputs.buttonIsHeld(GLFW_KEY_LEFT_CONTROL))
				{
					// Create new fan and select it
					Fan fan{};
					fan.density = 250.0f;
					fan.position = g_scene->mapPositionFromWorldSpace({ hitPos.x, hitPos.z });
					g_scene->config.fluidGridConfig.fans.push_back(fan);
					g_scene->config.fluidGridConfig.selectedFanIndex = (int)g_scene->config.fluidGridConfig.fans.size() - 1;
				}
				else if (Fan* fan = getSelectedFan())
				{
					fan->position = g_scene->mapPositionFromWorldSpace({ hitPos.x, hitPos.z });
				}

				g_fanDragStart = hitPos;
				g_fanIsDragging = true;
			}
		}
		else if (inputs.buttonIsHeld(GLFW_MOUSE_BUTTON_MIDDLE))
		{

			Fan* fan = getSelectedFan();
			if (fan && g_fanIsDragging)
			{
				auto hit = mouseHitsGround();
				if (hit.has_value()) {
					auto dif = hit.value() - g_fanDragStart;
					fan->velocity = g_scene->mapVelocityFromWorldSpace({ dif.x, dif.z });
				}
			}
		}
		else if (inputs.buttonIsReleased(GLFW_MOUSE_BUTTON_MIDDLE))
		{
			g_fanIsDragging = false;
		}

		if (inputs.keyIsPressed(GLFW_KEY_UP))
		{
			auto fan = getSelectedFan();
			if (fan)
			{
				fan->density += densityStep;
			}
		}

		if (inputs.keyIsPressed(GLFW_KEY_DOWN))
		{
			auto fan = getSelectedFan();
			if (fan)
			{
				fan->density -= densityStep;
			}
		}
	}

	void initShadersAndTextures()
	{
		bladesVertexShader = new Shader("assets/shaders/blades.vert", GL_VERTEX_SHADER);
		bladesFragmentShader = new Shader("assets/shaders/blades.frag", GL_FRAGMENT_SHADER);

		bladesShaderProgram = new ShaderProgram({ bladesVertexShader, bladesFragmentShader }, "BLADES SHADER");

		patchVertexShader = new Shader("assets/shaders/patch.vert", GL_VERTEX_SHADER);
		patchFragmentShader = new Shader("assets/shaders/patch.frag", GL_FRAGMENT_SHADER);

		patchShaderProgram = new ShaderProgram({ patchVertexShader, patchFragmentShader }, "PATCH SHADER");

		g_scene->config.perlinConfig.texture = new Texture("Perlin Noise", GL_TEXTURE_2D);
		g_scene->config.perlinConfig.texture->loadTextureSingleChannel(PERLIN_NOISE_TEXTURE_WIDTH);

		perlinNoiseComputeShader = new Shader("assets/shaders/perlin_noise.comp", GL_COMPUTE_SHADER);
		perlinNoiseComputeShaderProgram = new ShaderProgram({ perlinNoiseComputeShader }, "PERLIN NOISE COMPUTE SHADER");

		g_scene->config.checkerPatternTexture = new Texture("Checker Pattern", GL_TEXTURE_2D);
		g_scene->config.checkerPatternTexture->loadTextureSingleChannel(CHECKER_PATTERN_TEXTURE_WIDTH);

		checkerPatternComputeShader = new Shader("assets/shaders/checker_pattern.comp", GL_COMPUTE_SHADER);
		checkerPatternComputeShaderProgram = new ShaderProgram({ checkerPatternComputeShader }, "CHECKER PATTERN COMPUTE SHADER");


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

	void initSceneObjects(Patch& patch)
	{
		patch.init(MAX_BLADES_PER_PATCH, patchShaderProgram);

		if (g_scene->config.bladeDistribution == BladeDistribution::HARRY_STYLES_WITH_RANDOS)
		{
			patch.initHarryEdwardStylesBladeMatrices(g_scene->config.patchSize);
		}
		else if (g_scene->config.bladeDistribution == BladeDistribution::HARRY_STYLES)
		{
			patch.initHarryEdwardStylesBladeMatrices(g_scene->config.patchSize, false);
		}
		else if (g_scene->config.bladeDistribution == BladeDistribution::ONE_DIRECTION)
		{
			patch.initOneDirectionBladeMatrices(g_scene->config.patchSize);
		}

		createInstanceMatrixBuffer(patch.getBladeMatrices(), MAX_BLADES_PER_PATCH);

		for (int i = 0; i < MAX_PATCHES; i++)
		{
			// Position of the entire thing
			glm::vec2 position = calculateSpiralPosition(i) * g_scene->config.patchSize;
			SceneObjectIndexed* patchSceneObject = new SceneObjectIndexed(
				grassPatchPositions, grassPatchColors, grassPatchIndices, grassPatchNormals, *patchShaderProgram);
			glm::mat4 translation = glm::translate(position.x, 0, position.y);
			// Set the patch object
			patchSceneObject->model = translation * glm::scale(g_scene->config.patchSize, g_scene->config.patchSize, g_scene->config.patchSize);
			g_scene->patches.push_back(patchSceneObject);

			// Set the blades on the patch
			SceneObjectInstanced* blades = new SceneObjectInstanced(
				grassPositions, grassColors, grassIndices, grassNormals, instanceMatrixBuffer, *bladesShaderProgram, &grassUVs);
			// Do not scale the blades
			blades->model = translation * glm::scale(1, g_scene->config.bladeHeight, 1);
			g_scene->blades.push_back(blades);
		}

		SceneObjectArrays* fanDebugIcon = new SceneObjectArrays(fanDebugIconVertexPositions, *g_scene->lightShaderProgram);
		g_scene->fanDebugIcon = fanDebugIcon;
	}
	void generateCheckerPatternTexture()
	{
		checkerPattern2DGPU(checkerPatternComputeShaderProgram, g_scene->config.checkerPatternTexture->getTextureID(), g_scene->config.checkerSize);
	}

	void checkerPattern2DGPU(ShaderProgram* computeShaderProgram, GLuint computeShaderTexture, int checkerSize)
	{
		computeShaderProgram->use();

		GLCall(glBindImageTexture(0, computeShaderTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8));

		computeShaderProgram->setInt("checkerSize", checkerSize);

		GLCall(glDispatchCompute(CHECKER_PATTERN_TEXTURE_WIDTH / 16, CHECKER_PATTERN_TEXTURE_WIDTH / 16, 1));
	}
	void generatePerlinNoiseTexture()
	{
		using namespace std::chrono;
		// Initialize seed data
		for (int i = 0; i < PERLIN_NOISE_TEXTURE_WIDTH * PERLIN_NOISE_TEXTURE_WIDTH; i++)
			perlinNoiseSeedTextureData[i] = (float)rand() / (float)RAND_MAX;

		perlinNoise2DGPU(*perlinNoiseSeedTexture, perlinNoiseSeedTextureData, perlinNoiseComputeShaderProgram,
			g_scene->config.perlinConfig.texture->getTextureID(), g_scene->config.perlinConfig.octaves,
			g_scene->config.perlinConfig.bias);
	}
	void setWindTexturesForSimulationMode()
	{
		if (g_scene->config.simulationMode == SimulationMode::FLUID_GRID)
		{
			g_scene->config.windX = g_scene->config.fluidGridConfig.velX;
			g_scene->config.windY = g_scene->config.fluidGridConfig.velY;
		}
		else if (g_scene->config.simulationMode == SimulationMode::PERLIN_NOISE)
		{
			g_scene->config.windX = g_scene->config.perlinConfig.texture;
			g_scene->config.windY = nullptr;
		}
		else if (g_scene->config.simulationMode == SimulationMode::CHECKER_PATTERN)
		{
			g_scene->config.windX = g_scene->config.checkerPatternTexture;
			g_scene->config.windY = nullptr;
		}

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

	void setWorldMinMax()
	{
		/* Since we're making a spiral pattern, we can find the leftmost edge by taking the sqrt, giving us the width.
			 Dividing that in two, so we have what would be to either side, taking the floor of that, since we know we
			 start at (0, 0), so the left will have fewer patches for odd widths. We then negate it, because it's in the
			 negative halfspace.
		 */
		g_scene->config.worldMin = (float)(-floor(sqrt(MAX_PATCHES) / 2) * (double)g_scene->config.patchSize);

		/* The process for the max is similar, though we take the ceiling rather than the floor. Because since we start at (0, 0) there are more
			 patches to the right.
		 */
		g_scene->config.worldMax = (float)(ceil(sqrt(MAX_PATCHES) / 2) * (double)g_scene->config.patchSize);
	}



	void simulateGrass(float deltaTime)
	{
		if (clearNextSimulate)
		{
			fluidGrid->clearCurrent();
		}

		// FAN!
		for (size_t fanIndex = 0; fanIndex < g_scene->config.fluidGridConfig.fans.size(); fanIndex++)
		{
			Fan& fan = g_scene->config.fluidGridConfig.fans[fanIndex];
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

		if (!g_scene->config.isPaused)
		{
			fluidGrid->simulate(deltaTime);
		}
	}

	bool shouldSimulateGrass()
	{
		return g_scene->config.numPatches > 0 && g_scene->config.numBladesPerPatch > 0;
	}

	bool setup(Scene* scene)
	{
		g_scene = scene;
		fluidGrid = new FluidGrid(128, 0, 0, &g_scene->config.fluidGridConfig);

		initShadersAndTextures();
		initSceneObjects(patchTemplate);

		if (g_scene->config.simulationMode == SimulationMode::CHECKER_PATTERN)
		{
			generateCheckerPatternTexture();
		}
		else if (g_scene->config.simulationMode == SimulationMode::PERLIN_NOISE)
		{
			generatePerlinNoiseTexture();
		}
		setWorldMinMax();

		float worldWidth = scene->config.worldMax - scene->config.worldMin;
		float center = (scene->config.worldMax + scene->config.worldMin) / 2.0f;
		scene->worldRekt.center = { center, 0.0f, center };
		scene->worldRekt.height = scene->worldRekt.width = worldWidth;

		Fan fan{};
		fan.active = true;
		fan.position.x = 0.2f;
		fan.position.y = 0.2f;
		fan.density = 250.0f;
		fan.velocity.x = 150.0f;
		fan.velocity.y = 0.0f;
		scene->config.fluidGridConfig.selectedFanIndex = 0;

		scene->config.fluidGridConfig.fans.push_back(fan);

		// Set num blades per patch
		if (scene->config.numBladesPerPatch < 0)
		{
			scene->config.numBladesPerPatch = MAX_BLADES_PER_PATCH;
		}

		if (scene->config.numBladesPerPatch > MAX_BLADES_PER_PATCH)
		{
			LOG_WARNING("Num blades per patch should be between 0 and %d but was %d. Clamping.",
				MAX_BLADES_PER_PATCH,
				scene->config.numBladesPerPatch);
			scene->config.numBladesPerPatch = glm::clamp(scene->config.numBladesPerPatch, 0, MAX_BLADES_PER_PATCH);
		}

		if (scene->config.numPatches < 0)
		{
			scene->config.numPatches = MAX_PATCHES;
		}
		if (scene->config.numPatches > MAX_PATCHES)
		{
			LOG_WARNING("Num patches should be between 0 and %d but was %d. Clamping.",
				MAX_PATCHES, scene->config.numPatches);
			scene->config.numPatches = glm::clamp(scene->config.numPatches, 0, MAX_PATCHES);
		}

		return true;
	}

	bool update(float deltaTime)
	{
		if (shouldSimulateGrass())
		{
			simulateGrass(deltaTime);

			glm::vec2 mousePos = inputs.getNormalizedMousePosition();
			Ray mouseRay = getMouseRay();
			auto hitPos = mouseHitsGround();

			debugText("Mouse Pos: (%.3f, %.3f)", mousePos.x, mousePos.y);
			debugText("Origin: (%.3f, %.3f, %.3f)", mouseRay.origin.x, mouseRay.origin.y, mouseRay.origin.z);
			debugText("Direction: (%.3f, %.3f, %.3f)", mouseRay.direction.x, mouseRay.direction.y, mouseRay.direction.z);
			if (hitPos.has_value()) {
				glm::vec3 pos = hitPos.value();
				debugText("Hits: (%.3f, %.3f, %.3f)", pos.x, pos.y, pos.z);
			}
			else {
				debugText("Hits: MISS");
			}
			debugText("WorldRekt: (%.2f, %.2f, %.2f)", g_scene->worldRekt.center.x, g_scene->worldRekt.center.y, g_scene->worldRekt.center.z);
			debugText("    Size (% .3f, % .3f)", g_scene->worldRekt.width, g_scene->worldRekt.height);
		}

		return true;
	}

	void reloadShaders() {
		bladesShaderProgram->reloadShaders();
		patchShaderProgram->reloadShaders();
	}

	void cleanup()
	{
		delete bladesVertexShader;
		delete bladesFragmentShader;
		delete bladesShaderProgram;
		delete patchVertexShader;
		delete patchFragmentShader;
		delete patchShaderProgram;
	}

	void drawFluidGridWindow()
	{
		auto& config = g_scene->config;
		auto& fluidConf = config.fluidGridConfig;
		if (config.simulationMode == SimulationMode::FLUID_GRID)
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

			//ImGui::Text("Total Density %.1f", fluidGrid->totalDensity());


			if (ImGui::Button("Reset"))
			{
				fluidGrid->initialize();
			}



			if (static float diffRatio = 0.56f;
				ImGui::DragFloat("Diffusion Ratio", &diffRatio, 0.01f, 0.0f, 1.0f))
			{
				*fluidGrid->getDiffPointer() = glm::mix(0.0f, 0.001f, diffRatio);
			}
			ImGui::LabelText("Diffusion Value", "%.5f", *fluidGrid->getDiffPointer());
			ImGui::DragFloat("Viscosity", fluidGrid->getViscPointer(), 0.0001f, 0.0f, 0.005f);
			ImGui::DragFloat("Velocity Multiplier", &config.fluidGridConfig.velocityMultiplier, 0.1f, 0, 100.0f);
			ImGui::DragFloat2("Velocity Clamp", (float*)&config.fluidGridConfig.velocityClampRange, 0.1f, 0, 2.0f);

			ImGui::End();
		}
	}


	void drawFanWindow()
	{
		auto& conf = g_scene->config;
		auto& fluidConf = conf.fluidGridConfig;

		ImGui::Begin("Fans");
		ImGui::Checkbox("Draw Fans", &conf.fluidGridConfig.shouldDrawFans);

		for (int fanIndex = 0; fanIndex < fluidConf.fans.size(); fanIndex++)
		{
			Fan& fan = conf.fluidGridConfig.fans[fanIndex];

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

	void drawSimulationSettingsWindow()
	{
		auto& config = g_scene->config;
		ImGui::Text("Simulation Mode Settings");
		if (ImGui::RadioButton("Perlin Noise", config.simulationMode == SimulationMode::PERLIN_NOISE))
		{
			config.simulationMode = SimulationMode::PERLIN_NOISE;
			generatePerlinNoiseTexture();

			config.windX = config.perlinConfig.texture;
			config.windY = nullptr;
		}
		ImGui::SameLine();
		drawTooltip("Blades respond to the generated perlin noise.");
		if (ImGui::RadioButton("Checker Pattern", config.simulationMode == SimulationMode::CHECKER_PATTERN))
		{
			config.simulationMode = SimulationMode::CHECKER_PATTERN;
			generateCheckerPatternTexture();

			config.windX = config.checkerPatternTexture;
			config.windY = nullptr;
		}
		ImGui::SameLine();
		drawTooltip("Blades respond to the generated checker pattern.");
		if (ImGui::RadioButton("Fluid Grid", config.simulationMode == SimulationMode::FLUID_GRID))
		{
			config.simulationMode = SimulationMode::FLUID_GRID;
			config.windX = config.fluidGridConfig.velX;
			config.windY = config.fluidGridConfig.velY;
		}
		drawTooltip("Blades respond to the fluid grid simulation.");


		ImGui::Text("Harry Styles Settings");
		if (ImGui::RadioButton("Harry Styles With Randos",
			config.bladeDistribution == BladeDistribution::HARRY_STYLES_WITH_RANDOS))
		{
			config.bladeDistribution = BladeDistribution::HARRY_STYLES_WITH_RANDOS;
			patchTemplate.initHarryEdwardStylesBladeMatrices(config.patchSize);
			transferInstanceMatrixBuffer(patchTemplate.getBladeMatrices(), MAX_BLADES_PER_PATCH);
		}
		ImGui::SameLine();
		drawTooltip("Blades are placed uniformly on the patch with random rotations.");
		if (ImGui::RadioButton("Harry Styles", config.bladeDistribution == BladeDistribution::HARRY_STYLES))
		{
			config.bladeDistribution = BladeDistribution::HARRY_STYLES;
			patchTemplate.initHarryEdwardStylesBladeMatrices(config.patchSize, false);
			transferInstanceMatrixBuffer(patchTemplate.getBladeMatrices(), MAX_BLADES_PER_PATCH);
		}
		ImGui::SameLine();
		drawTooltip("Blades are placed uniformly on the patch without random rotations.");
		if (ImGui::RadioButton("One Direction", config.bladeDistribution == BladeDistribution::ONE_DIRECTION))
		{
			config.bladeDistribution = BladeDistribution::ONE_DIRECTION;
			patchTemplate.initOneDirectionBladeMatrices(config.patchSize);
			transferInstanceMatrixBuffer(patchTemplate.getBladeMatrices(), MAX_BLADES_PER_PATCH);
		}
		drawTooltip("Blades are placed in a line in the middle of the patch without random rotations.");

		ImGui::Checkbox("Debug Blades", &config.debugBlades);
		ImGui::Checkbox("Visualize Texture On Patch", &config.visualizeTexture);

		if (config.simulationMode == SimulationMode::FLUID_GRID && config.visualizeTexture)
		{
			ImGui::Text("Visualize");
			if (ImGui::RadioButton("Density", config.fluidGridConfig.visualizeDensity))
			{
				config.fluidGridConfig.visualizeDensity = true;
				if (config.windX) config.windX->unbind();
				if (config.windY) config.windY->unbind();

				config.windX = config.fluidGridConfig.density;
				config.windY = nullptr;
			}
			if (ImGui::RadioButton("Velocity", !config.fluidGridConfig.visualizeDensity))
			{
				config.fluidGridConfig.visualizeDensity = false;
				if (config.windX) config.windX->unbind();
				if (config.windY) config.windY->unbind();

				config.windX = config.fluidGridConfig.velX;
				config.windY = config.fluidGridConfig.velY;
			}
		}

		if (ImGui::SliderFloat("Blade height", &config.bladeHeight, 0.1f, 10.0f))
		{
			for (int i = 0; i < MAX_PATCHES; i++)
			{
				glm::vec2 position = calculateSpiralPosition(i) * config.patchSize;
				glm::mat4 translation = glm::translate(position.x, 0, position.y);
				g_scene->blades[i]->model = translation * glm::scale(1, config.bladeHeight, 1);
			}
		}

		if (ImGui::SliderFloat("PatchSize", &config.patchSize, 1.0f, 100.0f))
		{
			switch (config.bladeDistribution)
			{
			case BladeDistribution::ONE_DIRECTION:
				patchTemplate.initOneDirectionBladeMatrices(config.patchSize);
				break;
			case BladeDistribution::HARRY_STYLES_WITH_RANDOS:
				patchTemplate.initHarryEdwardStylesBladeMatrices(config.patchSize, true);
				break;
			case BladeDistribution::HARRY_STYLES:
				patchTemplate.initHarryEdwardStylesBladeMatrices(config.patchSize, false);
				break;
			}

			transferInstanceMatrixBuffer(patchTemplate.getBladeMatrices(), MAX_BLADES_PER_PATCH);

			for (int i = 0; i < MAX_PATCHES; i++)
			{
				glm::vec2 position = calculateSpiralPosition(i) * config.patchSize;
				glm::mat4 translation = glm::translate(position.x, 0, position.y);
				g_scene->patches[i]->model = translation * glm::scale(config.patchSize, config.patchSize, config.patchSize);
				g_scene->blades[i]->model = translation * glm::scale(1, config.bladeHeight, 1);
			}

			setWorldMinMax();
		}


		drawTooltip("The sizes of each individual Patch. Same number of blades of grass.");

		ImGui::SliderInt("Number of patches", &config.numPatches, 0, MAX_PATCHES);
		ImGui::DragInt("Number blades per patch", &config.numBladesPerPatch, 1, 0, MAX_BLADES_PER_PATCH);
		config.numBladesPerPatch = glm::clamp(config.numBladesPerPatch, 0, (int)MAX_BLADES_PER_PATCH);
		ImGui::SliderFloat("Sway Reach", &config.swayReach, 0.0f, 2.0f);
		drawTooltip("How far the blades will move in the wind.");

		if (config.simulationMode != SimulationMode::FLUID_GRID)
		{
			ImGui::SliderFloat("Wind Strength", &config.windStrength, 0, 0.5f);
			drawTooltip("Strength of the wind");

			ImGui::DragFloat2("Wind Direction", (float*)&config.windDirection,
				0.1f, -1.0f, 1.0f);
			drawTooltip("Direction of the wind");
		}

		if (glm::epsilonEqual(glm::length(config.windDirection), 0.0f, 0.0001f))
		{
			config.windDirection = { 1, 0 };
		}
		if (ImGui::Button("Normalize Wind Direction"))
			config.windDirection = glm::normalize(config.windDirection);

		drawTooltip("Normalize the wind direction");

		if (config.simulationMode == SimulationMode::PERLIN_NOISE && ImGui::CollapsingHeader(
			"Perlin Noise Settings"))
		{
			ImGui::SliderInt("Octaves", &config.perlinConfig.octaves, 2, 10);
			drawTooltip("Octaves for fun.");

			ImGui::SliderFloat("Bias", &config.perlinConfig.bias, 0.2f, 2.0f);
			drawTooltip("Bias for fun.");

			if (ImGui::Button("Generate Perlin Noise"))
			{
				generatePerlinNoiseTexture();
			}
			float width = PERLIN_NOISE_TEXTURE_WIDTH;

			ImGui::Image((ImTextureID)(long long)config.perlinConfig.texture->getTextureID(),
				{ width, width },
				{ 0.0f, 1.0f },
				{ 1.0f, 0.0f });
		}

		if (config.simulationMode == SimulationMode::CHECKER_PATTERN && ImGui::CollapsingHeader("Checker Pattern Settings"))
		{
			if (ImGui::SliderInt("CheckerSize", &config.checkerSize, 1, 512))
			{
				generateCheckerPatternTexture();
			}
			drawTooltip("Checker size for fun. Only powers of two look nice.");
		}
	}

	void drawGui()
	{
		drawFanWindow();
		drawFluidGridWindow();
		drawSimulationSettingsWindow();
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


	Ray getMouseRay()
	{
		Ray ray = {};

		glm::vec2 cursorPos = inputs.getNormalizedMousePosition();

		glm::mat4 inverseProjection = glm::inverse(g_scene->projection);
		glm::mat4 inverseView = glm::inverse(g_scene->view);

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

		return ray.intersectsRectangle(g_scene->worldRekt);
	}
}