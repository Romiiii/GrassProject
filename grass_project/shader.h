/*
 * Shader class that handles initializing the shader, activating the shader
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
    unsigned int ID;

	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	/* Initialize, compile and link vertex and fragment shader.
	 * \param vertexPath - path to vertex shader code
	 * \param framentPath - path to fragment shader code
	 */
	void initialize(const char* vertexPath, const char* fragmentPath);
	bool compile();
	bool isInitialized();

	/* Activate the shader.
	 */
	bool use() const;
	unsigned int getShaderID();

	/* Utility functions to set uniforms. 
	 */
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w) const;
	void setMat2(const std::string& name, const glm::mat2& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	const char* getFragmentPath();

private:
    bool initialized = false;
	unsigned int vertex;
	unsigned int fragment;
	const char* vertexPath;
	const char* fragmentPath;

	/* Utility function for checking shader compilation or linking errors. 
	 * \param shader - Shader ID 
	 * \param type - Shader type as all caps string
	 */
	bool checkCompileErrors(GLuint shader, std::string type);
};
#endif