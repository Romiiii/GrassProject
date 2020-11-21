#include "scene_object_instanced.h"

SceneObjectInstanced::SceneObjectInstanced(
	const const std::vector<float>& positions, 
	const std::vector<float>& colors, 
	const std::vector<unsigned int>& indices, 
	const std::vector<float>& normals, 
	unsigned int instanceMatrixBuffer, 
	Shader& shaderProgram, const std::vector<float>* uvs)
	: SceneObject(shaderProgram) {
	createVertexArray(positions, colors, indices, normals, instanceMatrixBuffer, uvs);

}

void SceneObjectInstanced::createVertexArray(const std::vector<float>& positions, const std::vector<float>& colors, const std::vector<unsigned int>& indices, const std::vector<float>& normals, unsigned int instanceMatrixBuffer, const std::vector<float>* uvs) {
	shader.use();
	GLCall(glGenVertexArrays(1, &VAO));
	// Bind vertex array object
	GLCall(glBindVertexArray(VAO));

	int instanceMatrixAttributeLocation = glGetAttribLocation(shader.getShaderID(), "instanceMatrix");
	GLCall(glEnableVertexAttribArray(instanceMatrixAttributeLocation));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer)); // this attribute comes from a different vertex buffer
			// set attribute pointers for matrix (4 times vec4)
	GLCall(glEnableVertexAttribArray(instanceMatrixAttributeLocation));
	GLCall(glVertexAttribPointer(instanceMatrixAttributeLocation, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0));
	GLCall(glEnableVertexAttribArray(instanceMatrixAttributeLocation + 1));
	GLCall(glVertexAttribPointer(instanceMatrixAttributeLocation + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))));
	GLCall(glEnableVertexAttribArray(instanceMatrixAttributeLocation + 2));
	GLCall(glVertexAttribPointer(instanceMatrixAttributeLocation + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))));
	GLCall(glEnableVertexAttribArray(instanceMatrixAttributeLocation + 3));
	GLCall(glVertexAttribPointer(instanceMatrixAttributeLocation + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))));

	GLCall(glVertexAttribDivisor(instanceMatrixAttributeLocation, 1));
	GLCall(glVertexAttribDivisor(instanceMatrixAttributeLocation + 1, 1));
	GLCall(glVertexAttribDivisor(instanceMatrixAttributeLocation + 2, 1));
	GLCall(glVertexAttribDivisor(instanceMatrixAttributeLocation + 3, 1));

	// Set attributes
	setVertexShaderAttribute("pos", positions, 3, shader);
	setVertexShaderAttribute("color", colors, 4, shader);
	setVertexShaderAttribute("normal", normals, 3, shader);
	if (uvs != NULL) {
		setVertexShaderAttribute("uvs", *uvs, 2, shader);
	}


	// Create and bind the EBO
	createElementArrayBuffer(indices);

	vertexCount = indices.size();
}

void SceneObjectInstanced::draw(Scene& scene) {
	//std::cout << shader->getShaderID() << std::endl;
	if (shader.use()) {
		setUniforms(scene);
	}
	else {
		std::cout << "ERROR: Shader is not initialized." << std::endl;
	}
	GLCall(glBindVertexArray(VAO));
	//glBindBuffer(GL_ARRAY_BUFFER, instanceMatrixBuffer);
	// Draw patchDensity triangles of 3 vertices each
	GLCall(glDrawArraysInstanced(GL_TRIANGLES, 0, 3, scene.config.patchDensity));
	GLCall(glBindVertexArray(0));
}



