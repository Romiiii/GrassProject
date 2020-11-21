#include "scene_object_arrays.h"

SceneObjectArrays::SceneObjectArrays(const std::vector<float>& positions, Shader& shaderProgram) 
	: SceneObject(shaderProgram) {
	createVertexArray(positions);
}

/* Creates the Vertex Array Object and saves
	* positions.
	*/
void SceneObjectArrays::createVertexArray(const std::vector<float>& positions) {
	shader.use();
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glBindVertexArray(VAO));
	createArrayBuffer(positions);
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	vertexCount = positions.size();
}

void SceneObjectArrays::draw(Scene& scene) {
	if (shader.use()) {
		setUniforms(scene);
	}
	else {
		std::cout << "ERROR: Shader is not initialized." << std::endl;
	}
	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, vertexCount));
	GLCall(glBindVertexArray(0));
}

