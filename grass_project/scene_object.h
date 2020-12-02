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
#include "debug.h"
#include "texture.h"
#include "glmutils.h"
#include "shader_program.h"

class Scene;

class SceneObject {
public:
	SceneObject(ShaderProgram& shaderProgram);
	virtual void draw(Scene& scene) = 0;
	void setUniforms(Scene& scene);
	glm::mat4 model = glm::mat4(1) * glm::translate(glm::mat4(1), { 2,3,4 });

protected:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int vertexCount = 0;
	void createArrayBuffer(const std::vector<float>& array);
	ShaderProgram& shaderProgram;

	/* Sets the specified attribute of the vertex shaderProgram
		*/
	void setVertexShaderAttribute(
		char *attributeName,
		const std::vector<float>& data,
		int dataSize,
		ShaderProgram& shaderProgram);

	unsigned int createElementArrayBuffer(const std::vector<unsigned int>& array);
};

#endif