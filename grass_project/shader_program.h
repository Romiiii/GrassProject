#ifndef H_SHADER_PROGRAM
#define H_SHADER_PROGRAM

#include "shader.h"

#include <vector>
#include <string>

/**
 * @brief Represents a opengl shader program
 */
class ShaderProgram {
public:
	/**
	 * @brief Creates shader program from shaders, with name
	 */
	ShaderProgram(std::vector<Shader*> shaders, const std::string& name);

	unsigned int getShaderProgramId();

	void linkShaders();

	void use() const;

	/**
	 * @brief Recompile and relink the shaders
	 */
	void reloadShaders();

	/**
	 * @brief Creates shader program from shaders, with name
	 */
	const std::string& getName() const;

	GLint getUniformLocation(const std::string& name) const;

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

private:
	unsigned int id;
	Shader* fragment;
	Shader* vertex;
	std::vector<Shader*> shaders;
	std::string name;

	void checkShaderProgramError();
};

#endif // !H_SHADER_PROGRAM
