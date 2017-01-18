#include "UpgradeAction.h"
#include "../../Camera/CameraRTS.h"
#include "../../GUI/RTSGUI.h"
#include "../../SceneOpenGL.h"
#include "../../Input.h"

UpgradeAction::UpgradeAction(RTSGUI* ip_GUIPointer, const std::string& i_name)
:RTSAction(ip_GUIPointer, i_name)
{
	m_shortcuts.push_back(std::vector<SDLKey>(Input::getInstance()->getSDLKey("UPGRADE_TEMPLATE")));
}

UpgradeAction::~UpgradeAction()
{

}

void UpgradeAction::doAction(const std::string& i_param)
{
	SceneOpenGL::getInstance()->getCamera()->upgradeTemplates();
}

void UpgradeAction::createButton()
{
	mp_buttons.push_back(mp_GUIPointer->createRTSButton("template/templateList_text", m_name, "RTS/templateUpgradeBut"));
}