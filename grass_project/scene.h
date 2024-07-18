#ifndef SCENE_H
#define SCENE_H 

#include <vector>
#include "rendering/scene_object.h"
#include "rendering/shader.h"
#include "grass_simulation/perlin_noise.h"
#include "grass_simulation/fluid_grid.h"
#include "grass_simulation/grass_math.h"

/**
* \brief Sets the appearance of the skybox
*/
enum class SkyboxType {
	DAY,
	NIGHT
};

/**
* Represents how the blades are distributed
*
* HARRY_STYLES_WITH_RANDOS: Blades are distributed uniformly within the patch
*							and have random rotations.
* HARRY_STYLES:				Blades are distributed uniformly within the patch
*							and have no rotation.
* ONE_DIRECTION:			Blades are places in a straight line in	the
*							middle of the patch and have no rotation.
*/
enum class BladeDistribution {
	HARRY_STYLES_WITH_RANDOS,
	HARRY_STYLES,
	ONE_DIRECTION
};

enum class SimulationMode {
	PERLIN_NOISE,
	CHECKER_PATTERN,
	FLUID_GRID
};

/**
* All variables that can be configured using the GUI
 */
struct Config {
	SimulationMode simulationMode = SimulationMode::FLUID_GRID;
	float swayReach = 0.5f;
	Texture* windX = nullptr;
	Texture* windY = nullptr;
	float currentTime = 0;
	bool isPaused = false;
	float patchSize = 10;
	float bladeHeight = 5;
	glm::vec2 windDirection = glm::normalize(glm::vec2(0.5f, 0.0f));
	float windStrength = 0.0f;//0.05f;

	PerlinConfig perlinConfig;
	FluidGridConfig fluidGridConfig;

	int checkerSize = 32;
	Texture* checkerPatternTexture = nullptr;

	int numBladesPerPatch = -1;
	int numPatches = -1;

	SkyboxType skyboxType = SkyboxType::NIGHT;
	float ambientStrength = 0.55f;
	glm::vec3 lightPosition = glm::vec3(0.0, 15.0, 0.0);
	glm::vec4 lightColor = { 1.0, 1.0, 1.0, 1.0 };
	float lightIntensity = 10;

	BladeDistribution bladeDistribution = BladeDistribution::HARRY_STYLES_WITH_RANDOS;

	bool visualizeTexture = false;
	bool debugBlades = false;
	float worldMin = 0.0f;
	float worldMax = 0.0f;
};

/**
* Contains all the elements contained in a scene, like the sceneObject
* patches, blades, textures and lights. Updates and renders the scene accordingly.
*/
class Scene {
public:
	void addSceneObject(SceneObject* sceneObject);
	/**
	* Updates the dynamic scene objects, based on the user's settings.
	* Currently only updates lights based on the position the user set.
	*/
	void updateDynamic();
	void render();

	std::vector<SceneObject*> sceneObjects;
	std::vector<SceneObject*> patches;
	std::vector<SceneObject*> blades;

	Texture* currentSkyboxTexture = nullptr;
	Texture* cubemapTextureDay = nullptr;
	Texture* cubemapTextureNight = nullptr;

	glm::mat4 projection = glm::mat4(1);
	glm::mat4 view = glm::mat4(1);
	SceneObject* light = nullptr;
	SceneObject* fanDebugIcon = nullptr;

	ShaderProgram* lightShaderProgram;

	/**
	* \brief All variables that can be configured using the GUI
	*/
	Config config;

	Rectangle worldRekt = {};

	inline glm::vec2 mapPositionFromWorldSpace(glm::vec2 pos)
	{
		return {
			map(pos.x, config.worldMin, config.worldMax, 0.0, 1.0f),
			map(pos.y, config.worldMin, config.worldMax, 1.0, 0.0f)
		};
	}

	inline glm::vec2 mapPositionToWorldSpace(glm::vec2 pos)
	{
		return map(pos, 0.0, 1.0f, config.worldMin, config.worldMax);
	}

	inline glm::vec2 mapVelocityFromWorldSpace(glm::vec2 dif)
	{
		return map(dif, 0, worldRekt.width, 0, config.fluidGridConfig.wholeWorldToVelocityMapping);
	}

	inline glm::vec2 mapVelocityToWorldSpace(glm::vec2 dif)
	{
		return map(dif, 0, this->config.fluidGridConfig.wholeWorldToVelocityMapping, 0, worldRekt.width);
	}


};

#endif