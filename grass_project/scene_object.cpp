#include "scene_object.h"

void SceneObject::drawSceneObject() {
	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0));
}

void SceneObject::drawSceneObjectArrays() {
	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
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

	// Set vertex shader attribute "pos"
	createArrayBuffer(positions);  // Creates and binds the VBO
	int posAttributeLocation = glGetAttribLocation(shaderProgram.getShaderID(), "pos");
	glEnableVertexAttribArray(posAttributeLocation);
	glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Set vertex shader attribute "color"
	createArrayBuffer(colors);  // Creates and binds the VBO
	int colorAttributeLocation = glGetAttribLocation(shaderProgram.getShaderID(), "color");
	glEnableVertexAttribArray(colorAttributeLocation);
	glVertexAttribPointer(colorAttributeLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// Set vertex shader attribute "normal"
	createArrayBuffer(normals);  // Creates and binds the VBO
	int normalAttributeLocation = glGetAttribLocation(shaderProgram.getShaderID(), "normal");
	glEnableVertexAttribArray(normalAttributeLocation);
	glVertexAttribPointer(normalAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Creates and binds the EBO
	createElementArrayBuffer(indices);

	vertexCount = indices.size();

	return VAO;
}

/* Creates the Vertex Array Object and saves
	* positions, colors, indices, uvs (for the texture) and normals.
	*/
unsigned int SceneObject::createVertexArrayTexture(const std::vector<float>& positions, const std::vector<float>& colors, const std::vector<unsigned int>& indices, const std::vector<float>& uvs, const std::vector<float>& normals, Shader& shaderProgram) {
	shaderProgram.use();
	glGenVertexArrays(1, &VAO);
	// bind vertex array object
	glBindVertexArray(VAO);

	// set vertex shader attribute "pos"
	createArrayBuffer(positions); // creates and bind  the VBO
	int posAttributeLocation = glGetAttribLocation(shaderProgram.getShaderID(), "pos");

	glEnableVertexAttribArray(posAttributeLocation);
	glVertexAttribPointer(posAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// set vertex shader attribute "color"
	createArrayBuffer(colors); // creates and bind the VBO
	int colorAttributeLocation = glGetAttribLocation(shaderProgram.getShaderID(), "color");
	glEnableVertexAttribArray(colorAttributeLocation);
	glVertexAttribPointer(colorAttributeLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// set vertex shader attribute "uv"
	createArrayBuffer(uvs); // creates and bind the VBO
	int uvAttributeLocation = glGetAttribLocation(shaderProgram.getShaderID(), "uv");
	GLCall(glEnableVertexAttribArray(uvAttributeLocation));
	GLCall(glVertexAttribPointer(uvAttributeLocation, 2, GL_FLOAT, GL_FALSE, 0, 0));

	// set vertex shader attribute "normal"
	createArrayBuffer(normals); // creates and bind the VBO
	int normalAttributeLocation = glGetAttribLocation(shaderProgram.getShaderID(), "normal");
	glEnableVertexAttribArray(normalAttributeLocation);
	glVertexAttribPointer(normalAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

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
