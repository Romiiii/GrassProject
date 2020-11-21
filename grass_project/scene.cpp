#include "scene.h"
#include "glmutils.h"
#include <glm\glm.hpp>

void Scene::addSceneObject(SceneObject* sceneObject) {
	sceneObjects.push_back(sceneObject);
}

void Scene::updateDynamic() {
	light->model = glm::translate(glm::mat4(1), config.lightPosition);
	//light->model = glm::translate(scene.config.lightPosition.x, scene.config.lightPosition.y, scene.config.lightPosition.z);
}

void Scene::render() {
	GLCall(glDepthFunc(GL_LEQUAL));
	for (auto object : sceneObjects) {
		object->draw(*this);
	}
	for (int i = 0; i < config.numPatches; i++) {
		patches[i]->draw(*this);
		blades[i]->draw(*this);
	}
}
//
//void Scene::setModelMatrices() {
//	for (auto object : sceneObjects) {
//		if (patch) {
//			
//		}
//		object->draw(*this, );
//	}
//}