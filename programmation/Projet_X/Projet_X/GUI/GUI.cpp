#include "GUI.h"

#include "../SceneOpenGL.h"

GUI::GUI()
:m_isVisible(true), m_needMouse(false), m_onTop(true)
{

}

GUI::~GUI()
{

}

void GUI::setVisible(bool i_isVisible)
{
	if(m_isVisible != i_isVisible)
	{
		m_isVisible = i_isVisible;
		m_layout->setVisible(m_isVisible);

		if(i_isVisible && m_onTop)
			m_layout->moveToFront();

	}
}
void GUI::activate()
{
	if(m_onTop)
		m_layout->activate();
}

std::string GUI::convertToString(unsigned int i_value)
{
	char a_converter[32];
	sprintf(a_converter,"%i",i_value);
	return a_converter;
}

std::pair<float, float> GUI::getAbsoluteSize(const std::string& i_path)const
{
	CEGUI::USize elementSize(m_layout->getChild(i_path)->getSize());
	std::pair<float, float> result;
	//On ajoute le offset
	result.first = elementSize.d_width.d_offset;
	result.second = elementSize.d_height.d_offset;

	//On ajoute le scale
	if(elementSize.d_width.d_scale != 0.f || elementSize.d_height.d_scale != 0.f)
	{
		size_t lastElementStartingPos(i_path.rfind("/"));
		if(lastElementStartingPos == std::string::npos)
		{
			//La grandeur de la fenêtre total
			SceneOpenGL* p_sceneOpenGL(SceneOpenGL::getInstance());
			result.first += elementSize.d_width.d_scale * p_sceneOpenGL->getWindowWidth();
			result.second += elementSize.d_height.d_scale * p_sceneOpenGL->getWindowHeight();
		}
		else
		{
			//La grandeur du parent
			std::pair<float, float> parentResult(getAbsoluteSize(i_path.substr(0, lastElementStartingPos)));
			result.first += elementSize.d_width.d_scale * parentResult.first;
			result.second += elementSize.d_height.d_scale * parentResult.second;
		}

	}
	return result;
}

void GUI::moveButton(const std::string& i_containerName, const std::string& i_componantName, int i_position)
{
	const float iconSize(37.f);
	const float padding(5.f);
	CEGUI::Window* container(m_layout->getChild(i_containerName));
	CEGUI::Window* button(container->getChild(i_componantName));

	std::pair<float, float> paneSize(getAbsoluteSize(i_containerName));
	const int iconNumPerLine(paneSize.second / (iconSize + 2 * padding));

	button->setPosition(CEGUI::UVector2(CEGUI::UDim(0.f, padding + (i_position/ iconNumPerLine) * (iconSize + padding)), CEGUI::UDim(0.f, padding + (i_position % iconNumPerLine) * (iconSize + padding))));
}

CEGUI::Window* GUI::createButton(const std::string& i_container, const std::string& i_name, const std::string& i_imageName)
{
	const float iconSize(37.f);

	//Chargement de l'icon
	CEGUI::Window* button(m_layout->getChild(i_container)->createChild("TaharezLook/Button", i_name.c_str()));
	CEGUI::Window* imageButton(button->createChild("Generic/Image", "icon_img"));

	button->setSize(CEGUI::USize(CEGUI::UDim(0.f, iconSize), CEGUI::UDim( 0.f, iconSize)));
	button->setTooltipText(i_name);

	imageButton->setSize(CEGUI::USize(CEGUI::UDim(0.8f, 0.f), CEGUI::UDim( 0.8f, 0.f)));
	imageButton->setProperty("MouseInputPropagationEnabled", "true");
	imageButton->setProperty("MousePassThroughEnabled", "true");

	imageButton->setVerticalAlignment(CEGUI::VA_CENTRE);
	imageButton->setHorizontalAlignment(CEGUI::HA_CENTRE);

	imageButton->setProperty("Image", i_imageName);
	return button;
}