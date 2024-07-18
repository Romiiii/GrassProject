#ifndef GRASS_SIMULATION_H
#define GRASS_SIMULATION_H

#include "scene.h"
#include "input.h"

namespace GrassSimulation
{
	bool setup(Scene* scene);
	bool update(float deltaTime);
	void reloadShaders();
	void cleanup();
	void drawGui();
	void handleInputs();
}

#endif // !GRASS_SIMULATION_H
