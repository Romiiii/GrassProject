#ifndef PATCH_H
#define PATCH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>     /* srand, rand */
#include <glm/glm.hpp>

#include "scene_object.h"
#include "primitives.h"
#include "util.h"
#include "scene_object_indexed.h"

// Array of grass coordinates 



class Patch {
public:
	Patch() {

	}

	~Patch() {
		delete bladeMatrices;
	}

	void init(int numBlades, Shader* shaderProgram) {
		this->numBlades = numBlades;
		this->shaderProgram = shaderProgram;
		bladeMatrices = new glm::mat4[numBlades];
		initBladeMatrices();
	}

	/* Initializes the patch by calculating the coordinates of the grass/billboards.
	 * Coordinates are sampled uniformly. A random rotation of the grass is
	 * generated as well.
	 */
	void initBladeMatrices() {
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

	//SceneObject* createPatchInstance() {
	//	SceneObjectIndexed *patchInstance = new SceneObjectIndexed();
	//	patchInstance->createVertexArray(grassPatchPositions, grassPatchColors,
	//		grassPatchIndices, grassPatchNormals);
	//	return patchInstance;
	//}

	glm::mat4* getBladeMatrices() {
		return bladeMatrices;
	}

	std::vector<glm::vec3> grassCoordinates;
	std::vector<glm::vec2> grassRotations;
	std::vector<glm::mat4> grassMatrixes;


private:
	glm::mat4* bladeMatrices = nullptr;
	int numBlades = 0;
	Shader* shaderProgram;
};

// Doesnt need to exist anymore
//class PatchInstance {
//public:
//	PatchInstance() {
//
//	}
//
//	void init(SceneObject *patchInstance, glm::mat4 patchMatrix) {
//		this->patchInstance = patchInstance;
//		this->patchMatrix = patchMatrix;
//	}
//
//	SceneObject* getPatchInstance() {
//		return this->patchInstance;
//	}
//
//	glm::mat4& getPatchMatrix() {
//		return patchMatrix;
//	}
//
//
//private:
//	SceneObject* patchInstance;
//	glm::mat4 patchMatrix;
//
//};


#endif