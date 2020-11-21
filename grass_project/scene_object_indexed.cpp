#include "scene_object_indexed.h"


SceneObjectIndexed::SceneObjectIndexed(const std::vector<float>& positions, const std::vector<float>& colors, const std::vector<unsigned int>& indices, const std::vector<float>& normals,
	Shader& shaderProgram) 
	: SceneObject(shaderProgram) {
	createVertexArray(positions, colors, indices, normals);
}

/* Creates the Vertex Array Object and saves
	* positions, colors, indices and normals.
	*/
void SceneObjectIndexed::createVertexArray(const std::vector<float>& positions, const std::vector<float>& colors, const std::vector<unsigned int>& indices, const std::vector<float>& normals) {
	shader.use();
	glGenVertexArrays(1, &VAO);
	// Bind vertex array object
	glBindVertexArray(VAO);

	// Set attributes
	setVertexShaderAttribute("pos", positions, 3, shader);
	setVertexShaderAttribute("color", colors, 4, shader);
	setVertexShaderAttribute("normal", normals, 3, shader);

	// Create and bind the EBO
	createElementArrayBuffer(indices);

	vertexCount = indices.size();
}


void SceneObjectIndexed::draw(Scene& scene) {
	if (shader.use()) {
		setUniforms(scene);
	}
	else {
		std::cout << "ERROR: Shader is not initialized." << std::endl;
	}
	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0));
}
