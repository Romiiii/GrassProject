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
	SceneObjectArrays(const std::vector<float>& positions, ShaderProgram& shaderProgram);

	/* Creates the Vertex Array Object and saves
	* positions.
	*/
	void createVertexArray(const std::vector<float>& positions);


	void draw(Scene& scene) override;


};


#endif