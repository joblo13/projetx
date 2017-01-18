#include "TemplateEditorGUI.h"

#include "../Terrain/Terrain.h"
#include "GUIHandler.h"
#include "RTSGUI.h"
#include "../Terrain/Chunk.h"
#include "../Serializer.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/System.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include <CEGUI/DefaultResourceProvider.h>
#include <CEGUI/colour.h>
#include <CEGUI/SchemeManager.h>
#include <CEGUI/Image.h>
#include "../Input.h"

#define MAX_CHAT_ENTRIES 100
#define CHAR_PIXEL_LENGTH 7

TemplateEditorGUI::TemplateEditorGUI()
:m_choosingKeyState(IDLE_CHOOSING_KEY_STATE)
{
	m_onTop = true;
	m_needMouse = true;
	m_layout = CEGUI::WindowManager::getSingleton().loadLayoutFromFile( "TemplateEditor.layout", "layouts");
	setVisible(false);

	loadIcon();
}

TemplateEditorGUI::~TemplateEditorGUI()
{

}
void TemplateEditorGUI::updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera)
{
	if(m_choosingKeyState != IDLE_CHOOSING_KEY_STATE)
	{
		const std::set<SDLKey> keyList(Input::getInstance()->getKeyList());

		m_totalKeyList.insert(keyList.begin(), keyList.end());

		if(keyList.size() == 0)
		{
			if(m_choosingKeyState == SAVING_CHOOSING_KEY_STATE)
			{
				m_choosingKeyState = IDLE_CHOOSING_KEY_STATE;
				if(m_totalKeyList.size() > 1)
				{
					m_layout->getChild("TemplateEditor/IdentificationInformation/shortcut_but")->setText("Keys selected : ");
				}
				else
				{
					m_layout->getChild("TemplateEditor/IdentificationInformation/shortcut_but")->setText("Key selected : " + Input::keyCodeToString((*m_totalKeyList.begin())));
				}
			}

		}
		else
				m_choosingKeyState = SAVING_CHOOSING_KEY_STATE;

	}
}

void TemplateEditorGUI::registerHandlers()
{
	m_layout->getChild("TemplateEditor/save_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&TemplateEditorGUI::saveTemplate, this));
	m_layout->getChild("TemplateEditor/IdentificationInformation/icon_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&TemplateEditorGUI::openIconSelection, this));
	m_layout->getChild("TemplateEditor/IdentificationInformation/shortcut_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&TemplateEditorGUI::startSelectingShortcut, this));
}
bool TemplateEditorGUI::saveTemplate(const CEGUI::EventArgs& i_event)
{

	//((RTSGUI*)GUIHandler::getInstance()->getLayout(RTS_LAYOUT))->addTemplate("chunk.map");
	std::string templateName(m_layout->getChild("TemplateEditor/IdentificationInformation/name_textbox")->getText().c_str());
	std::string templateImageName(m_layout->getChild("TemplateEditor/IdentificationInformation/icon_but/icon_img")->getProperty("Image").c_str());
	if(templateName != "")
	{
		Terrain::getInstance()->saveChunk(*mp_selectorChunk, templateName + ".map");
		setVisible(false);

		saveTemplateDescription(templateName, templateImageName);
	}

	return true;
}

void TemplateEditorGUI::addSelectorChunk(Chunk* ip_selectorChunk)
{
	mp_selectorChunk = ip_selectorChunk;
}

void TemplateEditorGUI::saveTemplateDescription(const std::string& ik_templateName, const std::string& ik_imageName)
{
	//On ouvre le fichier
	std::ofstream ofstreamFile;
	ofstreamFile.open("Map/" + ik_templateName + ".tpl", std::ios::binary);

	//On créer les variables pour l'écriture
	short imageNameSize(ik_imageName.length());
	writeToFile(ofstreamFile, imageNameSize);
	writeToFile(ofstreamFile, ik_imageName);

	writeToFile(ofstreamFile, ((short)m_totalKeyList.size()));
	for(std::set<SDLKey>::iterator it(m_totalKeyList.begin()), endTotalKeyList(m_totalKeyList.end()); it != endTotalKeyList; ++it)
		writeToFile(ofstreamFile, *it);

	//On ferme le fichier
	ofstreamFile.close();

	//On ajoute le template
	((RTSGUI*)GUIHandler::getInstance()->getLayout(RTS_LAYOUT))->addTemplate(ik_templateName);
}

void TemplateEditorGUI::loadIcon()
{
	const float iconSize(37.f);
	const float padding(5.f);

	std::pair<float, float> paneSize(getAbsoluteSize("IconSelector/IconList/IconScrollablePane"));

	int iconNumPerLine(paneSize.second / (iconSize + 2 * padding));

	for(int i(0), iconNum(GUIHandler::getInstance()->getIconNum()); i < iconNum; ++i)
	{
		char buffer[20];
		sprintf(buffer, "icon_but%i", i);
		std::string name(buffer);
		sprintf(buffer, "ICON_RTS/icon%i", i);
		std::string imageName(buffer);

		createButton("IconSelector/IconList/IconScrollablePane", name, imageName)->subscribeEvent(CEGUI::PushButton::EventClicked,
		CEGUI::Event::Subscriber(&TemplateEditorGUI::chooseIcon,this));
	}
}

bool TemplateEditorGUI::chooseIcon(const CEGUI::EventArgs& i_event)
{
	m_selectedIcon = (static_cast<const CEGUI::WindowEventArgs*> (&i_event))->window->getChild("icon_img")->getProperty("Image").c_str();

	//icone par defaut?

	m_layout->getChild("TemplateEditor/IdentificationInformation/icon_but/icon_img")->setProperty("Image", m_selectedIcon);

	m_layout->getChild("IconSelector")->setVisible(false);
    return true;
}

bool TemplateEditorGUI::openIconSelection(const CEGUI::EventArgs& i_event)
{
	CEGUI::Window* p_window(m_layout->getChild("IconSelector"));
	p_window->setVisible(true);
	p_window->activate();
	return true;
}

bool TemplateEditorGUI::startSelectingShortcut(const CEGUI::EventArgs& i_event)
{
	m_layout->getChild("TemplateEditor/IdentificationInformation/shortcut_but")->setText("Press a key");
	m_choosingKeyState = RECORDING_CHOOSING_KEY_STATE;
	std::set<SDLKey> emptyKeyList;
	m_totalKeyList.swap(emptyKeyList);
	return true;
}
