#ifndef SCENE_H
#define SCENE_H 

#include <vector>
#include "scene_object.h"
#include "shader.h"
#include "perlin_noise.h"

enum class WindType {
	TRIG_SIMPLE,
	TRIG_COMPLEX_1,
	TRIG_COMPLEX_2,
	PERLIN
};

enum class SkyboxType {
	DAY,
	NIGHT
};

enum class BladeDistribution {
	HARRY_STYLES_WITH_RANDOS,
	HARRY_STYLES,
	ONE_DIRECTION
};

/* All variables that can be configured using the GUI
 */
struct Config {
	PerlinConfig perlinConfig;
	int patchDensity = 10000;
	WindType windType = WindType::TRIG_SIMPLE;
	float windStrength = 0.0f;
	float swayReach = 0.3f;
	float perlinSampleScale = 1.0f;
	int perlinTexture = 1;  // Either 1 or 2
	glm::vec3 lightPosition = glm::vec3(0.0, 15.0, 0.0);
	float ambientStrength = 0.5f;
	SkyboxType skyboxType = SkyboxType::NIGHT;
	int numPatches = 9;
	glm::vec4 lightColor = { 1.0, 1.0, 1.0, 1.0 };
	float lightIntensity = 10;
	BladeDistribution bladeDistribution = BladeDistribution::HARRY_STYLES_WITH_RANDOS;
	bool visualizeTexture = false;
};


class Scene {
public:
	void addSceneObject(SceneObject* sceneObject);
	std::vector<SceneObject*> sceneObjects;
	std::vector<SceneObject*> patches;
	std::vector<SceneObject*> blades;

	void updateDynamic();
	void render();
	Texture* currentTexture = nullptr;
	Texture* cubemapTextureDay = nullptr;
	Texture* cubemapTextureNight = nullptr;
	Texture* perlinNoise = nullptr;
	GLuint perlinNoiseID = 0;
	glm::mat4 projection = glm::mat4(1);
	glm::mat4 view = glm::mat4(1);
	SceneObject* light = nullptr;


	Config config;


};


#endif