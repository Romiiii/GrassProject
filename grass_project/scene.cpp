#include "scene.h"
#include "glmutils.h"
#include <glm\glm.hpp>
#include "grass_math.h"

void Scene::addSceneObject(SceneObject* sceneObject) {
	sceneObjects.push_back(sceneObject);
}

void Scene::updateDynamic() {
	light->model = glm::translate(glm::mat4(1), config.lightPosition);

	if (config.fluidGridConfig.shouldDrawFans) {
		float fanX = map(config.fluidGridConfig.fan.x, 0.0f, 1.0f, config.worldMin, config.worldMax);
		float fanY = map(config.fluidGridConfig.fan.y, 0.0f, 1.0f, config.worldMin, config.worldMax) - config.patchSize;

		float velX = config.fluidGridConfig.fan.velocityX;
		float velY = config.fluidGridConfig.fan.velocityY;

		float angle = glm::atan(velX, velY);

		float minSize = 0.25f;
		float maxSize = 10.0f;

		float mappingValue = config.fluidGridConfig.wholeWorldToVelocityMapping;
		glm::vec2 direction;
		direction.x = map(velX, .0f, mappingValue, 0.0f, worldRekt.width);
		direction.y = map(velY, .0f, mappingValue, 0.0f, worldRekt.width);
		float magnitude = glm::length(direction);

		float density = config.fluidGridConfig.fan.density;
		float sizeX = glm::clamp(map(density, 0.0f, 500.0f, minSize, maxSize), minSize, maxSize);
		float sizeZ = glm::max(magnitude, minSize);

		fanDebugIcon->model = glm::translate(glm::mat4(1), { fanX, 1.0f, -fanY }) * glm::rotateY(angle) * glm::scale(sizeX, 1.0f, sizeZ);
		fanDebugIcon->isVisible = true;
	}
	else {
		fanDebugIcon->isVisible = false;
	}
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

	{
		glm::vec4 oldColor = config.lightColor;

		glEnable(GL_BLEND);
		config.lightColor = glm::vec4(1, 1, 1, 0.2f);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		fanDebugIcon->draw(*this);
		glDisable(GL_BLEND);

		config.lightColor = oldColor;
	}

}