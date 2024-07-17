#include "shader.h"
#include "debug.h"

Shader::Shader(const char* path, GLenum type) {
	this->path = path;
	this->type = type;
	id = glCreateShader(type);
	if (compile()) {
		initialized = true;
	}
}

Shader::~Shader() {
	if (initialized) {
		glDeleteProgram(id);
	}
}

bool Shader::compile() {
	std::string code;
	std::ifstream fileStream;

	// Ensure ifstream objects can throw exceptions:
	fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// Open files
		fileStream.open(path);
		std::stringstream codeStream;
		// Read file's buffer contents into streams
		codeStream << fileStream.rdbuf();
		// Close file handlers
		fileStream.close();
		// Convert stream into string
		code = codeStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* code_c = code.c_str();
	
	glShaderSource(id, 1, &code_c, NULL);
	glCompileShader(id);
	bool success = checkCompileErrors();
	if (!success) {
		return success;
	}

	return success;
}


bool Shader::isInitialized() {
	return initialized;
}

unsigned int Shader::getShaderId() {
	return id;
}

std::string Shader::shaderTypeToString() {
	switch (type) {
	case GL_VERTEX_SHADER:
		return "vertex";
	case GL_FRAGMENT_SHADER:
		return "fragment";
	case GL_COMPUTE_SHADER:
		return "compute";
	default:
		return "unknown";
	}
}

/* Utility function for checking shaderProgram compilation or linking errors.
	* \param shaderProgram - Shader id
	* \param type - Shader type as all caps string
	*/
bool Shader::checkCompileErrors()
{
	GLint success;
	GLchar infoLog[1024];

	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(id, 1024, NULL, infoLog);
		std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
			<< shaderTypeToString() << "\n" << infoLog <<
			"\n -- --------------------------------------------------- -- "
			<< std::endl;
	}
	

	return success;
}
