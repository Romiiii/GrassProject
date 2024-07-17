#include "shader_program.h"
#include "debug.h"

ShaderProgram::ShaderProgram(std::vector<Shader *> shaders, const std::string &name) : shaders(shaders), name(name) {
	id = glCreateProgram();
	linkShaders();
}

unsigned int ShaderProgram::getShaderProgramId()
{
	return id;
}

void ShaderProgram::linkShaders()
{
	for (auto shader : shaders) {
		GLCall(glAttachShader(id, shader->getShaderId()));
	}

	GLCall(glLinkProgram(id));

	for (auto shader : shaders) {
		GLCall(glDetachShader(id, shader->getShaderId()));
	}
	checkShaderProgramError();
}

void ShaderProgram::use() const
{
	for (auto shader : shaders) {
		assert(shader->isInitialized());
	}

	glUseProgram(id);
}

void ShaderProgram::reloadShaders()
{
	for (auto shader : shaders) {
		shader->compile();

	}
	linkShaders();

}

const std::string &ShaderProgram::getName() const
{
	return name;
}

GLint ShaderProgram::getUniformLocation(const std::string &name) const
{
	GLint location = glGetUniformLocation(id, name.c_str());
	if (location == -1)
	{
		std::cout << "\x1B[01;93mWARNING: Uniform location: "
			<< name
			<< " - not found. Using shader: "
			<< getName()
			<< "\x1B[0m" << std::endl;
	}
	return location;
}

void ShaderProgram::setBool(const std::string &name, bool value) const
{
	glUniform1i(getUniformLocation(name), (int)value);
}

void ShaderProgram::setInt(const std::string &name, int value) const
{
	glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setFloat(const std::string &name, float value) const
{
	GLCall(glUniform1f(getUniformLocation(name), value));
}

void ShaderProgram::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(getUniformLocation(name), 1, &value[0]);
}
void ShaderProgram::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(getUniformLocation(name), x, y);
}

void ShaderProgram::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void ShaderProgram::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(getUniformLocation(name), x, y, z);
}

void ShaderProgram::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(getUniformLocation(name), 1, &value[0]);
}

void ShaderProgram::setVec4(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(getUniformLocation(name), x, y, z, w);
}

void ShaderProgram::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}


void ShaderProgram::checkShaderProgramError()
{
	int success;
	GLchar infoLog[1024];
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 1024, NULL, infoLog);
		std::cout << "ERROR::PROGRAM_LINKING_ERROR with name: "
			<< name << "\n" << infoLog <<
			"\n -- --------------------------------------------------- -- "
			<< std::endl;
	}
}

