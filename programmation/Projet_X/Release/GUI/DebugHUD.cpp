#include "DebugHUD.h"

#include "../Camera/Camera.h"

DebugHUD::DebugHUD()
{
	m_needMouse = false;
	m_layout = CEGUI::WindowManager::getSingleton().loadLayoutFromFile( "DebugHUD.layout", "layouts");
	setVisible(true);
}
DebugHUD::~DebugHUD()
{

}

void DebugHUD::updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera)
{

}

void DebugHUD::registerHandlers()
{

}

void DebugHUD::printBuffer()
{
	m_layout->getChild("information")->setText(m_buffer.str());
	std::string test(m_buffer.str());
	m_buffer.str(std::string());


}