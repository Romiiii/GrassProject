/*
 * The SceneObjectsInstanced class handles the vertex information of the
 * objects using instancing and draws them on the screen.
 */
#ifndef SCENE_OBJECT_INSTANCED_H
#define SCENE_OBJECT_INSTANCED_H

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

class SceneObjectInstanced : public SceneObject {
public:
	unsigned int createVertexArray(
		const std::vector<float>& positions, const std::vector<float>& colors, const std::vector<unsigned int>& indices, const std::vector<float>& normals, Shader& shaderProgram, unsigned int instanceVBO, const std::vector<float>* uvs = nullptr);

	void draw(Scene& scene) override;



};


#endif