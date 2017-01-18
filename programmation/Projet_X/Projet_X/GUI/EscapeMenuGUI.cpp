#include "EscapeMenuGUI.h"

#include "../Terrain/Terrain.h"
#include "../Entities/UnitHandler.h"
#include "GUIHandler.h"
#include "../Camera/Camera.h"
#include "../SceneOpenGL.h"
#include "../Input.h"

EscapeMenuGUI::EscapeMenuGUI()
{
	m_onTop = true;
	m_needMouse = true;
	m_layout = CEGUI::WindowManager::getSingleton().loadLayoutFromFile( "EscapeMenu.layout", "layouts");
	setVisible(false);
}

EscapeMenuGUI::~EscapeMenuGUI()
{

}

void EscapeMenuGUI::updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera)
{

}

void EscapeMenuGUI::registerHandlers()
{
	m_layout->getChild("Menu/Resume_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
						CEGUI::Event::Subscriber(&EscapeMenuGUI::resumeHandler, this));

	m_layout->getChild("Menu/Option_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
						CEGUI::Event::Subscriber(&EscapeMenuGUI::optionHandler, this));

	m_layout->getChild("Menu/MainMenu_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
						CEGUI::Event::Subscriber(&EscapeMenuGUI::mainMenuHandler, this));

	m_layout->getChild("Menu/Exit_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
						CEGUI::Event::Subscriber(&EscapeMenuGUI::exitHandler, this));
}

bool EscapeMenuGUI::resumeHandler(const CEGUI::EventArgs& i_event)
{
	setVisible(false);
	return true;
}
bool EscapeMenuGUI::optionHandler(const CEGUI::EventArgs& i_event)
{
	return true;
}
bool EscapeMenuGUI::mainMenuHandler(const CEGUI::EventArgs& i_event)
{
	return true;
}
bool EscapeMenuGUI::exitHandler(const CEGUI::EventArgs& i_event)
{
	SceneOpenGL::getInstance()->closeScene();
	return true;
}