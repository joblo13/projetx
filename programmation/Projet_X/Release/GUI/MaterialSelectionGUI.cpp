#include "MaterialSelectionGUI.h"

#include "../Terrain/Terrain.h"
#include "../Entities/UnitHandler.h"
#include "GUIHandler.h"
#include "../Camera/Camera.h"
#include "../Rendering/Texture.h"

MaterialSelectionGUI::MaterialSelectionGUI()
:m_currentSelectedCubeTypeID(0)
{
	m_needMouse = true;
	m_layout = CEGUI::WindowManager::getSingleton().loadLayoutFromFile( "MaterialSelection.layout", "layouts");
	Texture::chooseTexture(Texture::getTextureID("TextureSampler", "Texture/terrain.png"));
	m_imageCount = Texture::getImageCount();
	Texture::createImageSet();

	init();
	setVisible(false);
}

MaterialSelectionGUI::~MaterialSelectionGUI()
{

}

void MaterialSelectionGUI::updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera)
{
	
}

void MaterialSelectionGUI::registerHandlers()
{
}

int extractNumber(const std::string& i_text)
{
	std::string result("");
	for(int i(0), textSize(i_text.length()); i < textSize; ++i)
		if(48 <= i_text[i] && i_text[i] <= 57 || i_text[i] == 45)
			result += i_text[i];
	return atoi(result.c_str());
}

bool MaterialSelectionGUI::selectionChanged(const CEGUI::EventArgs& i_event)
{
	m_currentSelectedCubeTypeID = extractNumber((static_cast<const CEGUI::WindowEventArgs*> (&i_event))->window->getName().c_str());

	GUIHandler::getInstance()->setVisibility(MATERIAL_SELECTION_LAYOUT, false);

	return true;
}

void MaterialSelectionGUI::init()
{
	//On commence à 1 puisque 0 est le cube d'air
	for(int i(0); i < m_imageCount; ++i)
	{
		char buffer[20];
		sprintf(buffer, "icon_but%i", i);
		std::string name(buffer);
		sprintf(buffer, "terrain/icon%i", i);
		std::string imageName(buffer);

		CEGUI::Window* button = createButton("materialSelection/textureList/textureScrollablePane", name, imageName);
		button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MaterialSelectionGUI::selectionChanged,this));
		moveButton("materialSelection/textureList/textureScrollablePane", name, i);
	}
}