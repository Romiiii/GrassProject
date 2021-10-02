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

/**
 * \brief Abstraction of a shader file
 */
class Shader {
public:

	/**
	 * @brief Initialize and load a shader
	 * @param shaderPath path to the shader
	 * @param type The type of shader: Vertex/Fragment/Compute
	*/
	Shader(const char* shaderPath, GLenum type);

	~Shader();

	/**
	 * @brief Compile the shader
	 * @return Returns true if compilation succeeded
	*/
	bool compile();

	/**
	 * @brief Checks if the shader is ready for use
	 * @return Returns true if shader is ready for use
	*/
	bool isInitialized();

	unsigned int getShaderId();

private:
	/**
	 * @brief Is the shader initialized?
	*/
	bool initialized = false;

	/**
	 * @brief The id
	*/
	unsigned int id = 0;

	/**
	 * @brief Path to the shader file, used for runtime-recompilation
	*/
	const char* path = nullptr;

	/**
	 * @brief Shader type: Vertex/Fragment/Compute
	*/
	GLenum type = 0;

	/**
	 * @brief Helper function for getting a string representation of the shader type
	 * @return The string representation of the shader type eg. "Vertex" 
	*/
	std::string shaderTypeToString();

	/**
	 * \brief Utility function for checking shaderProgram compilation or linking errors. 
	 * \param shaderProgram - Shader id 
	 * \param type - Shader type as all caps string
	 */
	bool checkCompileErrors();
};
#endif