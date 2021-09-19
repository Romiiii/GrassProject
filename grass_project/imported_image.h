#ifndef IMPORTED_IMAGE_H
#define IMPORTED_IMAGE_H

#include <string>

/**
 * \brief RAII wrapper for an STBI-imported image.
 * Constructor loads the image, destructor deletes it. 
 * Remember to copy the data you want before the destructor fires.
 */
class ImportedImage
{
public:
	/**
	 * \brief Loads the image.
	 * \param file Path the the file you want to load.
	 * \exception std::invalid_argument If the file found not be found.
	 */
	ImportedImage(const std::string &file);

	/**
	 * \brief Unloads the image, frees ImportedImage::data
	 */
	~ImportedImage();

	/**
	 * \brief Number of channels
	 */
	int channels;

	/**
	 * \brief Width of the image
	 */
	int width;

	/**
	 * \brief Height of the image
	 */
	int height;

	/**
	 * \brief Image pixel data
	 */
	unsigned char *data;

};


#endif
