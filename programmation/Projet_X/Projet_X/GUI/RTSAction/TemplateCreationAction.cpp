#include "TemplateCreationAction.h"
#include "../../Camera/CameraRTS.h"
#include "../../GUI/GUI.h"
#include "../../GUI/RTSGUI.h"
#include "../../SceneOpenGL.h"
#include "../../Terrain/Terrain.h"
#include "../../Input.h"
#include "../../Serializer.h"

#include <boost/filesystem.hpp>

TemplateCreationAction::TemplateCreationAction(RTSGUI* ip_GUIPointer, const std::string& i_name)
:RTSAction(ip_GUIPointer, i_name)
{
	m_shortcuts.push_back(std::vector<SDLKey>(Input::getInstance()->getSDLKey("ZONE_SELECTION_TEMPLATE")));
}

TemplateCreationAction::~TemplateCreationAction()
{

}

void TemplateCreationAction::doAction(const std::string& i_param)
{
	((CameraRTS*)SceneOpenGL::getInstance()->getCamera())->switchToTemplateSelectionMode();
}

void TemplateCreationAction::createButton()
{
	mp_buttons.push_back(mp_GUIPointer->createRTSButton("template/templateList_text", m_name, "RTS/templateCreationBut"));
}