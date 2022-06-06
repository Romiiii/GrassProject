#ifndef SCENE_H
#define SCENE_H 

#include <vector>
#include "scene_object.h"
#include "shader.h"
#include "perlin_noise.h"
#include "fluid_grid.h"


/**
* \brief Represents different types of techniques for simulating wind. 
* Currently suports trigonometric functions and perlin noise
*/
enum class WindType {
	TRIG_SIMPLE,
	TRIG_COMPLEX_1,
	TRIG_COMPLEX_2,
	PERLIN
};

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
	PerlinConfig perlinConfig;
	FluidGridConfig fluidGridConfig;
	int patchDensity = 10000;
	WindType windType = WindType::TRIG_SIMPLE;
	float windStrength = 0.0f;
	float swayReach = 0.5f;
	glm::vec3 lightPosition = glm::vec3(0.0, 15.0, 0.0);
	float ambientStrength = 0.05f;
	SkyboxType skyboxType = SkyboxType::NIGHT;
	int numPatches = 1;
	glm::vec4 lightColor = { 1.0, 1.0, 1.0, 1.0 };
	float lightIntensity = 10;
	BladeDistribution bladeDistribution = BladeDistribution::HARRY_STYLES;
	bool visualizeTexture = false;
	SimulationMode simulationMode = SimulationMode::FLUID_GRID;
	glm::vec2 windDirection = { 0.0f, -1.0f };
	bool debugBlades = false;
	Texture* windX = nullptr;
	Texture* windY = nullptr;
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
	/**
	* \brief All variables that can be configured using the GUI
	*/
	Config config;
};

#endif