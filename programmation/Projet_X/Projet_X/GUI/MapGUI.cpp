#include "MapGUI.h"

#define ILUT_USE_OPENGL	// This MUST be defined before calling the DevIL headers or we don't get OpenGL functionality
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "../Terrain/Terrain.h"
#include "../Rendering/Texture.h"
#include "../Shaders/Shader.h"
#include "../Math/Matrix.h"
#include "../Position.h"
#include "../Camera/Camera.h"
#include "../GUI/GUIHandler.h"
#include "../Input.h"
#include "../Entities/UnitHandler.h"
#include "../Entities/Unit.h"
#include "../SceneOpenGL.h"
#include "../Camera/CameraRTS.h"
#include "../Building/TemplateManager.h"

#include "../Debugger/GlDebugger.h"

MapGUI::MapGUI()
:m_scaling(1.f), m_offsetX(0.f), m_offsetY(0.f), m_mouseInside(false)
{
	m_onTop = false;
	m_needMouse = false;
	m_layout = CEGUI::WindowManager::getSingleton().loadLayoutFromFile( "Map.layout", "layouts");

	setVisible(false);
}

MapGUI::~MapGUI()
{

}

void MapGUI::updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera)
{
	Input* p_input(Input::getInstance());

	if(p_input->getKeyState("zoomIn") && m_mouseInside)
	{
		m_scaling += 0.2f;
		if(m_scaling > 10.f)
			m_scaling = 10.f;
	}
	if(p_input->getKeyState("zoomOut") && m_mouseInside)
	{
		m_scaling -= 0.2f;
		if(m_scaling < 0.2f)
			m_scaling = 0.2f;
	}

	updateImage();
}

void MapGUI::registerHandlers()
{
	m_layout->getChild("map")->subscribeEvent(CEGUI::Editbox::EventMouseEntersArea,
                    CEGUI::Event::Subscriber(&MapGUI::mouseEnterMap,this));
	m_layout->getChild("map")->subscribeEvent(CEGUI::Editbox::EventMouseLeavesArea,
                    CEGUI::Event::Subscriber(&MapGUI::mouseExitMap,this));

	m_layout->getChild("map/map_img")->subscribeEvent(CEGUI::Editbox::EventMouseButtonUp,
					CEGUI::Event::Subscriber(&MapGUI::mouseRelease,this));
}


void MapGUI::load()
{
	generateBaseColor();

	//On choisi la texture courante sur la carte graphique
	m_texturesIndex.push_back(Texture::initTexture("MapTexture"));
	Texture::chooseTexture(m_texturesIndex[0]);
	Texture::linkTexture();
	GUIHandler::getInstance()->createOpenGLImage(m_texturesIndex[0], MAP_LAYOUT, "map/map_img");
}

void MapGUI::generateBaseColor()
{
	ILuint imageID;				// Create a image ID as a ULuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the image

	success = ilLoadImage("Texture/terrain.png"); 	// Load the image file

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

		ILubyte* imageData = ilGetData();

		int imageWidth(ilGetInteger(IL_IMAGE_WIDTH));
		int imageHeight(ilGetInteger(IL_IMAGE_HEIGHT));
		int totalPixelNb(imageWidth * imageHeight);

		int pixelSize(ilGetInteger(IL_IMAGE_BPP));
		int textureNb(imageWidth/imageHeight);
		int texturesWidth(imageHeight);
		int pixelPerTextures(imageHeight * texturesWidth);

		std::vector<int> tempArray;
		tempArray.resize(3);

		for(int i(0); i < textureNb; ++i)
			m_colorAverage.push_back(tempArray);

		for(int i(0); i < totalPixelNb; i += imageWidth)
			for(int currentTexture(0); currentTexture < textureNb; ++currentTexture)
				for(int k(0); k < texturesWidth; ++k)
					for(int color(0); color < 3; ++color)
						m_colorAverage[currentTexture][color] += imageData[3 * (i + k + texturesWidth * currentTexture) + color];


		for(int i(0), colorAverageSize(m_colorAverage.size()); i < colorAverageSize; ++i)
			for(int color(0); color < 3; ++color)
				m_colorAverage[i][color] /= pixelPerTextures;
 	}
  	else // If we failed to open the image file in the first place...
  	{
		error = ilGetError();
		std::cout << "Image load failed - IL reports error: " << error << std::endl;
		exit(-1);
  	}

 	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.
}

void MapGUI::reloadImage()
{
	ILuint imageID;				// Create a image ID as a ULuint

	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the imag

	std::vector<unsigned char> data;

	Terrain* p_terrain(Terrain::getInstance());
	p_terrain->getMapColor(data);



	ilTexImage(p_terrain->getWidth() * CHUNKLENGTH * MAP_PIXEL_SIZE, p_terrain->getLength() * CHUNKLENGTH * MAP_PIXEL_SIZE, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, &data[0]);

	Texture::chooseTexture(m_texturesIndex[0]);

	Texture::loadFromCurrentDevilImage();

	ilDeleteImages(1, &imageID);
}

