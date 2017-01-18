#include "RTSGUI.h"
#include "../Entities/UnitHandler.h"
#include "../Entities/Unit.h"
#include "../Input.h"
#include "../Camera/Camera.h"
#include "../Terrain/Chunk.h"
#include "../Terrain/Terrain.h"
#include "../Rendering/DeferredRenderer/DeferredRenderer.h"
#include "../Serializer.h"
#include "../SceneOpenGL.h"
#include "../Camera/CameraRTS.h"
#include "RTSAction/RTSAction.h"
#include "RTSAction/ChooseTemplateAction.h"
#include "RTSAction/TemplateCreationAction.h"
#include "RTSAction/FillAction.h"
#include "RTSAction/UpgradeAction.h"
#include "GUIHandler.h"

#include <boost/filesystem.hpp>

RTSGUI::RTSGUI()
:m_lastButtonPosition(0)
{
	m_needMouse = true;
	m_layout = CEGUI::WindowManager::getSingleton().loadLayoutFromFile( "RTS.layout", "layouts");
	setVisible(false);

	generateButton("template");
	generateButton("Zone filler");
	generateButton("Template creation");
	generateButton("Upgrade Action");

	chooseButtons();
}

RTSGUI::~RTSGUI()
{
	for(std::map<std::string, RTSAction*>::iterator it(m_RTSActionList.begin()), endActionList(m_RTSActionList.end()); it != endActionList; ++it)
	{
			delete it->second;
	}
}

void RTSGUI::generateButton(const std::string& i_name)
{
	if(i_name == "template")
		m_RTSActionList[i_name] = new ChooseTemplateAction(this, i_name);
	else if(i_name == "Zone filler")
		m_RTSActionList[i_name] = new FillAction(this, i_name);
	else if(i_name == "Template creation")
		m_RTSActionList[i_name] = new TemplateCreationAction(this, i_name);
	else if(i_name == "Upgrade Action")
		m_RTSActionList[i_name] = new UpgradeAction(this, i_name);
	else
		assert(false);

	m_RTSActionList[i_name]->createButton();
	m_RTSActionList[i_name]->desactivateButton();
}

void RTSGUI::updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ikp_camera)
{
	//Affichage des ressources
	UnitHandler* p_unitHandler(UnitHandler::getInstance());
	m_layout->getChild("ressources/res_cube")->setText(convertToString(p_unitHandler->getRessources(i_teamID, HUMAN_CUBES)));
	m_layout->getChild("ressources/res_weaponPart")->setText(convertToString(p_unitHandler->getRessources(i_teamID, HUMAN_GUN_PARTS)));
	m_layout->getChild("ressources/res_vehiculePart")->setText(convertToString(p_unitHandler->getRessources(i_teamID, HUMAN_VEHICULE_PARTS)));
	m_layout->getChild("ressources/res_advancedPart")->setText(convertToString(p_unitHandler->getRessources(i_teamID, HUMAN_ADVANCED_PARTS)));
	m_layout->getChild("ressources/res_advancedPart")->setText(convertToString(p_unitHandler->getRessources(i_teamID, HUMAN_ADVANCED_PARTS)));
	m_layout->getChild("ressources/res_population")->setText(convertToString(p_unitHandler->getUnitCount(i_teamID)) + "/200");

	//Affichage du compas
	m_layout->getChild("compas/compas_img")->setPosition(CEGUI::UVector2(CEGUI::UDim(0, -150 - 480 * ikp_camera->getTheta()/360.0f), CEGUI::UDim(0, 0)));

	//Information
	std::hash_set<Unit*> selectedUnitList(ikp_camera->getSelectedUnit());

	int selectedUnitListSize(selectedUnitList.size());

	if(selectedUnitListSize == 1)
	{
		std::stringstream information;

		information << "Team: ";
		information << (*selectedUnitList.begin())->m_teamID;
		information << "\nHitPoint: ";
		information << (*selectedUnitList.begin())->getHitpoint();
		information << "\nSpeed: ";
		information << (*selectedUnitList.begin())->m_speed;
		information << "\nAmmo: ";
		information << (*selectedUnitList.begin())->getCurrentAmmo();
		information << "/";
		information << (*selectedUnitList.begin())->getCurrentBackupAmmo();

		m_layout->getChild("information/information_text")->setText(information.str());
	}
	else
	{
		m_layout->getChild("information/information_text")->setText("");
	}
	bool isActive(true);
	Input* p_input(Input::getInstance());
	

	for(int i(0), nbActions(currentActions.size()); i < nbActions; ++i)
	{
		const std::vector<std::vector<SDLKey>>* shortcuts(m_RTSActionList[currentActions[i]]->getShortcuts());
		if(shortcuts != 0)
		{
			for(int j(0), nbShortcuts((*shortcuts).size()); j < nbShortcuts; ++j)
			{
				if(shortcuts[j].size() > 0)
				{
					for(int k(0), nbKey(shortcuts[j].size()); k < nbKey; ++k)
						if(!p_input->getSDLKeystate((*shortcuts)[j][k]))
						{
							isActive = false;
							break;
						}

					if(isActive)
						m_RTSActionList[currentActions[i]]->doAction("");
				}
				isActive = true;
			}
		}
	}

	if(ikp_camera->isSelectorStateChanged())
	{
		chooseButtons();
	}
}

