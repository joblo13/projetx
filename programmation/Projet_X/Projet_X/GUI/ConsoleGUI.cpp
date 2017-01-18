#include "ConsoleGUI.h"

#include "../Terrain/Terrain.h"
#include "../Entities/UnitHandler.h"
#include "GUIHandler.h"
#include "../Camera/Camera.h"
#include "../Input.h"
#include "../Rendering/Mesh/Mesh.h"
#include "../GUI/RTSGUI.h"
#include "../SceneOpenGL.h"
#include "../Camera/CameraRTS.h"

#define MAX_CHAT_ENTRIES 100
#define CHAR_PIXEL_LENGTH 7

ConsoleGUI::ConsoleGUI()
:m_currentMessage(m_oldMessage.end())
{
	m_onTop = true;
	m_needMouse = true;
	m_layout = CEGUI::WindowManager::getSingleton().loadLayoutFromFile( "Console.layout", "layouts");
	setVisible(false);
}

ConsoleGUI::~ConsoleGUI()
{

}

void ConsoleGUI::updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera)
{
	Input* p_input(Input::getInstance());
	if(p_input->getKeyPress("up"))
	{
		if(m_currentMessage != m_oldMessage.begin())
			--m_currentMessage;
		m_layout->getChild("Console/textToSend_editbox")->setText(*m_currentMessage);
	}
	else if(p_input->getKeyPress("down"))
	{
		if(m_currentMessage != m_oldMessage.end())
			++m_currentMessage;
		m_layout->getChild("Console/textToSend_editbox")->setText(*m_currentMessage);
	}
}

void ConsoleGUI::registerHandlers()
{
	m_layout->getChild("Console/Send_but")->subscribeEvent(CEGUI::PushButton::EventClicked,
                        CEGUI::Event::Subscriber(&ConsoleGUI::textSubmittedHandler, this));

    m_layout->getChild("Console/textToSend_editbox")->subscribeEvent(CEGUI::Editbox::EventTextAccepted,
                        CEGUI::Event::Subscriber(&ConsoleGUI::textSubmittedHandler,this));
}

bool ConsoleGUI::textSubmittedHandler(const CEGUI::EventArgs& i_event)
{
    CEGUI::String message = m_layout->getChild("Console/textToSend_editbox")->getText();
	m_oldMessage.push_back(message);
	m_currentMessage = m_oldMessage.end();
	parseText(message);
    m_layout->getChild("Console/textToSend_editbox")->setText("");
 
    return true;
}

void ConsoleGUI::parseText(CEGUI::String i_message)
{
    std::string message(i_message.c_str());
 
	if (message.length() >= 1)
	{
		if (message.at(0) == '-')
		{
			std::string::size_type spacePosition(message.find(" ", 1));
			std::string command(message.substr(1, spacePosition - 1));
			std::vector<std::string> arguments;

			std::string::size_type oldSpacePosition;
			while(message.size() > spacePosition)
			{
				oldSpacePosition = spacePosition;
				spacePosition = message.find(" ", spacePosition + 1);
				if(spacePosition - oldSpacePosition - 1 != 0)
					arguments.push_back(message.substr(oldSpacePosition + 1, spacePosition - oldSpacePosition - 1));

			}

			for(std::string::size_type i(0); i < command.length(); ++i)
			{
				command[i] = tolower(command[i]);
			}

			if(command == "save")
			{
				save(arguments);
			}
			else if(command == "load")
			{
				load(arguments);
			}
			else if(command == "res")
			{
				giveRessources(arguments);
			}
			else if(command == "debug")
			{
				GUIHandler::getInstance()->toggleVisibility(DEBUG_HUD_LAYOUT);
			}
			else if(command == "changemesh")
			{
				changeMesh(arguments);
			}
			else if(command == "savemesh")
			{
				saveMesh(arguments);
			}
			else if(command == "getmesh")
			{
				getMeshes();
			}
			else if(command == "getcube")
			{
				getCube(arguments);
			}
			else if(command == "updatecube")
			{
				updateCube(arguments);
			}
			else if(command == "reloadgui")
			{
				reloadGUI(arguments);
			}
			else
			{
				outputText("Invalid command", CEGUI::Colour(1.0f,0.0f,0.0f));
			}

		}
		else
		{
			outputText(message);
		}
	} 
}

void ConsoleGUI::outputText(CEGUI::String i_message, CEGUI::Colour i_colour)
{
	CEGUI::Listbox *outputWindow = static_cast<CEGUI::Listbox*>(m_layout->getChild("Console/resultText_listbox"));
 
	CEGUI::ListboxTextItem *newItem=0;
 
	CEGUI::USize listBoxSize(outputWindow->getSize());
	CEGUI::USize windowSize(m_layout->getChild("Console")->getSize());
	int charPerItem((listBoxSize.d_width.d_scale * windowSize.d_width.d_offset)/CHAR_PIXEL_LENGTH);

	int currentPos(0);
	while(i_message.size() > currentPos)
	{
		newItem = new CEGUI::ListboxTextItem(i_message.substr(currentPos, charPerItem), CEGUI::HTF_WORDWRAP_LEFT_ALIGNED);
		currentPos += charPerItem;
		newItem->setTextColours(i_colour);
		outputWindow->addItem(newItem);
	}

    while(outputWindow->getItemCount() > MAX_CHAT_ENTRIES) 
	{
        outputWindow->removeItem(outputWindow->getListboxItemFromIndex(0));
    }
	//On s'assure que l'item ajouté est visible (scroll)
	outputWindow->ensureItemIsVisible(newItem);
}

