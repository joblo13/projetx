#include "MainMenuGUI.h"

#include "GUIHandler.h"
#include "../Input.h"
#include "../SceneOpenGL.h" 

MainMenuGUI::MainMenuGUI()
{
	m_onTop = true;
	m_needMouse = true;
	m_layout = CEGUI::WindowManager::getSingleton().loadLayoutFromFile( "MainMenu.layout", "layouts");
	setVisible(true);
}

MainMenuGUI::~MainMenuGUI()
{

}

void MainMenuGUI::updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera)
{

}

void MainMenuGUI::registerHandlers()
{
	m_layout->getChild("start_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&MainMenuGUI::startButHandler, this));
}

bool MainMenuGUI::startButHandler(const CEGUI::EventArgs& i_event)
{
	setVisible(false);
	SceneOpenGL::getInstance()->setInMainMenu(false);
	return true;
}