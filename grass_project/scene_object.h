/*
 * The SceneObjects class handles the vertex information of the
 * objects and draws them on the screen.
 */
#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT__H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "shader.h"
#include "debug.h"
#include "texture.h"

class SceneObject {
public:
	void drawSceneObject();
	void drawSceneObjectArrays();


	/* Creates the Vertex Array Object and saves 
	 * positions, colors, indices and normals. 
	 */
	unsigned int createVertexArray(const std::vector<float>& positions,
		const std::vector<float>& colors, const std::vector<unsigned int>& indices, 
		const std::vector<float>& normals, Shader& shaderProgram);

	/* Creates the Vertex Array Object and saves
	 * positions, colors, indices, uvs (for the texture) and normals.
	 */
	unsigned int createVertexArrayTexture(const std::vector<float>& positions,
		const std::vector<float>& colors, const std::vector<unsigned int>& indices,
		const std::vector<float>& uvs, const std::vector<float>& normals, Shader& shaderProgram);

	/* Creates the Vertex Array Object and saves
	 * positions.
	 */
	unsigned int createVertexArrayFromPositions(const std::vector<float>& positions);

private:
	unsigned int VAO;
	unsigned int vertexCount;
	unsigned int createArrayBuffer(const std::vector<float>& array);
	unsigned int createElementArrayBuffer(const std::vector<unsigned int>& array);
};

#endif