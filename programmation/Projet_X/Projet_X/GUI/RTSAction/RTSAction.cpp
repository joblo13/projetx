#include "RTSAction.h"

#include "../RTSGUI.h"

RTSAction::RTSAction(RTSGUI* ip_GUIPointer, const std::string& i_name)
:mp_GUIPointer(ip_GUIPointer), m_name(i_name)
{

}

RTSAction::~RTSAction()
{

}

void RTSAction::activateButton()
{
	int lastButtonPosition(mp_GUIPointer->getLastButtonPosition());
	for(int i(0), nbButton(mp_buttons.size()); i < nbButton; ++i)
	{
		mp_buttons[i]->setVisible(true);
		mp_GUIPointer->moveButton("template/templateList_text", mp_buttons[i]->getName().c_str(), lastButtonPosition++);
	}
	mp_GUIPointer->setLastButtonPosition(lastButtonPosition);
}

void RTSAction::desactivateButton()
{
	for(int i(0), nbButton(mp_buttons.size()); i < nbButton; ++i)
		mp_buttons[i]->setVisible(false);
}