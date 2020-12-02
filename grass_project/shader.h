/*
 * Shader class that handles initializing the shaderProgram, activating the shaderProgram
 * and setting uniforms. 
 */
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:

	/* Initialize, compile and link vertex and fragment shaderProgram.
	 * \param vertexPath - path to vertex shaderProgram code
	 * \param framentPath - path to fragment shaderProgram code
	 */
	Shader(const char* shaderPath, GLenum type);
	~Shader();

	bool compile();
	bool isInitialized();

	unsigned int getShaderId();

private:
    bool initialized = false;
	unsigned int id = 0;
	const char* path = nullptr;
	GLenum type = 0;

	std::string shaderTypeToString();

	/* Utility function for checking shaderProgram compilation or linking errors. 
	 * \param shaderProgram - Shader id 
	 * \param type - Shader type as all caps string
	 */
	bool checkCompileErrors();
};
#endif