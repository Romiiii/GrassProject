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

/**
* Represents the elements of the scene. Contains functionality to draw
* the object.
*/
class SceneObject {
public:
	SceneObject(ShaderProgram& shaderProgram);
	virtual void draw(Scene& scene) = 0;
	/**
	* \brief Sets all active uniforms, based on the user's settings
	*/
	void setUniforms(Scene& scene);
	glm::mat4 model;
	bool isVisible = true;

protected:
	void createArrayBuffer(const std::vector<float>& array);
	unsigned int createElementArrayBuffer(const std::vector<unsigned int>& array);
	/**
	* \brief Sets the specified attribute of the vertex shaderProgram
	*/
	void setVertexShaderAttribute(
		const char* attributeName,
		const std::vector<float>& data,
		int dataSize,
		ShaderProgram& shaderProgram);

	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int vertexCount = 0;
	ShaderProgram& shaderProgram;


};

#endif