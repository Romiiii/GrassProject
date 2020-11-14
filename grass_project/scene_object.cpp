#include "scene_object.h"

void SceneObject::setUniforms(Scene& scene) {
	GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 16; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length

	glGetProgramiv(shader->getShaderID(), GL_ACTIVE_UNIFORMS, &count);
	printf("Active Uniforms: %d\n", count);

	for (i = 0; i < count; i++)
	{
		glGetActiveUniform(shader->getShaderID(), (GLuint)i, bufSize, &length, &size, &type, name);
		printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
		// Get info and pass it to shader
	}
}


unsigned int SceneObject::createArrayBuffer(const std::vector<float>& array) {
	unsigned int VBO;
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(GLfloat), &array[0], GL_STATIC_DRAW));

	return VBO;
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

unsigned int SceneObject::createElementArrayBuffer(const std::vector<unsigned int>& array) {
	unsigned int EBO;
	GLCall(glGenBuffers(1, &EBO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, array.size() * sizeof(unsigned int), &array[0], GL_STATIC_DRAW));

	return EBO;
}


