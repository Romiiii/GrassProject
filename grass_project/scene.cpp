#include "scene.h"
#include "glmutils.h"
#include <glm\glm.hpp>
#include "grass_math.h"

void Scene::addSceneObject(SceneObject* sceneObject) {
	sceneObjects.push_back(sceneObject);
}

void setupFanDebugIconForFan(Scene* scene, Fan& fan)
{
	auto &config = scene->config;

	// Position
	auto fanPosition = scene->mapPositionToWorldSpace(fan.position);
	fanPosition.y -= config.patchSize;
	
	// Rotation
	float angle = glm::atan(fan.velocity.x, fan.velocity.y);

	// Scaling
	float mappingValue = config.fluidGridConfig.wholeWorldToVelocityMapping;
	auto velocityWs = scene->mapVelocityToWorldSpace(fan.velocity);
	float magnitude = glm::length(velocityWs);

	float minSize = 0.25f;
	float maxSize = 10.0f;
	float sizeX = glm::clamp(map(fan.density, 0.0f, 500.0f, minSize, maxSize), minSize, maxSize);
	float sizeZ = glm::max(magnitude, minSize);

	scene->fanDebugIcon->model =
		glm::translate(glm::mat4(1), { fanPosition.x, 1.0f, -fanPosition.y}) *
		glm::rotateY(angle) *
		glm::scale(sizeX, 1.0f, sizeZ);
	scene->fanDebugIcon->isVisible = true;
}

void Scene::updateDynamic() {
	light->model = glm::translate(glm::mat4(1), config.lightPosition);
}

void Scene::render() {
	GLCall(glDepthFunc(GL_LEQUAL)); // Why do we have this?

	for (auto object : sceneObjects) {
		if (object->isVisible)
			object->draw(*this);
	}

	glEnable(GL_CULL_FACE);
	for (int i = 0; i < config.numPatches; i++) {
		patches[i]->draw(*this);
	}
	glDisable(GL_CULL_FACE);
	
	for (int i = 0; i < config.numPatches; i++) {
		blades[i]->draw(*this);
	}

	if(config.fluidGridConfig.shouldDrawFans)
	{
		glm::vec4 oldColor = config.lightColor;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (size_t fanIndex = 0; fanIndex < config.fluidGridConfig.fans.size(); fanIndex++)
		{
			config.lightColor = config.fluidGridConfig.selectedFanIndex == fanIndex ?
				glm::vec4(1.0, 0, 0, 0.2f) : glm::vec4(1, 1, 1, 0.2f);

			Fan& fan = config.fluidGridConfig.fans[fanIndex];
			setupFanDebugIconForFan(this, fan);
			fanDebugIcon->draw(*this);
		}

		glDisable(GL_BLEND);

		config.lightColor = oldColor;
	}

}