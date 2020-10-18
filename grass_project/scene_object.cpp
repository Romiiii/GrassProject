#include "scene_object.h"

void SceneObject::drawSceneObject() const {
	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0));
}

void SceneObject::drawSceneObjectArrays() const {
	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	GLCall(glBindVertexArray(0));
}

void SceneObject::drawSceneObjectInstanced(int numSceneObjects, unsigned int instanceVBO, int offset) {
	GLCall(glBindVertexArray(VAO));
	//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	// Draw numSceneObjects triangles of 3 vertices each
	GLCall(glDrawArraysInstanced(GL_TRIANGLES, offset, 3, numSceneObjects));
	GLCall(glBindVertexArray(0));
}

/* Creates the Vertex Array Object and saves
	* positions, colors, indices and normals.
	*/
unsigned int SceneObject::createVertexArray(const std::vector<float>& positions, const std::vector<float>& colors, const std::vector<unsigned int>& indices, const std::vector<float>& normals, Shader& shaderProgram) {
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

	return VAO;
}

unsigned int SceneObject::createVertexArrayInstanced(const std::vector<float>& positions, const std::vector<float>& colors, const std::vector<unsigned int>& indices, const std::vector<float>& normals, Shader& shaderProgram, unsigned int instanceVBO, const std::vector<float>* uvs) {
	shaderProgram.use();
	GLCall(glGenVertexArrays(1, &VAO));
	// Bind vertex array object
	GLCall(glBindVertexArray(VAO));

	int instanceMatrixAttributeLocation = glGetAttribLocation(shaderProgram.getShaderID(), "instanceMatrix");
	GLCall(glEnableVertexAttribArray(instanceMatrixAttributeLocation));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, instanceVBO)); // this attribute comes from a different vertex buffer
			// set attribute pointers for matrix (4 times vec4)
	GLCall(glEnableVertexAttribArray(instanceMatrixAttributeLocation));
	GLCall(glVertexAttribPointer(instanceMatrixAttributeLocation, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0));
	GLCall(glEnableVertexAttribArray(instanceMatrixAttributeLocation+1));
	GLCall(glVertexAttribPointer(instanceMatrixAttributeLocation + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4))));
	GLCall(glEnableVertexAttribArray(instanceMatrixAttributeLocation+2));
	GLCall(glVertexAttribPointer(instanceMatrixAttributeLocation + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4))));
	GLCall(glEnableVertexAttribArray(instanceMatrixAttributeLocation+3));
	GLCall(glVertexAttribPointer(instanceMatrixAttributeLocation + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4))));

	GLCall(glVertexAttribDivisor(instanceMatrixAttributeLocation, 1));
	GLCall(glVertexAttribDivisor(instanceMatrixAttributeLocation+1, 1));
	GLCall(glVertexAttribDivisor(instanceMatrixAttributeLocation+2, 1));
	GLCall(glVertexAttribDivisor(instanceMatrixAttributeLocation+3, 1));

	//GLCall(glBindVertexArray(0));

	/*GLCall(glVertexAttribPointer(
		instanceMatrixAttributeLocation,
		1,
		GL_FLOAT_MAT4, GL_FALSE, sizeof(glm::mat4),
		(void*)0));*/
	//GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	//GLCall(glVertexAttribDivisor(instanceMatrixAttributeLocation, 1)); // tell OpenGL this is an instanced vertex attribute.


	//unsigned int instanceVBO;
	//glGenBuffers(1, &instanceVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glEnableVertexAttribArray(2);
	//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glVertexAttribDivisor(2, 1);

	// setup instanced array with positions
	//createArrayBuffer(positions); // creates and bind  the VBO
	//int posAttributeLocation = glGetAttribLocation(shaderProgram.getShaderID(), "instanceMatrix");
	//glEnableVertexAttribArray(posAttributeLocation);
	//glBindVertexArray(VAO);
	//// sizeof(glm::vec2) * 100, &translations[0]
	//glVertexAttribPointer(posAttributeLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glVertexAttribDivisor(2, 1);

	// Set attributes
	setVertexShaderAttribute("pos", positions, 3, shaderProgram);
	setVertexShaderAttribute("color", colors, 4, shaderProgram);
	setVertexShaderAttribute("normal", normals, 3, shaderProgram);
	if (uvs) {
		setVertexShaderAttribute("uvs", *uvs, 2, shaderProgram);
	}

	// Create and bind the EBO
	createElementArrayBuffer(indices);

	vertexCount = indices.size();

	return VAO;
}


/* Creates the Vertex Array Object and saves
	* positions, colors, indices, uvs (for the texture) and normals.
	*/
unsigned int SceneObject::createVertexArrayTexture(const std::vector<float>& positions, const std::vector<unsigned int>& indices, const std::vector<float>& uvs, const std::vector<float>& normals, Shader& shaderProgram) {
	shaderProgram.use();
	GLCall(glGenVertexArrays(1, &VAO));
	// bind vertex array object
	GLCall(glBindVertexArray(VAO));


	// Set attributes
	setVertexShaderAttribute("pos", positions, 3, shaderProgram);
	setVertexShaderAttribute("uv", uvs, 2, shaderProgram);
	setVertexShaderAttribute("normal", normals, 3, shaderProgram);

	// creates and bind the EBO
	createElementArrayBuffer(indices);

	vertexCount = positions.size();

	return VAO;
}

/* Creates the Vertex Array Object and saves
	* positions.
	*/
unsigned int SceneObject::createVertexArrayFromPositions(const std::vector<float>& positions) {
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glBindVertexArray(VAO));
	createArrayBuffer(positions);
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	return VAO;
}


unsigned int SceneObject::createArrayBuffer(const std::vector<float>& array) {
	unsigned int VBO;
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(GLfloat), &array[0], GL_STATIC_DRAW));

	return VBO;
}

unsigned int SceneObject::createElementArrayBuffer(const std::vector<unsigned int>& array) {
	unsigned int EBO;
	GLCall(glGenBuffers(1, &EBO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, array.size() * sizeof(unsigned int), &array[0], GL_STATIC_DRAW));

	return EBO;
}

/* Sets the specified attribute of the vertex shader
	*/
void SceneObject::setVertexShaderAttribute(char *attributeName,
	const std::vector<float>& data, int dataSize, Shader& shaderProgram) {
	createArrayBuffer(data);  // Creates and binds the VBO
	int attributeLocation = glGetAttribLocation(shaderProgram.getShaderID(), attributeName);
	//assert(attributeLocation != -1);
	//std::cout << attributeLocation << std::endl;
	if (attributeLocation == -1) {

		std::cout << "\x1B[01;93mWARNING: Attribute: " << attributeName << " - not found" << std::endl;
		std::cout << "WARNING: Using shader: " << shaderProgram.getFragmentPath() << "\x1B[0m" << std::endl;
	}
	GLCall(glEnableVertexAttribArray(attributeLocation));
	GLCall(glVertexAttribPointer(attributeLocation, dataSize, GL_FLOAT, GL_FALSE, 0, 0));
}
