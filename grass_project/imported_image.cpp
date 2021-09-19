#include "imported_image.h"

#include <exception>
#include <sstream>

// Supress third party warnings
#pragma warning (push, 0)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>  // For loading textures from images
#pragma warning (pop)

ImportedImage::ImportedImage(const std::string &file)
{
	data = stbi_load(file.c_str(), &width, &height, &channels, 0);
	if (!data)
	{
		std::stringstream buffer;
		buffer << "Texture failed to load at path: " << file;
		throw new std::invalid_argument(buffer.str());
	}
}

ImportedImage::~ImportedImage()
{
	stbi_image_free(data);
}
