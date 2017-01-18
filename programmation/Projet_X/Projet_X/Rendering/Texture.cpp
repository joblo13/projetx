#include "Texture.h"

//Includes externes
#include "../Shaders/Shader.h"
#include "../Debugger/GlDebugger.h"


#include <boost/filesystem.hpp>

//Initialisation de l'instance
std::hash_map<unsigned int, Texture*> Texture::mp_textures;
int Texture::m_currentTexture(0);
int Texture::m_counter(1);

////////////////////////////////////////////////////////////////////////
///
/// @fn Texture(const char* ikp_imagepath, int i_textureIndex)
///
/// Constructeur
/// 
/// @param[in] imagePath: Chemin d'accès de l'image sur le disque
/// @param[in] i_textureIndex: emplacement choisie pour la texture
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Texture::Texture(int i_textureIndex, const char* ikp_imagepath)
:m_imagePath(ikp_imagepath), m_textureIndex(i_textureIndex), m_width(0), m_height(0), m_format(IL_RGBA)
{
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Texture()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Texture::~Texture()
{

}

////////////////////////////////////////////////////////////////////////
///
/// @fn void load()
///
/// fonction chargeant l'image sur la carte graphique
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Texture::load()
{
	ILuint imageID;				// Create a image ID as a ULuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the image

	success = ilLoadImage(m_imagePath.c_str()); 	// Load the image file

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
			std::cout << "Image conversion failed - IL reports error: " << error << std::endl;
			exit(-1);
		}

		// Generate a new texture
		glGenTextures(1, &m_textureLocation);
		//On choisi la texture courante sur la carte graphique
		glActiveTexture(GL_TEXTURE0 + m_textureIndex);
		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, m_textureLocation);

		m_width = ilGetInteger(IL_IMAGE_WIDTH);
		m_height = ilGetInteger(IL_IMAGE_HEIGHT);
		m_format = ilGetInteger(IL_IMAGE_FORMAT);

		ILubyte* tempImageData = ilGetData();
		for(int i(0), imageSize(m_width * m_height * getFormatSize()); i < imageSize; ++i)
		{
			m_data.push_back(tempImageData[i]);
		}

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
					 0,				// Pyramid level (for mip-mapping) - 0 is the top level
					 ilGetInteger(IL_IMAGE_BPP),	// Image colour depth
					 m_width,	// Image width
					 m_height,	// Image height
					 0,				// Border width in pixels (can either be 1 or 0)
					 getOpenGLFormat(),	// Image format (i.e. RGB, RGBA, BGR etc.)
					 GL_UNSIGNED_BYTE,		// Image data type
					 &m_data[0]);			// The actual image data itself



		glGenerateMipmap(GL_TEXTURE_2D);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Set texture interpolation method to use linear interpolation
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);//GL_NEAREST_MIPMAP_LINEAR
 	}
  	else // If we failed to open the image file in the first place...
  	{
		error = ilGetError();
		std::cout << "Image load failed - IL reports error: " << error << std::endl;
		exit(-1);
  	}

 	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	std::cout << "Texture creation successful." << std::endl;
}