void RTSGUI::chooseButtons()
{
		clearCurrentAction();
		Camera* p_camera(SceneOpenGL::getInstance()->getCamera());
		SelectorState selectorState = NONE_UNIT_SELECTOR_STATE;
		if(p_camera != 0)
			selectorState = p_camera->getSelectorState();

		switch(selectorState)
		{
			case NONE_UNIT_SELECTOR_STATE:
			case UNIT_SELECTOR_STATE:
				addCurrentAction("template");
				addCurrentAction("Zone filler");
				addCurrentAction("Template creation");
				//addCurrentAction("Upgrade Action");
				break;
			case BULDING_SELECTOR_STATE:
				addCurrentAction("Upgrade Action");
				break;
			default:
				assert(false);
		}
}

Chunk* RTSGUI::getTemplateShape(int i_index)
{
	return ((ChooseTemplateAction*)m_RTSActionList["template"])->getTemplateShape(i_index);
}

void RTSGUI::addTemplate(const std::string& i_fileName)
{
	((ChooseTemplateAction*)m_RTSActionList["template"])->addTemplate(i_fileName);
}

bool RTSGUI::doAction(const CEGUI::EventArgs& i_event)
{
	std::string params((static_cast<const CEGUI::WindowEventArgs*> (&i_event))->window->getName().c_str());
	int pipePos(params.find('|'));
	std::string actionName(params.substr(0, pipePos));
	params = params.substr(pipePos + 1);

	m_RTSActionList[actionName]->doAction(params);

	return true;
}

CEGUI::Window* RTSGUI::createRTSButton(const std::string& i_container, const std::string& i_name, const std::string& imageName)
{
	CEGUI::Window* button(createButton(i_container, i_name, imageName));
	button->subscribeEvent(CEGUI::PushButton::EventMouseButtonUp,
	CEGUI::Event::Subscriber(&RTSGUI::doAction,this));
	return button;
}

void RTSGUI::addCurrentAction(const std::string& i_action)
{
	m_RTSActionList[i_action]->activateButton();
	currentActions.push_back(i_action);
}

void RTSGUI::clearCurrentAction()
{
	for(int i(0), nbAction(currentActions.size()); i < nbAction; ++i)
		m_RTSActionList[currentActions[i]]->desactivateButton();

	std::vector<std::string> emptyVector;
	currentActions.swap(emptyVector);
	m_lastButtonPosition = 0;
} 