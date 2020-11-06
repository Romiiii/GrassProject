#include "shader.h"
#include "debug.h"

Shader::Shader() {
	initialized = false;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	initialize(vertexPath, fragmentPath);
}

Shader::~Shader() {
	if (initialized) {
		glDeleteProgram(ID);
	}
}

/* Initialize, compile and link vertex and fragment shader.
	* \param vertexPath - path to vertex shader code
	* \param framentPath - path to fragment shader code
	*/
void Shader::initialize(const char* vertexPath, const char* fragmentPath) {
	this->vertexPath = vertexPath;
	this->fragmentPath = fragmentPath;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	compile();

	
	// Delete the shaders as they're linked into our program now
	//glDeleteShader(vertex);
	//glDeleteShader(fragment);
	initialized = true;
}

bool Shader::compile() {
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// Ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// Close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	bool success = checkCompileErrors(vertex, "VERTEX");
	if (!success) {
		return success;
	}
	// Fragment Shader

	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	success = checkCompileErrors(fragment, "FRAGMENT");
	if (!success) {
		return success;
	}
	// Shader Program
	ID = glCreateProgram();
	GLCall(glAttachShader(ID, vertex));
	GLCall(glAttachShader(ID, fragment));
	GLCall(glLinkProgram(ID));
	GLCall(glDetachShader(ID, vertex));
	GLCall(glDetachShader(ID, fragment));
	success = checkCompileErrors(ID, "PROGRAM");
	
	return success;
}


bool Shader::isInitialized() {
	return initialized;
}

/* Activate the shader.
	*/
bool Shader::use() const
{
	if (initialized)
		glUseProgram(ID);
	return initialized;
}

unsigned int Shader::getShaderID() {
	return ID;
}

/* Utility functions to set uniforms.
	*/
void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	GLCall(glUniform1f(glGetUniformLocation(ID, name.c_str()), value));
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

const char* Shader::getFragmentPath() {
	return fragmentPath;
}

/* Utility function for checking shader compilation or linking errors.
	* \param shader - Shader ID
	* \param type - Shader type as all caps string
	*/
bool Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
				<< type << "\n" << infoLog <<
				"\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: "
				<< type << "\n" << infoLog <<
				"\n -- --------------------------------------------------- -- "
				<< std::endl;
		}
	}
	return success;
}
