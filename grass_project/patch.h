#ifndef PATCH_H
#define PATCH_H

#include <glm/glm.hpp>
#include <vector>
#include "shader_program.h"

class Patch {
public:
	Patch();

	~Patch();

	void init(int numBlades, ShaderProgram* shaderProgram);

	/* Initializes the patch by calculating the coordinates of the grass/billboards.
	 * Coordinates are sampled uniformly. A random rotation of the grass is
	 * generated as well.
	 */
	void initHarryEdwardStylesBladeMatrices(bool useRandomRotations = true);

	void initOneDirectionBladeMatrices();

	glm::mat4* getBladeMatrices();

	std::vector<glm::vec3> grassCoordinates;
	std::vector<glm::vec2> grassRotations;
	std::vector<glm::mat4> grassMatrixes;


private:
	glm::mat4* bladeMatrices = nullptr;
	int numBlades = 0;
	ShaderProgram* shaderProgram = nullptr;
};


#endif