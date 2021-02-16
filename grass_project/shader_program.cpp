#include "shader_program.h"
#include "debug.h"

ShaderProgram::ShaderProgram(std::vector<Shader*> shaders, const std::string& name) : shaders(shaders), name(name) {
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

const std::string& ShaderProgram::getName()
{
	return name;
}

void ShaderProgram::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void ShaderProgram::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const
{
	GLCall(glUniform1f(glGetUniformLocation(id, name.c_str()), value));
}

void ShaderProgram::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}
void ShaderProgram::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void ShaderProgram::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void ShaderProgram::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
}

void ShaderProgram::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
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

