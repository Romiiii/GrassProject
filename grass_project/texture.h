/*
 * The Texture class handles loading and binding textures.
 */
#ifndef TEXTURE_H
#define TEXTURE_H

 // Supress third party warnings
#pragma warning (push, 0)
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb_image.h>  // For loading textures from images
#include <vector>
#pragma warning (pop)

#include "debug.h"

class Texture {
public:
	void bindTexture();
	void bindTextureCubeMap();

	/* Loads a 2D texture from a specified file.
	 * \param fileName - texture's filename
	 * \param alpha - set to true if alpha channel should be read from texture
	 * \return textureID
	 */
	unsigned int loadTexture(std::string fileName, bool alpha = true);

	unsigned int loadTextureSingleChannel(const std::string& name, int perlinNoiseSize);

	void generateTexture(void* data, int width, int height, GLenum format);
	
	unsigned int Texture::loadTextureData(void* data, int width, int height, GLenum format);

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

private:
	GLuint textureID;
};

#endif