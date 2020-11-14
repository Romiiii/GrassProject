/*
 * The SceneObjects class handles the vertex information of the
 * objects and draws them on the screen.
 */
#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cassert>
#include "shader.h"
#include "debug.h"
#include "texture.h"

class Scene;

class SceneObject {
public:
	virtual void draw(Scene& scene) = 0;
	void setUniforms(Scene& scene);

protected:
	unsigned int VAO;
	unsigned int vertexCount;
	unsigned int createArrayBuffer(const std::vector<float>& array);
	Shader* shader;

	/* Sets the specified attribute of the vertex shader
		*/
	void setVertexShaderAttribute(
		char *attributeName,
		const std::vector<float>& data,
		int dataSize,
		Shader& shaderProgram);

	unsigned int createElementArrayBuffer(const std::vector<unsigned int>& array);
};

#endif