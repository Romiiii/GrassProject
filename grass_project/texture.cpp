#include "texture.h"

void Texture::bindTexture() {
	GLCall(glActiveTexture(GL_TEXTURE0 + textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
}

void Texture::bindTextureCubeMap() {
	GLCall(glActiveTexture(GL_TEXTURE0 + textureID));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, textureID));
}

/* Loads a 2D texture from a specified file.
 * \param fileName - texture's filename
 * \param alpha - set to true if alpha channel should be read from texture
 * \return textureID
 */
unsigned int Texture::loadTexture(std::string fileName, bool alpha) {
	GLCall(glGenTextures(1, &textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, textureID));

	int width, height, nrChannels;
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alpha) {
			GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
		}
		else {
			GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
		}

		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		std::cout << "Texture failed to load at path: " << fileName << std::endl;
		return 0;
	}
	stbi_image_free(data);
	return textureID;
}

void Texture::generateTexture(void* data, int width, int height, GLenum format) {
	GLCall(glGenTextures(1, &textureID));
	loadTextureData(data, width, height, GL_RED);
}


/* Loads a 2D texture from a specified file.
 * \param fileName - texture's filename
 * \param alpha - set to true if alpha channel should be read from texture
 * \return textureID
 */
unsigned int Texture::loadTextureData(void* data, int width, int height, GLenum format) {
	GLCall(glBindTexture(GL_TEXTURE_2D, textureID));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, data));

	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	

	return textureID;
}


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
unsigned int Texture::loadTextureCubeMap(std::vector<std::string> faces, bool alpha) {
	//unsigned int textureID;
	GLCall(glGenTextures(1, &textureID));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, textureID));

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			if (alpha) {
				GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
			}
			else {
				GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
			}

		}
		else
		{
			std::cout << "Texture failed to load at path: " << faces[i] << std::endl;
			return 0;
		}
		stbi_image_free(data);
	}
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	return textureID;
}

unsigned int Texture::getTextureID() {
	return textureID;
}

