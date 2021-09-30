#include "scene.h"
#include "glmutils.h"
#include <glm\glm.hpp>

void Scene::addSceneObject(SceneObject* sceneObject) {
	sceneObjects.push_back(sceneObject);
}

void Scene::updateDynamic() {
	light->model = glm::translate(glm::mat4(1), config.lightPosition);
}

void Scene::render() {
	GLCall(glDepthFunc(GL_LEQUAL)); // Why do we have this?

	for (auto object : sceneObjects) {
		object->draw(*this);
	}

	for (int i = 0; i < config.numPatches; i++) {
		patches[i]->draw(*this);
		blades[i]->draw(*this);
	}
}