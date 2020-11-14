#ifndef SCENE_H
#define SCENE_H 

#include <vector>
#include "scene_object.h"

class Scene {
public:
	std::vector<SceneObject*> sceneObjects;
	void render();
	int numSceneObjects = 10;

};


#endif