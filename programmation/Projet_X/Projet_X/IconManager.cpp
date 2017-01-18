#include "IconManager.h"

#include <boost/filesystem.hpp>
#include <CEGUI/CEGUI.h>

IconManager::IconManager()
:m_iconNum(0)
{
	ilEnable(IL_FILE_OVERWRITE);
	int nbIcon(loadIcons("GUI/imagesets/Icons.png", 50, 50));
	if(nbIcon != -1)
		createImageSet("GUI/imagesets/Icons.imageset", "GUI/imagesets/Icons.png", nbIcon, 50, 50);
	ilDisable(IL_FILE_OVERWRITE);
}
IconManager::~IconManager()
{

}

int IconManager::loadIcons(const std::string& ik_filePath, int i_width, int i_height)
{
	boost::filesystem::path dirPath(boost::filesystem::current_path());
	dirPath /= "Ressources\\Icon\\";

	if(!boost::filesystem::exists(dirPath)) 
		return -1;

	std::vector<std::vector<unsigned char>> images;
	for ( boost::filesystem::directory_iterator itr( dirPath ), end_itr; itr != end_itr; ++itr )
	{
		if (!boost::filesystem::is_directory(itr->status()))
		{
			
			loadImage(itr->path().string().c_str(), i_width, i_height, images);
		}
	}
	m_iconNum = images.size();
	std::vector<unsigned char> resultImage;
	concatenateImage(images, i_width, i_height, resultImage);

	ILuint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	
	ilTexImage(i_width * images.size(), i_height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, &resultImage[0]);
	ilSaveImage(ik_filePath.c_str());
	
	ilDeleteImages(1, &imageID);

	return images.size();
}

void IconManager::loadImage(const std::string& ip_path, int i_targetWidth, int i_targetHeight, std::vector<std::vector<unsigned char>>& i_images)
{
	//On créer l'image
	ILuint imageID;				// Create a image ID as a ULuint
	ILboolean success;			// Create a flag to keep track of success/failure
	ILenum error;				// Create a flag to keep track of the IL error state
	ilGenImages(1, &imageID); 		// Generate the image ID
	ilBindImage(imageID); 			// Bind the image

	success = ilLoadImage(ip_path.c_str()); 	// Load the image file

	// If we managed to load the image, then we can start to do things with it...
	if (success)
	{
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo imageInfo;
		iluGetImageInfo(&imageInfo);
		if (imageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// ... then attempt to conver it.
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			error = ilGetError();
			exit(-1);
		}
		scaleImage(i_targetWidth, i_targetHeight, i_images);
	}
  	else // If we failed to open the image file in the first place...
  	{
		error = ilGetError();
		exit(-1);
  	}

 	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

}

void IconManager::scaleImage(int i_targetWidth, int i_targetHeight, std::vector<std::vector<unsigned char>>& i_resultImage)
{
	i_resultImage.push_back(std::vector<unsigned char>(i_targetWidth * i_targetHeight * 3));
	int currentImage(i_resultImage.size() - 1);

	
	float sourceRatio((float)ilGetInteger(IL_IMAGE_WIDTH)/(float)ilGetInteger(IL_IMAGE_HEIGHT));
	float targetRatio((float)i_targetWidth/(float)i_targetHeight);

	int startX, startY;
	int width, height;
	if(targetRatio < sourceRatio)
	{
		width = sourceRatio * i_targetHeight;
		height = i_targetHeight;

		startX = (width - i_targetWidth)/2;
		startY = 0;
	}
	else
	{
		width = i_targetWidth;
		height = i_targetWidth / sourceRatio;

		startX = 0;
		startY = (i_targetHeight - height)/2;
	}
	iluScale(width, height, 0);

	ILubyte* p_data(ilGetData());
	for(int i(0); i < i_targetWidth; ++i)
		for(int j(0); j < i_targetHeight; ++j)
			for(int k(0); k < 3; ++k)
			{
				i_resultImage[currentImage][(i * 3) + (i_targetWidth * 3 * j) + k] = p_data[((i + startX) * 3) + (width * 3 * (j + startY)) + k];
			}
}

void IconManager::concatenateImage(const std::vector<std::vector<unsigned char>>& i_images, int i_width, int i_height, std::vector<unsigned char>& i_resultImage)
{
	i_resultImage.resize(i_images.size() * i_images[0].size());
	int count(0);
	for(int j(0); j < i_width; ++j)
		for(int n(0), resultImageSize(i_images.size()); n < resultImageSize; ++n)
			for(int i(0); i < i_height; ++i)
				for(int k(0); k < 3; ++k)
				{
					i_resultImage[count] = i_images[n][(i * 3) + (i_width * 3 * j) + k];
					++count;
				}
}

void IconManager::createImageSet(const std::string& ik_imageSetFilePath, const std::string& ik_imageFilePath, int i_nbIcon, int i_width, int i_height)
{
	//On ouvre le fichier
	std::ofstream ofstreamFile;
	ofstreamFile.open(ik_imageSetFilePath);

	char buffer[200];
	int bufferSize(0);

	//On écrit le header dans un fichier
	bufferSize = sprintf(buffer, "<Imageset autoScaled=\"vertical\" imagefile=\"%s\" name=\"ICON_RTS\" nativeHorzRes=\"%i\" nativeVertRes=\"%i\" version=\"2\">\n", 
						ik_imageFilePath.substr(ik_imageFilePath.rfind('/') + 1).c_str(), i_width * i_nbIcon, i_height);
	ofstreamFile.write(buffer, bufferSize);

	for(int i(0); i < i_nbIcon; ++i)
	{
		bufferSize = sprintf(buffer, "<Image name=\"icon%i\" height=\"%i\" width=\"%i\" xPos=\"%i\" yPos=\"0\" />\n", i, i_width, i_height, i_width * i);
		ofstreamFile.write(buffer, bufferSize);
	}

	bufferSize = sprintf(buffer, "</Imageset>");
	ofstreamFile.write(buffer, bufferSize);

	//On ferme le fichier
	ofstreamFile.close();
	
}