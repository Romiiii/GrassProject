#include "scene.h"

void Scene::render() {
	for (auto object : sceneObjects) {
		object->draw(*this);
	}
}