#ifndef ICON_MANAGER_H
#define ICON_MANAGER_H

#include <string>
#include <vector>

#define ILUT_USE_OPENGL	// This MUST be defined before calling the DevIL headers or we don't get OpenGL functionality
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

class IconManager
{
    public:
		IconManager();
		~IconManager();
		int getIconNum(){return m_iconNum;}
    private:
		int loadIcons(const std::string& ik_filePath, int i_width, int i_height);
		void createImageSet(const std::string& ik_imageSetFilePath, const std::string& ik_imageFilePath, int i_nbIcon, int i_width, int i_height);
		void loadImage(const std::string& ip_path, int i_targetWidth, int i_targetHeight, std::vector<std::vector<unsigned char>>& i_image);
		void scaleImage(int i_targetWidth, int i_targetHeight, std::vector<std::vector<unsigned char>>& i_resultImage);
		void concatenateImage(const std::vector<std::vector<unsigned char>>& i_images, int i_width, int i_height, std::vector<unsigned char>& i_resultImage);

		int m_iconNum;
};

#endif //ICON_MANAGER_H