void Texture::loadFromCurrentDevilImage()
{
	Texture::bindTexture();
	mp_textures[m_currentTexture]->m_width = ilGetInteger(IL_IMAGE_WIDTH);
	mp_textures[m_currentTexture]->m_height = ilGetInteger(IL_IMAGE_HEIGHT);

	if(mp_textures[m_currentTexture]->m_width == 0 || mp_textures[m_currentTexture]->m_height == 0)
		return;
	mp_textures[m_currentTexture]->m_format = ilGetInteger(IL_IMAGE_FORMAT);

	ILubyte* tempImageData = ilGetData();
	for(int i(0), imageSize(mp_textures[m_currentTexture]->m_width * mp_textures[m_currentTexture]->m_height * mp_textures[m_currentTexture]->getFormatSize()); i < imageSize; ++i)
	{
		mp_textures[m_currentTexture]->m_data.push_back(tempImageData[i]);
	}

	glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
				0,				// Pyramid level (for mip-mapping) - 0 is the top level
				ilGetInteger(IL_IMAGE_BPP),	// Image colour depth
				mp_textures[m_currentTexture]->m_width,	// Image width
				mp_textures[m_currentTexture]->m_height,	// Image height
				0,				// Border width in pixels (can either be 1 or 0)
				mp_textures[m_currentTexture]->getOpenGLFormat(),	// Image format (i.e. RGB, RGBA, BGR etc.)
				GL_UNSIGNED_BYTE,		// Image data type
				ilGetData());			// The actual image data itself
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void load()
///
/// fonction chargeant l'emplacement d'une image sur la carte graphique
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Texture::loadEmpty()
{
	// Generate a new texture
	glGenTextures(1, &m_textureLocation);
	//On choisi la texture courante sur la carte graphique
	//int oldCurrentTexture(m_currentTexture);
	//m_currentTexture = m_counter;
	//bindTexture();
	//m_currentTexture = oldCurrentTexture;

	glBindTexture(GL_TEXTURE_2D, mp_textures[m_counter]->m_textureLocation);

	// Set texture clamping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// Set texture interpolation method to use linear interpolation
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR_MIPMAP_LINEAR
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void activeTexture()
///
/// Fonction choisisant la texture courante dans la carte graphique
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Texture::bindTexture()
{
	glActiveTexture(GL_TEXTURE0 + mp_textures[m_currentTexture]->m_textureIndex);
	//On choisi la texture courante
	glBindTexture(GL_TEXTURE_2D, mp_textures[m_currentTexture]->m_textureLocation);
}

void Texture::linkTexture()
{
	//On envoi au shader le numéro de la texture
	if(mp_textures[m_currentTexture]->m_textureIndex != 0)
		glUniform1i(Shader::getShader()->getUniformVariable(mp_textures[m_currentTexture]->m_variableName), mp_textures[m_currentTexture]->m_textureIndex);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void getTextureDimension(int i_textureIndex, unsigned int& i_width, unsigned int& i_height)
///
/// Fonction permettant de récuprer les dimensions de la texture
///
/// @param[in] i_width: largeur de l'image
/// @param[in] i_height: hauteur de l'image
/// @param[in] i_textureIndex: emplacement choisie pour la texture
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Texture::getTextureDimension(unsigned int& i_width, unsigned int& i_height)
{
	i_width = mp_textures[m_currentTexture]->m_width; 
	i_height = mp_textures[m_currentTexture]->m_height;
}
int Texture::getTextureLocation()
{
	return mp_textures[m_currentTexture]->m_textureLocation;
}

std::string Texture::getVariableName()
{
	return m_variableName;
}

void Texture::createImageSet()
{	
	int beginPos(mp_textures[m_currentTexture]->m_imagePath.rfind("/") + 1);
	std::string imageName(mp_textures[m_currentTexture]->m_imagePath.substr(beginPos, mp_textures[m_currentTexture]->m_imagePath.length() - beginPos - 4));
	int nbIcon(mp_textures[m_currentTexture]->m_width/mp_textures[m_currentTexture]->m_height);
	int subImageSize(mp_textures[m_currentTexture]->m_height);


	//On ouvre le fichier
	std::ofstream ofstreamFile;
	ofstreamFile.open("GUI/imagesets/" + imageName + ".imageset");

	char buffer[200];
	int bufferSize(0);

	//On écrit le header dans un fichier
	bufferSize = sprintf(buffer, "<Imageset autoScaled=\"vertical\" imagefile=\"%s\" name=\"%s\" nativeHorzRes=\"%i\" nativeVertRes=\"%i\" version=\"2\">\n", 
						("../../" + mp_textures[m_currentTexture]->m_imagePath).c_str(), imageName.c_str(), subImageSize * nbIcon, subImageSize);
	ofstreamFile.write(buffer, bufferSize);

	for(int i(0); i < nbIcon; ++i)
	{
		bufferSize = sprintf(buffer, "<Image name=\"icon%i\" height=\"%i\" width=\"%i\" xPos=\"%i\" yPos=\"0\" />\n", i, subImageSize, subImageSize, subImageSize * i);
		ofstreamFile.write(buffer, bufferSize);
	}

	bufferSize = sprintf(buffer, "</Imageset>");
	ofstreamFile.write(buffer, bufferSize);

	//On ferme le fichier
	ofstreamFile.close();

	CEGUI::ImageManager::getSingleton().loadImageset(imageName + ".imageset", "imagesets");
}

int Texture::getTextureID(const std::string& i_imagePath)
{
	for(std::hash_map<unsigned int, Texture*>::iterator it(mp_textures.begin()), endTextures(mp_textures.end());
		it != endTextures; ++it)
		if(it->second->getImagePath() == i_imagePath)
			return it->first;
	return -1;
}

ILubyte* Texture::getData()
{
	return &mp_textures[m_currentTexture]->m_data[0];
}

int Texture::getOpenGLFormat()
{
	switch(m_format)
	{
		case IL_RGB:
			return GL_RGB;
     
		case IL_RGBA: 
			return GL_RGBA;

		default:
			assert(false);
	}
}

CEGUI::Texture::PixelFormat Texture::getCEGUIFormat()
{
	switch(m_format)
	{
		case IL_RGB:
			return CEGUI::Texture::PF_RGB;
     
		case IL_RGBA: 
			return CEGUI::Texture::PF_RGBA;

		default:
			assert(false);
	}
}

int Texture::getFormatSize()
{
	switch(m_format)
	{
		case IL_RGB:
			return 3;
     
		case IL_RGBA: 
			return 4;

		default:
			assert(false);
	}
}


void Texture::createCEGUITexture(LayoutType i_layoutType, const std::string& i_name)
{
	static CEGUI::OpenGLTexture &guiTex = static_cast<CEGUI::OpenGLTexture&>(CEGUI::System::getSingleton().getRenderer()->createTexture("tex_" + i_name));
	static CEGUI::BasicImage* image = (CEGUI::BasicImage*)( &CEGUI::ImageManager::getSingleton().create("BasicImage", "RTTImg_" + i_name));
	image->setTexture(&guiTex);
}

void Texture::updateCEGUITexture(LayoutType i_layoutType, const std::string& i_name, float i_scalling, float i_offsetX, float i_offsetY)
{
	static CEGUI::OpenGLTexture &guiTex(static_cast<CEGUI::OpenGLTexture&>(CEGUI::System::getSingleton().getRenderer()->getTexture("tex_" + i_name)));
	CEGUI::Sizef oldSize(guiTex.getSize());

	unsigned int width, height;
	Texture::getTextureDimension(width, height);



	if(oldSize.d_width != width * i_scalling || oldSize.d_height != height * i_scalling)
	{
		CEGUI::Sizef size(width, height);
		guiTex.setOpenGLTexture(Texture::getTextureLocation(), size);

		// put the texture in an image
		
		CEGUI::Vector2f centerPos(size.d_width/2.f, size.d_width/2.f);
		CEGUI::Vector2f halfSize(size.d_width/2.f * i_scalling, size.d_width/2.f * i_scalling);

		const CEGUI::Rectf rect((centerPos + CEGUI::Vector2f(i_offsetX, i_offsetY)) - halfSize, (centerPos + CEGUI::Vector2f(i_offsetX, i_offsetY)) + halfSize);
		static CEGUI::BasicImage* image = (CEGUI::BasicImage*)( &CEGUI::ImageManager::getSingleton().get("RTTImg_" + i_name));
		image->setArea(rect);
	}
}

void Texture::changePixelColor(int i_x, int i_y, unsigned char i_color[3])
{
	glTexSubImage2D(GL_TEXTURE_2D, 0, i_x, i_y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, i_color);
}

void Texture::changePixelGroupColor(int i_x, int i_y, unsigned char i_color[3], int i_sizeX, int i_sizeY)
{

	std::vector<unsigned char> color(i_sizeX * i_sizeY * 3);
	for(int i(0), sizeColor(color.size()); i < sizeColor; ++i)
		color[i] = i_color[i % 3];
	glTexSubImage2D(GL_TEXTURE_2D, 0, i_x, i_y, i_sizeX, i_sizeY, GL_RGB, GL_UNSIGNED_BYTE, &color[0]);
}