/*
 * The SceneObjectsArrays class handles the vertex information of the
 * objects using arrays and draws them on the screen.
 */
#ifndef SCENE_OBJECT_INDEXED_H
#define SCENE_OBJECT_INDEXED_H

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


 /*
  * Handles the vertex information of the SceneObjects using an element
  * array buffer. 
  */
class SceneObjectIndexed : public SceneObject {
public:
	SceneObjectIndexed(
		const std::vector<float>& positions, 
		const std::vector<float>& colors, 
		const std::vector<unsigned int>& indices, 
		const std::vector<float>& normals,
		ShaderProgram& shaderProgram, 
		const std::vector<float>* uvs = NULL);

	/* Creates the Vertex Array Object and saves
	 * positions, colors, indices, uvs (for the texture) and normals.
	 */
	void createVertexArray(
		const std::vector<float>& positions, 
		const std::vector<float>& colors, 
		const std::vector<unsigned int>& indices, 
		const std::vector<float>& normals, 
		const std::vector<float>* uvs = NULL);

	void draw(Scene& scene) override;


};


#endif