void MapGUI::updateImage()
{
	Texture::chooseTexture(m_texturesIndex[0]);
	Texture::bindTexture();
	Terrain* p_terrain(Terrain::getInstance());
	unsigned char color[3];

	for(std::hash_set<PixelPosition>::iterator it(m_pixelToUpdate.begin()), pixelToUpdateEnd(m_pixelToUpdate.end()); it != pixelToUpdateEnd; ++it)
	{
		p_terrain->getMapOverviewSingleColor(it->first, it->second, color);
		
		Texture::changePixelGroupColor(it->first * MAP_PIXEL_SIZE, it->second * MAP_PIXEL_SIZE, color, MAP_PIXEL_SIZE, MAP_PIXEL_SIZE);
	}

	//Ajout de la couleur des unitées qui ont bougée
	UnitHandler* p_unitHandler(UnitHandler::getInstance());
	Unit* p_unit;
	Vector3I cubePosition;

	for(int i(0), teamCount(p_unitHandler->getTeamCount()); i < teamCount; ++i)
	{
		color[0] = 255;
		color[1] = 0;
		color[2] = 0;
		for(int j(0), unitCount(p_unitHandler->getUnitCount(i)); j < unitCount; ++j)
		{
			p_unit = p_unitHandler->getUnit(i,j);
			cubePosition = Terrain::positionToCube(p_unit->getPosition());

			std::vector<std::vector<PixelPosition>> mapShape;
			p_unit->getMapFullShape(mapShape);

			
			for(int i(0), nbLine(mapShape.size()); i < nbLine; ++i)
			{
				Texture::changePixelGroupColor(mapShape[i][0].first, mapShape[i][0].second, color, mapShape[i].size(), 1);
			}

			color[0] = 255;
			color[1] = 255;
			color[2] = 255;
			std::vector<PixelPosition> mapDirection;
			p_unit->getMapDirection(mapDirection);

			for(int i(0), nbPixel(mapDirection.size()); i < nbPixel; ++i)
			{
				Texture::changePixelGroupColor(mapDirection[i].first, mapDirection[i].second, color, 1, 1);
			}
			
		}
	}

	{
		std::hash_set<PixelPosition> emptyPixelToUpdate;
		m_pixelToUpdate.swap(emptyPixelToUpdate);
	}

	GUIHandler::getInstance()->updateOpenGLImage(m_texturesIndex[0], MAP_LAYOUT, "map/map_img", m_scaling, m_offsetX, m_offsetY);
}

std::vector<int> MapGUI::getColorAverage(int i_cubeType)
{
	return m_colorAverage[abs(i_cubeType)]; 
}

void MapGUI::addPixelToUpdate(int i_x, int i_z)
{
	if(!Terrain::getInstance()->isInLimit(i_x, 0, i_z))
		return;
	m_pixelToUpdate.insert(PixelPosition(i_x, i_z));
}

bool MapGUI::mouseEnterMap(const CEGUI::EventArgs& i_event)
{
	m_mouseInside = true;
    return true;
}

bool MapGUI::mouseExitMap(const CEGUI::EventArgs& i_event)
{
	m_mouseInside = false;
    return true;
}

bool MapGUI::mouseRelease(const CEGUI::EventArgs& i_event)
{
	const CEGUI::MouseEventArgs* mouseArgs = static_cast<const CEGUI::MouseEventArgs*>(&i_event);

	//Trouver la position en pixel du clique
	CEGUI::Vector2f  relativeMousePosition(CEGUI::CoordConverter::screenToWindow(*m_layout->getChild("map/map_img"), mouseArgs->position));

	std::pair<float, float> imageSize(getAbsoluteSize("map/map_img"));

	//Convertir la position en pixel en cube target
	Terrain* p_terrain(Terrain::getInstance());
	Vector3I target((p_terrain->getWidth() * p_terrain->getChunkLength() * relativeMousePosition.d_x) / imageSize.first, 0, (p_terrain->getLength() * p_terrain->getChunkLength() * relativeMousePosition.d_y) / imageSize.second);

	target.y = Terrain::getInstance()->getTopWalkable(target.x, target.z);

	//Effectuer le deplacement
	Camera* p_camera(SceneOpenGL::getInstance()->getCamera());
	
	if(p_camera->getCameraType() == RTS_CAMERA)
	{
		Vector sourceVect(Terrain::cubeToPosition(target));
		sourceVect.y = p_terrain->getHeight() * p_terrain->getChunkLength() * SIDE;
		((CameraRTS*)p_camera)->unitMoving(target, TemplateManager::getInstance()->templateInCollision(sourceVect, Vector(sourceVect.x, 0, sourceVect.z)));
	}

    return true;
}