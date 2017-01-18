#include "ChooseTemplateAction.h"
#include "../../Camera/CameraRTS.h"
#include "../../GUI/GUI.h"
#include "../../GUI/RTSGUI.h"
#include "../../SceneOpenGL.h"
#include "../../Terrain/Terrain.h"
#include "../../Input.h"
#include "../../Serializer.h"

#include <boost/filesystem.hpp>

ChooseTemplateAction::ChooseTemplateAction(RTSGUI* ip_GUIPointer, const std::string& i_name)
:RTSAction(ip_GUIPointer, i_name)
{

}

ChooseTemplateAction::~ChooseTemplateAction()
{

}

void ChooseTemplateAction::doAction(const std::string& i_param)
{
	((CameraRTS*)SceneOpenGL::getInstance()->getCamera())->selectCurrentTemplateShape(m_templateIDs[i_param]);
}

void ChooseTemplateAction::createButton()
{
	loadAllTemplate();
}

void ChooseTemplateAction::loadAllTemplate()
{
	boost::filesystem::path i_dirPath(boost::filesystem::current_path());
	i_dirPath /= "Map\\";

	if (!boost::filesystem::exists(i_dirPath)) 
		return;

	for ( boost::filesystem::directory_iterator itr( i_dirPath ), end_itr; itr != end_itr; ++itr )
	{
		if (!boost::filesystem::is_directory(itr->status()))
		{
			std::string templateFilePath(itr->path().string());
			int extensionPos(templateFilePath.find(".tpl"));
			if(std::string::npos != extensionPos)
			{
				int namePos(templateFilePath.rfind("\\") + 1);
				std::string fileName(templateFilePath.substr(namePos, templateFilePath.length() - namePos - 4));
				addTemplate(fileName);
			}
		}
	}
}

void ChooseTemplateAction::addTemplate(const std::string& i_fileName)
{
	//Chargement du template
	m_templateList.push_back(Terrain::getInstance()->loadChunk(Vector3I(0,0,0), i_fileName + ".map"));
	m_shortcuts.push_back(std::vector<SDLKey>());


	m_templateIDs[i_fileName] = m_templateIDs.size();


	std::string imageName; 
	getTemplateDetails(i_fileName, imageName, m_shortcuts[m_shortcuts.size() - 1]);

	mp_buttons.push_back(mp_GUIPointer->createRTSButton("template/templateList_text", m_name + "|" + i_fileName, imageName));
}

void ChooseTemplateAction::getTemplateDetails(const std::string& i_fileName, std::string& o_imageName, std::vector<SDLKey>& o_shortcut)
{
	
	std::ifstream ifstreamFile;
	ifstreamFile.open("map/" + i_fileName + ".tpl", std::ios::binary);
		
	//On lit les données d'entêtes
	short templateImageNameSize;
	readFromFile(ifstreamFile, templateImageNameSize);
	o_imageName.resize(templateImageNameSize);
	readFromFile(ifstreamFile, o_imageName);

	short shortcutNumber;
	readFromFile(ifstreamFile, shortcutNumber);
	SDLKey tempSdlKey;
	Input* p_input(Input::getInstance());
	for(short i(0); i < shortcutNumber; ++i)
	{
		readFromFile(ifstreamFile, tempSdlKey);
		o_shortcut.push_back(tempSdlKey);
		p_input->addKey(tempSdlKey);
	}

	ifstreamFile.close();
}

Chunk* ChooseTemplateAction::getTemplateShape(int i_index)
{
	return m_templateList[i_index];
}