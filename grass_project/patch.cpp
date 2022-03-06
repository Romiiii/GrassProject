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

void Patch::init(int numBlades, ShaderProgram* shaderProgram) {
	this->numBlades = numBlades;
	this->shaderProgram = shaderProgram;
	this->bladeMatrices = new glm::mat4[numBlades];
}

void Patch::initHarryEdwardStylesBladeMatrices(bool useRandomRotations) {
	float upperBoundX = std::max({ grassPatchPositions[0], grassPatchPositions[3], grassPatchPositions[6], grassPatchPositions[9] });
	float lowerBoundX = std::min({ grassPatchPositions[0], grassPatchPositions[3], grassPatchPositions[6], grassPatchPositions[9] });

	float upperBoundZ = std::max({ grassPatchPositions[2], grassPatchPositions[5], grassPatchPositions[8], grassPatchPositions[11] });
	float lowerBoundZ = std::min({ grassPatchPositions[2], grassPatchPositions[5], grassPatchPositions[8], grassPatchPositions[11] });

	constexpr float swayX = glm::radians(20.0f);
	constexpr float swayY = glm::radians(180.0f);

	float middleX = (upperBoundX + lowerBoundX) / 2;

	// Distribute the blades uniformly within the patch
	for (int x = 0; x < numBlades; x += 1) {
		float randomPosX = generateRandomNumber(lowerBoundX, upperBoundX);
		float randomPosZ = generateRandomNumber(lowerBoundZ, upperBoundZ);

		glm::vec3 grassCoordinate = glm::vec3(randomPosX, 0, randomPosZ) * PATCH_SIZE;

		bladeMatrices[x] = glm::translate(grassCoordinate);

		// Apply a random rotation within the sway bounds to each blade
		if (useRandomRotations) {
			float randomRotX = generateRandomNumber(-swayX, swayX);
			float randomRotY = generateRandomNumber(-swayY, swayY);
			bladeMatrices[x] *= glm::rotateX(randomRotX) * glm::rotateY(randomRotY);
		}
		
	}
}

void Patch::initOneDirectionBladeMatrices() {
	float upperBoundX = std::max({ grassPatchPositions[0], grassPatchPositions[3], grassPatchPositions[6], grassPatchPositions[9] });
	float lowerBoundX = std::min({ grassPatchPositions[0], grassPatchPositions[3], grassPatchPositions[6], grassPatchPositions[9] });

	float upperBoundZ = std::max({ grassPatchPositions[2], grassPatchPositions[5], grassPatchPositions[8], grassPatchPositions[11] });
	float lowerBoundZ = std::min({ grassPatchPositions[2], grassPatchPositions[5], grassPatchPositions[8], grassPatchPositions[11] });

	constexpr float swayX = glm::radians(20.0f);
	constexpr float swayY = glm::radians(180.0f);

	float middleX = (upperBoundX + lowerBoundX) / 2;

	// Distribute the blades in the middle of the patch
	for (int x = 0; x < numBlades; x += 1) {
		float z = lowerBoundZ + ((upperBoundZ - lowerBoundZ) / (numBlades)) * x;

		glm::vec3 grassCoordinate = glm::vec3(middleX, 0, z) * PATCH_SIZE;

		bladeMatrices[x] = glm::translate(grassCoordinate);
	}
}

glm::mat4* Patch::getBladeMatrices() {
	return bladeMatrices;
}
