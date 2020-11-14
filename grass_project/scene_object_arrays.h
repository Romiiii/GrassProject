/*
 * The SceneObjectsArrays class handles the vertex information of the
 * objects using arrays and draws them on the screen.
 */
#ifndef SCENE_OBJECT_ARRAYS_H
#define SCENE_OBJECT_ARRAYS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cassert>
#include "shader.h"
#include "debug.h"
#include "texture.h"
#include "scene_object.h"
#include "scene.h"

class SceneObjectArrays : public SceneObject {
public:
	/* Creates the Vertex Array Object and saves
	* positions.
	*/
	unsigned int createVertexArray(const std::vector<float>& positions, Shader& shaderProgram);
	unsigned int SceneObjectArrays::createVertexArray(const std::vector<float>& positions, const std::vector<float>& colors, const std::vector<unsigned int>& indices, const std::vector<float>& normals, Shader& shaderProgram);

	void draw(Scene& scene) override;


};


#endif