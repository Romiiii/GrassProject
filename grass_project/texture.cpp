#include "texture.h"

#include "imported_image.h"

Texture::Texture(const std::string &label, GLuint textureType)
	:textureType(textureType)
{
	GLCall(glGenTextures(1, &textureID));
	bind();
	setLabel(label);
}

void Texture::generateMipmap()
{
	GLCall(glGenerateMipmap(textureType));
}

void Texture::setWrap(GLuint wrapType)
{
	GLCall(glTexParameteri(textureType, GL_TEXTURE_WRAP_S, wrapType));
	GLCall(glTexParameteri(textureType, GL_TEXTURE_WRAP_T, wrapType));
	GLCall(glTexParameteri(textureType, GL_TEXTURE_WRAP_R, wrapType));
}

void Texture::setFilter(GLuint filterType)
{
	GLCall(glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, filterType));
	GLCall(glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, filterType));
}

void Texture::activate()
{
	GLCall(glActiveTexture(GL_TEXTURE0 + textureID));
}

void Texture::bind()
{
	GLCall(glBindTexture(textureType, textureID));
}


unsigned int Texture::loadTexture(const std::string &fileName, bool alpha) {
	ImportedImage image(fileName);
	
	bind();

	GLuint format = alpha ? GL_RGBA : GL_RGB;
	GLCall(glTexImage2D(textureType, 0, format,
						image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data));

	generateMipmap();

	setFilter(GL_NEAREST);
	setWrap(GL_CLAMP_TO_EDGE);

	return textureID;
}

void Texture::setLabel(const std::string &label)
{
	GLCall(glObjectLabel(GL_TEXTURE, textureID, -1, label.c_str()));
}

unsigned int Texture::loadTextureSingleChannel(int perlinNoiseSize) {
	bind();

	setFilter(GL_NEAREST);
	setWrap(GL_REPEAT);

	GLCall(glTexImage2D(textureType, 0, GL_RED, perlinNoiseSize, perlinNoiseSize, 0, GL_RED, GL_FLOAT, 0));

	return textureID;
}


void Texture::generateTexture(void *data, int width, int height, GLenum format) {
	loadTextureData(data, width, height, format);
}


/* Loads a 2D texture from a specified file.
 * \param fileName - texture's filename
 * \param alpha - set to true if alpha channel should be read from texture
 * \return textureID
 */
unsigned int Texture::loadTextureData(void *data, int width, int height, GLenum format) {
	bind();

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, data));

	generateMipmap();

	setFilter(GL_NEAREST);
	setWrap(GL_CLAMP_TO_EDGE);

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
	bind();
	activate();

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		ImportedImage image(faces[i]);
		GLuint format = alpha ? GL_RGBA : GL_RGB;
		GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
							image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.data));

	}

	setFilter(GL_LINEAR);
	setWrap(GL_CLAMP_TO_EDGE);

	return textureID;
}

unsigned int Texture::getTextureID() {
	return textureID;
}

unsigned int Texture::getTextureType() {
	return textureType;
}
