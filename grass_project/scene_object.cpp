#include "scene_object.h"
#include "scene.h"


SceneObject::SceneObject(ShaderProgram& shaderProgram)
	: shaderProgram(shaderProgram) {
}	

void SceneObject::setUniforms(Scene& scene) {
	GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 128; // maximum name length
	GLchar nameBuf[bufSize]; // variable name in GLSL
	GLsizei length; // name length

	glGetProgramiv(shaderProgram.getShaderProgramId(), GL_ACTIVE_UNIFORMS, 
		&count);

	for (i = 0; i < count; i++)
	{
		glGetActiveUniform(shaderProgram.getShaderProgramId(), 
			(GLuint)i, bufSize, &length, &size, &type, nameBuf);
		std::string name = nameBuf;

		if (name == "model") {
			shaderProgram.setMat4("model", this->model);
		}
		else if (name == "projection") {
			shaderProgram.setMat4("projection", scene.projection);
		}
		else if (name == "view") {
			shaderProgram.setMat4("view", scene.view);
		}
		else if (name == "ambientStrength") {
			shaderProgram.setFloat("ambientStrength", 
				scene.config.ambientStrength);
		}
		else if (name == "lightPos") {
			shaderProgram.setVec3("lightPos", scene.config.lightPosition);
		}
		else if (name == "lightColor") {
			shaderProgram.setVec4("lightColor", scene.config.lightColor);
		}
		else if (name == "lightIntensity") {
			shaderProgram.setFloat("lightIntensity", 
				scene.config.lightIntensity);
		}
		else if (name == "currentTime") {
			shaderProgram.setFloat("currentTime", (float)glfwGetTime());
		}
		else if (name == "windStrength") {
			shaderProgram.setFloat("windStrength", scene.config.windStrength);
		}
		else if (name == "swayReach") {
			shaderProgram.setFloat("swayReach", scene.config.swayReach);
		}
		else if (name == "lightColor") {
			shaderProgram.setVec4("lightColor", scene.config.lightColor);
		}
		else if (name == "lightIntensity") {
			shaderProgram.setFloat("lightIntensity", 
				scene.config.lightIntensity);
		}
		else if (name == "skybox") {
			scene.currentSkyboxTexture->activate();
			scene.currentSkyboxTexture->bind();
			shaderProgram.setInt("skybox", 
				scene.currentSkyboxTexture->getTextureID());
		}
		else if (name == "windX") {
			if (scene.config.windX != nullptr) {
				scene.config.windX->activate();
				scene.config.windX->bind();
				shaderProgram.setInt("windX", scene.config.windX->getTextureID());
			}
			else {
				shaderProgram.setInt("windX", 0);
			}
		}
		else if (name == "windY") {
			if (scene.config.windY != nullptr) {
				scene.config.windY->activate();
				scene.config.windY->bind();
				shaderProgram.setInt("windY", scene.config.windY->getTextureID());
			}
			else {
				shaderProgram.setInt("windY", 0);
			}
		}
		else if (name == "textureScale") {
			shaderProgram.setFloat("textureScale",
				scene.config.perlinConfig.textureScale); //TODO:: should not be in perlinconfig
		}
		else if (name == "visualizeTexture") {
			shaderProgram.setBool("visualizeTexture", 
				scene.config.visualizeTexture);
		}
		else if (name == "windDirection") {
			shaderProgram.setVec2("windDirection", scene.config.windDirection);
		}
		else if (name == "debugBlades") {
			shaderProgram.setBool("debugBlades", scene.config.debugBlades);
		}
		//else if (name == "simulationMode") {
		//	shaderProgram.setInt("simulationMode", scene.config.debugBlades);
		//}
	}
}


void SceneObject::createArrayBuffer(const std::vector<float>& vertexData) {
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), 
		&vertexData[0], GL_STATIC_DRAW));
}

/* Sets the specified attribute of the vertex shaderProgram
	*/
void SceneObject::setVertexShaderAttribute(
	char *attributeName,
	const std::vector<float>& data, 
	int dataSize, 
	ShaderProgram& shaderProgram) {
	createArrayBuffer(data);  // Creates and binds the VBO
	int attributeLocation = glGetAttribLocation(
		shaderProgram.getShaderProgramId(), attributeName);
	//assert(attributeLocation != -1);
	//std::cout << attributeLocation << std::endl;
	if (attributeLocation == -1) {

		std::cout << "\x1B[01;93mWARNING: Attribute: " 
			<< attributeName 
			<< " - not found. Using shader: " 
			<< shaderProgram.getName() 
			<< "\x1B[0m" << std::endl;
	}
	else {
		GLCall(glEnableVertexAttribArray(attributeLocation));
		GLCall(glVertexAttribPointer(attributeLocation, dataSize, 
			GL_FLOAT, GL_FALSE, 0, 0));
	}

}

unsigned int SceneObject::createElementArrayBuffer(const std::vector<unsigned int>& array) {
	unsigned int EBO;
	GLCall(glGenBuffers(1, &EBO));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		array.size() * sizeof(unsigned int), &array[0], GL_STATIC_DRAW));

	return EBO;
}


