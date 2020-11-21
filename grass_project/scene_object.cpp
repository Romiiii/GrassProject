#include "scene_object.h"
#include "scene.h"


SceneObject::SceneObject(Shader& shaderProgram)
	: shader(shaderProgram) {
}	

void SceneObject::setUniforms(Scene& scene) {
	GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 16; // maximum name length
	GLchar nameBuf[bufSize]; // variable name in GLSL
	//std::string name;
	//name.reserve(bufSize);
	GLsizei length; // name length

	glGetProgramiv(shader.getShaderID(), GL_ACTIVE_UNIFORMS, &count);

	for (i = 0; i < count; i++)
	{
		glGetActiveUniform(shader.getShaderID(), (GLuint)i, bufSize, &length, &size, &type, nameBuf);
		std::string name = nameBuf;
		if (name == "model") {
			shader.setMat4("model", this->model);
		}
		else if (name == "projection") {
			shader.setMat4("projection", scene.projection);
		}
		else if (name == "view") {
			shader.setMat4("view", scene.view);
		}
		else if (name == "ambientStrength") {
			shader.setFloat("ambientStrength", scene.config.ambientStrength);
		}
		else if (name == "lightPos") {
			shader.setVec3("lightPos", scene.config.lightPosition);
		}
		else if (name == "lightColor") {
			shader.setVec4("lightColor", scene.config.lightColor);
		}
		else if (name == "lightIntensity") {
			shader.setFloat("lightIntensity", scene.config.lightIntensity);
		}
		else if (name == "currentTime") {
			shader.setFloat("currentTime", glfwGetTime());
		}
		else if (name == "windStrength") {
			shader.setFloat("windStrength", scene.config.windStrength);
		}
		else if (name == "swayReach") {
			shader.setFloat("swayReach", scene.config.swayReach);
		}
		else if (name == "windDirection") {
			shader.setVec2("windDirection", scene.config.windDirection);
		}
		else if (name == "lightColor") {
			shader.setVec4("lightColor", scene.config.lightColor);
		}
		else if (name == "lightIntensity") {
			shader.setFloat("lightIntensity", scene.config.lightIntensity);
		}
		else if (name == "skybox") {
			scene.currentTexture->bindTextureCubeMap();
			shader.setInt("skybox", scene.currentTexture->getTextureID());
		}
		//printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
		// Get info and pass it to shader
	}
}


void SceneObject::createArrayBuffer(const std::vector<float>& vertexData) {
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), &vertexData[0], GL_STATIC_DRAW));
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


