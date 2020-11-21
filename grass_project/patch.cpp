#include "patch.h"
#include "util.h"
#include "scene_object.h"
#include "primitives.h"
#include "scene_object_indexed.h"
#include <algorithm>
#include <stdlib.h>     /* srand, rand */

Patch::Patch() {
}

Patch::~Patch() {
	delete bladeMatrices;
}

void Patch::init(int numBlades, Shader* shaderProgram) {
	this->numBlades = numBlades;
	this->shaderProgram = shaderProgram;
	bladeMatrices = new glm::mat4[numBlades];
	initBladeMatrices();
}

/* Initializes the patch by calculating the coordinates of the blades.
* Coordinates are sampled uniformly. A random rotation of the grass is
* generated as well.
*/
void Patch::initBladeMatrices() {
	float upperBoundX = std::max({ grassPatchPositions[0], grassPatchPositions[3], grassPatchPositions[6], grassPatchPositions[9] });
	float lowerBoundX = std::min({ grassPatchPositions[0], grassPatchPositions[3], grassPatchPositions[6], grassPatchPositions[9] });

	int upperBoundZ = std::max({ grassPatchPositions[2], grassPatchPositions[5], grassPatchPositions[8], grassPatchPositions[11] });
	int lowerBoundZ = std::min({ grassPatchPositions[2], grassPatchPositions[5], grassPatchPositions[8], grassPatchPositions[11] });

	constexpr float swayX = glm::radians(20.0f);
	constexpr float swayY = glm::radians(180.0f);

	// Distribute the grass blades uniformly within the patch
	for (int x = 0; x < numBlades; x += 1) {
		float randomPosX = generateRandomNumber(lowerBoundX, upperBoundX);
		float randomPosZ = generateRandomNumber(lowerBoundZ, upperBoundZ);

		glm::vec3 grassCoordinate = glm::vec3(randomPosX, 0, randomPosZ);

		grassCoordinates.push_back(grassCoordinate);

		float randomRotX = generateRandomNumber(-swayX, swayX);
		float randomRotY = generateRandomNumber(-swayY, swayY);

		bladeMatrices[x] = glm::translate(grassCoordinate) * glm::rotateX(randomRotX) * glm::rotateY(randomRotY);
	}
}

glm::mat4* Patch::getBladeMatrices() {
	return bladeMatrices;
}
