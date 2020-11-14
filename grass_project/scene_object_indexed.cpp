#include "scene_object_indexed.h"


/* Creates the Vertex Array Object and saves
	* positions, colors, indices and normals.
	*/
unsigned int SceneObjectIndexed::createVertexArray(const std::vector<float>& positions, const std::vector<float>& colors, const std::vector<unsigned int>& indices, const std::vector<float>& normals, Shader& shaderProgram) {
	shaderProgram.use();
	glGenVertexArrays(1, &VAO);
	// Bind vertex array object
	glBindVertexArray(VAO);

	// Set attributes
	setVertexShaderAttribute("pos", positions, 3, shaderProgram);
	setVertexShaderAttribute("color", colors, 4, shaderProgram);
	setVertexShaderAttribute("normal", normals, 3, shaderProgram);

	// Create and bind the EBO
	createElementArrayBuffer(indices);

	vertexCount = indices.size();

	shader = &shaderProgram;

	return VAO;
}


void SceneObjectIndexed::draw(Scene& scene) {
	if (shader->use()) {
		//setUniforms(scene);
	}
	else {
		std::cout << "ERROR: Shader is not initialized." << std::endl;
	}
	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0));
}
