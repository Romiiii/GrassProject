#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <vector>

#include "debug.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

/**
* \brief Represents an OpenGL texture
*/
class Texture {
public:
	/**
	 * \brief Creates a new texture
	 */
	void create(GLuint textureType);

	/**
	 * \brief Activates the slot of this texture
	 */
	void activate();

	/**
	 * \brief Set the wrap mode
	 */
	void setWrap(GLuint wrapType);

	/**
	 * \brief Set the filter mode
	 */
	void setFilter(GLuint filterType);

	/**
	 * \brief Generate mip maps
	 */
	void generateMipmap();

	/**
	 * \brief Binds this texture
	 */
	void bind();

	/**
	 * \brief Loads a 2D texture from a specified file.
	 * \param fileName texture's filename
	 * \param alpha set to true if alpha channel should be read from texture
	 * \return textureID
	 */
	unsigned int loadTexture(const std::string &fileName, bool alpha = true);

	void setLabel(const std::string &label);

	/**
	 * \brief Loads a 2D texture from a specified file.
	 * \param fileName texture's filename
	 * \param alpha set to true if alpha channel should be read from texture
	 * \return textureID
	 */
	unsigned int loadTextureSingleChannel(const std::string &name, int perlinNoiseSize);

	void generateTexture(void *data, int width, int height, GLenum format);

	unsigned int loadTextureData(void *data, int width, int height, GLenum format);

	/* Loads a cubemap texture from 6 individual texture faces.
	 * Order:
	 * +X (right)
	 * -X (left)
	 * +Y (top)
	 * -Y (bottom)
	 * +Z (front)
	 * -Z (back)
	 * \param faces - filenames of faces
	 * \param alpha - set to true if alpha channel should be read from texture
	 * \return textureID
	 */
	unsigned int loadTextureCubeMap(std::vector<std::string> faces, bool alpha = true);

	unsigned int getTextureID();
	unsigned int getTextureType();

private:
	GLuint textureID;
	GLuint textureType;
};



#endif