bool isAlphanumeric(const std::string& i_message)
{
	for(int i(0), messageSize(i_message.size()); i < messageSize; ++i)
		if(i_message[i] < 32 || (i_message[i] > 32 && i_message[i] < 48) || (i_message[i] > 57 && i_message[i] < 65) || 
			(i_message[i] > 90 && i_message[i] < 97) || i_message[i] > 122)
			return false;
	return true;
}

void ConsoleGUI::save(const std::vector<std::string>& i_arguments)
{
	if(i_arguments.size() != 1)
	{
		outputText("Invalid number of arguments", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}
	if(!isAlphanumeric(i_arguments[0]))
	{
		outputText("Invalid character in the file name", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}
	Terrain::getInstance()->saveMap(i_arguments[0] + ".map");
	outputText("Map saved.", CEGUI::Colour(0.0f,1.0f,0.0f));
}

void ConsoleGUI::load(const std::vector<std::string>& i_arguments)
{
	if(i_arguments.size() != 1)
	{
		outputText("Invalid number of arguments", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}
	if(!isAlphanumeric(i_arguments[0]))
	{
		outputText("Invalid character in the file name", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}
	Terrain::getInstance()->loadMap(i_arguments[0] + ".map");
	outputText("Map loaded.", CEGUI::Colour(0.0f,1.0f,0.0f));
}

void ConsoleGUI::giveRessources(const std::vector<std::string>& i_arguments)
{
	if(i_arguments.size() != 0)
	{
		outputText("Invalid number of arguments", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}

	UnitHandler::getInstance()->addRessources(0, HUMAN_CUBES, 1000);
}

void ConsoleGUI::changeMesh(const std::vector<std::string>& i_arguments)
{
	int nbArguments(i_arguments.size());

	if(nbArguments == 0 || nbArguments % 2 != 1)
	{
		outputText("Invalid number of parameter", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}

	Mesh* p_mesh(Mesh::getLoadedMesh("../../../graphique/mesh/" + i_arguments[0]));
	if(p_mesh == 0)
	{
		outputText("The mesh don't exist", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}

	for(int i(1) ; i < nbArguments; i += 2)
	{
		if(i_arguments[i] == "cullface")
			p_mesh->setCustom(CULLFACE, atoi(i_arguments[i+1].c_str()));
		else
		{
			outputText(i_arguments[i] + " is not a valid parameter", CEGUI::Colour(1.0f,0.0f,0.0f));
		}
	}
}
void ConsoleGUI::saveMesh(const std::vector<std::string>& i_arguments)
{
	if(i_arguments.size() != 1)
	{
		outputText("Invalid number of parameter", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}

	Mesh* p_mesh(Mesh::getLoadedMesh("../../../graphique/mesh/" + i_arguments[0]));
	if(p_mesh == 0)
	{
		outputText("The mesh don't exist", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}

		p_mesh->saveInfoToFile();
}

void ConsoleGUI::getMeshes()
{
	std::vector<std::string> list;
	Mesh::getMeshesList(list);
	for(int i(0), listSize(list.size()); i < listSize; ++i)
		outputText(list[i], CEGUI::Colour(0.0f,1.0f,0.0f));
}

void ConsoleGUI::getCube(const std::vector<std::string>& i_arguments)
{
	static const std::string k_paramName[] = {"CUBE_TYPE", "CUBE_MAX_HITPOINT", "CUBE_HITPOINT", "CUBE_COLOR", 
		"CUBE_HIGH_0F", "CUBE_HIGH_1F", "CUBE_HIGH_2F", "CUBE_HIGH_3F", "CUBE_HIGH_4F", "CUBE_HIGH_5F", "CUBE_HIGH_6F", "CUBE_HIGH_7F",
		"CUBE_DRAWING_INDEX", "CUBE_NB_VERTEX", "CUBE_CHANGED",
		"NB_CUBE_PARAM"};

	if(i_arguments.size() != 3)
	{
		outputText("Invalid number of arguments", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}
	if(!isAlphanumeric(i_arguments[0]))
	{
		outputText("Invalid character", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}
	outputText("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", CEGUI::Colour(0.0f,1.0f,0.0f));
	int x(atoi(i_arguments[0].c_str())); 
	int y(atoi(i_arguments[1].c_str())); 
	int z( atoi(i_arguments[2].c_str()));
	Terrain* p_terrain(Terrain::getInstance());
	const std::vector<int>* cubeInfo = p_terrain->getFullCube(x, y, z);
	for(int i(0), cubeInfoSize(cubeInfo->size()); i < cubeInfoSize; ++i)
	{
		outputText(k_paramName[i], CEGUI::Colour(0.0f,1.0f,0.0f));
		if(i >= 4 && i <= 11)
		{

			float cubeInfoFloat;
			memcpy(&cubeInfoFloat, &(*cubeInfo)[i], sizeof((*cubeInfo)[i]));
			char strFloatConverter[32];
			sprintf(strFloatConverter,"%f",cubeInfoFloat);
			
			outputText(strFloatConverter, CEGUI::Colour(0.0f,1.0f,0.0f));
		}
		else
		{
			char strIntConverter[32];
			sprintf(strIntConverter, "%d", (*cubeInfo)[i]);
			outputText(strIntConverter, CEGUI::Colour(0.0f,1.0f,0.0f));
		}
	}
	outputText(p_terrain->isNatural(x, y, z)?"natural":"not_natural", CEGUI::Colour(0.0f,1.0f,0.0f));
	outputText(p_terrain->isSmoothTopRenderable(x, y, z)?"top_renderable":"top_not_renderable", CEGUI::Colour(0.0f,1.0f,0.0f));
	outputText(p_terrain->isFull(x, y, z)?"full":"not_full", CEGUI::Colour(0.0f,1.0f,0.0f));

	outputText(p_terrain->isHigherThen(x-1, y, z, LEFT_FACE)?"TRUE_LEFT_FACE":"FALSE_LEFT_FACE", CEGUI::Colour(0.0f,1.0f,0.0f));
	outputText(p_terrain->isHigherThen(x+1, y, z, RIGHT_FACE)?"TRUE_RIGHT_FACE":"FALSE_RIGHT_FACE", CEGUI::Colour(0.0f,1.0f,0.0f));
	outputText(p_terrain->isHigherThen(x, y, z+1, FRONT_FACE)?"TRUE_FRONT_FACE":"FALSE_FRONT_FACE", CEGUI::Colour(0.0f,1.0f,0.0f));
	outputText(p_terrain->isHigherThen(x, y, z-1, BACK_FACE)?"TRUE_BACK_FACE":"FALSE_BACK_FACE", CEGUI::Colour(0.0f,1.0f,0.0f));
	
	char strIntConverter[32];
	sprintf(strIntConverter, "%f", p_terrain->getIsoPosition(x, y, z, CUBE_HIGH_0F));
	outputText(strIntConverter, CEGUI::Colour(0.0f,1.0f,0.0f));
	sprintf(strIntConverter, "%f", p_terrain->getIsoPosition(x, y, z, CUBE_HIGH_1F));
	outputText(strIntConverter, CEGUI::Colour(0.0f,1.0f,0.0f));
	sprintf(strIntConverter, "%f", p_terrain->getIsoPosition(x, y, z, CUBE_HIGH_2F));
	outputText(strIntConverter, CEGUI::Colour(0.0f,1.0f,0.0f));
	sprintf(strIntConverter, "%f", p_terrain->getIsoPosition(x, y, z, CUBE_HIGH_3F));
	outputText(strIntConverter, CEGUI::Colour(0.0f,1.0f,0.0f));


	outputText("\n", CEGUI::Colour(0.0f,1.0f,0.0f));
}

void ConsoleGUI::updateCube(const std::vector<std::string>& i_arguments)
{
	static const std::string k_paramName[] = {"CUBE_TYPE", "CUBE_MAX_HITPOINT", "CUBE_HITPOINT", "CUBE_COLOR", 
		"CUBE_HIGH_0F", "CUBE_HIGH_1F", "CUBE_HIGH_2F", "CUBE_HIGH_3F", "CUBE_HIGH_4F", "CUBE_HIGH_5F", "CUBE_HIGH_6F", "CUBE_HIGH_7F",
		"CUBE_DRAWING_INDEX", "CUBE_NB_VERTEX", "CUBE_CHANGED",
		"NB_CUBE_PARAM"};

	if(i_arguments.size() != 3)
	{
		outputText("Invalid number of arguments", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}
	if(!isAlphanumeric(i_arguments[0]))
	{
		outputText("Invalid character", CEGUI::Colour(1.0f,0.0f,0.0f));
		return;
	}
	outputText("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@", CEGUI::Colour(0.0f,1.0f,0.0f));
	int x(atoi(i_arguments[0].c_str())); 
	int y(atoi(i_arguments[1].c_str())); 
	int z( atoi(i_arguments[2].c_str()));
	Terrain* p_terrain(Terrain::getInstance());
	p_terrain->setChangedFlag(x, y, z, true);


	outputText("Cube updated", CEGUI::Colour(0.0f,1.0f,0.0f));
}

void ConsoleGUI::reloadGUI(const std::vector<std::string>& i_arguments)
{
	GUIHandler::getInstance()->setNeedToReload(true);
}