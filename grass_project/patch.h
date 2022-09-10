#ifndef PATCH_H
#define PATCH_H

#include <glm/glm.hpp>
#include <vector>
#include "shader_program.h"

/** 
* Represent the patch on which blades are drawn. Also contains the 
* transformation (position and rotation) of each blade in the patch.
*/
class Patch {
public:
	Patch();

	~Patch();

	void init(int numBlades, ShaderProgram* shaderProgram);

	/** 
	 * Initializes the patch by calculating the coordinates of the blades.
	 * Coordinates are sampled uniformly. 
	 * \param useRandomRotation Applies a random rotation to each blade of 
	 * grass when set to true.
	 */
	void initHarryEdwardStylesBladeMatrices(float patchSize, bool useRandomRotations = true);

	/** 
	* Initializes the patch by calculating the coordinates of the blades.
	* The blades will all be in the middle of the x-axis of the corresponding 
	* patch and distributed evenly over the y-axis forming a straight line. 
	* The blades will not be rotated.
	*/
	void initOneDirectionBladeMatrices(float patchSize);

	/** 
	* Stores the transformation of all the blades in a patch.
	*/
	glm::mat4* getBladeMatrices();

private:
	glm::mat4* bladeMatrices = nullptr;
	int numBlades = 0;
	ShaderProgram* shaderProgram = nullptr;
};


#endif