#include "FillAction.h"

#include "../../Camera/CameraRTS.h"
#include "../../GUI/RTSGUI.h"
#include "../../SceneOpenGL.h"
#include "../../Input.h"

FillAction::FillAction(RTSGUI* ip_GUIPointer, const std::string& i_name)
:RTSAction(ip_GUIPointer, i_name)
{
	m_shortcuts.push_back(std::vector<SDLKey>(Input::getInstance()->getSDLKey("ZONE_SELECTION_FILL")));
}

FillAction::~FillAction()
{

}

void FillAction::doAction(const std::string& i_param)
{
	((CameraRTS*)SceneOpenGL::getInstance()->getCamera())->switchToFillSelectionMode();
}

void FillAction::createButton()
{
	mp_buttons.push_back(mp_GUIPointer->createRTSButton("template/templateList_text", m_name, "RTS/templateCreationBut"));
}