#ifndef TEXTURE_H
#define TEXTURE_H

#include "../Definition.h"

#include "../GUI/GUIHandler.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/Texture.h>


#define ILUT_USE_OPENGL	// This MUST be defined before calling the DevIL headers or we don't get OpenGL functionality
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <string>
#include <hash_map>
#include <utility>
#include <assert.h>
#include <hash_set>

class Texture
{
	public:
		//Singleton
		static int getNewTextureID()
		{
			++m_counter;
			return m_counter - 1;
		}
		static void chooseTexture(int i_index)
		{
			if(m_currentTexture != i_index)
			{
				//On choisi la texture comme texture courante
				m_currentTexture = i_index;
			}
		}

		static int initCEGUITexture()
		{
			//Texture 0 réservé pour CEGUI
			mp_textures[0] = new Texture(0);
			return 0;
		}

		static int getTextureID(const std::string& i_variableName, const std::string& i_path)
		{
			int textureID(getTextureID(i_path));
			if(textureID = -1)
				textureID = initTexture(i_variableName, i_path);
			return textureID;
		}

		static int initTexture(const std::string& i_variableName = "", const std::string& i_path = "")
		{

			if(i_path == "")
			{
				mp_textures[m_counter] = new Texture(m_counter);
				//On charge l'image sur la carte graphique
				mp_textures[m_counter]->loadEmpty();
			}
			else
			{
				mp_textures[m_counter] = new Texture(m_counter, i_path.c_str());
				//On charge l'image sur la carte graphique
				mp_textures[m_counter]->load();	
			}
			mp_textures[m_counter]->m_variableName = i_variableName;
			mp_textures[m_counter]->m_imagePath = i_path;

			return getNewTextureID();
		}

		static void releaseTextures()
		{
			//On détruit tout les shaders utilisés
			for (std::hash_map<unsigned int, Texture*>::iterator iter = mp_textures.begin(); iter != mp_textures.end(); ++iter)
			{
				
				if (iter->second != 0)
				{
					delete iter->second;
					iter->second = 0;
					
				}
			}
			mp_textures.clear();
		}

		static void getTextureDimension(unsigned int& i_width, unsigned int& i_height);
		static int getTextureLocation();

		//Changement d'état d'opengl
		static void bindTexture();
		static void linkTexture();
		std::string getVariableName();
		const std::string& getImagePath(){return m_imagePath;}
		static void createImageSet();

		static int getTextureID(const std::string& i_imagePath);
		static int getImageCount(){return mp_textures[m_currentTexture]->m_width/mp_textures[m_currentTexture]->m_height;}

		static ILubyte* getData();
		

		static void createCEGUITexture(LayoutType i_layoutType, const std::string& i_name);
		static void updateCEGUITexture(LayoutType i_layoutType, const std::string& i_name, float i_scalling, float i_offsetX, float i_offsetY);

		static void loadFromCurrentDevilImage();


		static void changePixelColor(int i_x, int i_y, unsigned char i_color[3]);
		static void changePixelGroupColor(int i_x, int i_y, unsigned char i_color[3], int i_sizeX, int i_sizeY);

	private:
		//Singleton
		Texture(int i_textureIndex, const char* ikp_imagepath = "");
		~Texture();

		//Fonctions de chargement des informations des images
		void load();
		void loadEmpty();

		int getOpenGLFormat();
		CEGUI::Texture::PixelFormat getCEGUIFormat();
		int getFormatSize();
		void setFormatFromDevil();

		//Emplacement de l'image sur le disque
		std::string m_imagePath;
		int m_textureIndex;

		//Emplacement de l'image dans la carte graphiques
		std::string m_variableName;
		GLuint m_textureLocation;

		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_format;
		std::vector<ILubyte> m_data;

		static std::hash_map<unsigned int, Texture*> mp_textures;
		static int m_currentTexture;
		static int m_counter;
};
#endif // TEXTURE_H