#include "scene_object_indexed.h"


SceneObjectIndexed::SceneObjectIndexed(
	const std::vector<float>& positions, 
	const std::vector<float>& colors, 
	const std::vector<unsigned int>& indices, 
	const std::vector<float>& normals,
	ShaderProgram& shaderProgram, 
	const std::vector<float>* uvs)
	: SceneObject(shaderProgram) {
	createVertexArray(positions, colors, indices, normals, uvs);
}

/* Creates the Vertex Array Object and saves
	* positions, colors, indices and normals.
	*/
void SceneObjectIndexed::createVertexArray(
	const std::vector<float>& positions, 
	const std::vector<float>& colors, 
	const std::vector<unsigned int>& indices, 
	const std::vector<float>& normals, 
	const std::vector<float>* uvs) {
	shaderProgram.use();
	glGenVertexArrays(1, &VAO);
	// Bind vertex array object
	glBindVertexArray(VAO);

	// Set attributes
	setVertexShaderAttribute("pos", positions, 3, shaderProgram);
	setVertexShaderAttribute("color", colors, 4, shaderProgram);
	setVertexShaderAttribute("normal", normals, 3, shaderProgram);
	if (uvs != NULL) {
		setVertexShaderAttribute("uvs", *uvs, 2, shaderProgram);
	}
	

	// Create and bind the EBO
	createElementArrayBuffer(indices);

	vertexCount = (unsigned int)indices.size();
}


void SceneObjectIndexed::draw(Scene& scene) {
	shaderProgram.use();
	setUniforms(scene);

	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0));
}
