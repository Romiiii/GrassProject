#ifndef PATCH_H
#define PATCH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>     /* srand, rand */
#include <glm/glm.hpp>

#include "scene_object.h"
#include "primitives.h"
#include "util.h"

// Array of grass coordinates 
std::vector<glm::vec3> grassCoordinates;
std::vector<glm::vec2> grassRotations;
std::vector<glm::mat4> grassMatrixes;


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
		float upperBoundX = std::max({ grassPatchVertices[0], grassPatchVertices[3], grassPatchVertices[6], grassPatchVertices[9] });
		float lowerBoundX = std::min({ grassPatchVertices[0], grassPatchVertices[3], grassPatchVertices[6], grassPatchVertices[9] });

		int upperBoundZ = std::max({ grassPatchVertices[2], grassPatchVertices[5], grassPatchVertices[8], grassPatchVertices[11] });
		int lowerBoundZ = std::min({ grassPatchVertices[2], grassPatchVertices[5], grassPatchVertices[8], grassPatchVertices[11] });

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

	SceneObject createPatchInstance() {
		SceneObject patchInstance;
		patchInstance.createVertexArray(grassPatchVertices, grassPatchColors,
			grassPatchIndices, grassPatchNormals, *shaderProgram);
		return patchInstance;
	}

	glm::mat4* getBladeMatrices() {
		return bladeMatrices;
	}


private:
	glm::mat4* bladeMatrices = nullptr;
	int numBlades = 0;
	Shader* shaderProgram;
};


class PatchInstance {
public:
	PatchInstance() {

	}

	void init(SceneObject patchInstance, glm::mat4 patchMatrix) {
		this->patchInstance = patchInstance;
		this->patchMatrix = patchMatrix;
	}

	SceneObject& getPatchInstance() {
		return this->patchInstance;
	}

	glm::mat4& getPatchMatrix() {
		return patchMatrix;
	}


private:
	SceneObject patchInstance;
	glm::mat4 patchMatrix;

};


#endif