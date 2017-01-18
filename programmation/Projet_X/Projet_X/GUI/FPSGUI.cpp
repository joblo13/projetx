#include "FPSGUI.h"
#include "../Entities/UnitHandler.h"
#include "../Entities/Unit.h"
#include "../Input.h"
#include "../Camera/Camera.h"
#include "GUIHandler.h"

FPSGUI::FPSGUI()
:m_weaponAlphaTimer(0, 1000)
{
	m_onTop = true;
	m_needMouse = false;
	m_layout = CEGUI::WindowManager::getSingleton().loadLayoutFromFile( "FPS.layout", "layouts");
	setVisible(false);
}

FPSGUI::~FPSGUI()
{

}

void FPSGUI::updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera)
{
	//Visibilité des armes
	if(m_weaponAlphaTimer.doAction(false))
	{
		CEGUI::Window* p_componant = m_layout->getChild("weaponList");
		p_componant->setAlpha(p_componant->getAlpha() - ((float)i_timestep/1000.0f));
	}
	m_weaponAlphaTimer.increment(i_timestep);

	Input* p_input(Input::getInstance());
	if((p_input->getKeyState("switchGun1") || p_input->getKeyState("switchGun2") ||
		p_input->getKeyState("switchGun3") || p_input->getKeyState("switchGun4") ||
		p_input->getKeyState("switchGun5") || p_input->getKeyState("switchGun6") ||
		p_input->getKeyState("switchGun7") || p_input->getKeyState("switchGun8") ||
		p_input->getKeyState("switchGun9") || p_input->getKeyState("switchGun0")) && !p_input->getMouseVisible())
	{
		m_weaponAlphaTimer.setCurrentTime(0);
		m_layout->getChild("weaponList")->setAlpha(1.0f);
	}

	//Affichage des ressources
	UnitHandler* p_unitHandler(UnitHandler::getInstance());
	m_layout->getChild("ressources/res_cube")->setText(convertToString(p_unitHandler->getRessources(i_teamID, HUMAN_CUBES)));
	m_layout->getChild("ressources/res_weaponPart")->setText(convertToString(p_unitHandler->getRessources(i_teamID, HUMAN_GUN_PARTS)));
	m_layout->getChild("ressources/res_vehiculePart")->setText(convertToString(p_unitHandler->getRessources(i_teamID, HUMAN_VEHICULE_PARTS)));
	m_layout->getChild("ressources/res_advancedPart")->setText(convertToString(p_unitHandler->getRessources(i_teamID, HUMAN_ADVANCED_PARTS)));
	m_layout->getChild("ressources/res_advancedPart")->setText(convertToString(p_unitHandler->getRessources(i_teamID, HUMAN_ADVANCED_PARTS)));
	m_layout->getChild("ressources/res_population")->setText(convertToString(p_unitHandler->getUnitCount(i_teamID)) + "/200");

	int unitID(ip_camera->getUnitID());
	if(unitID != -1)
	{
		m_layout->getChild("ammoAndLife/ammo")->setText(convertToString(p_unitHandler->getUnit(i_teamID, unitID)->getCurrentAmmo()) + "/" + 
														convertToString(p_unitHandler->getUnit(i_teamID, unitID)->getCurrentBackupAmmo()));

		m_layout->getChild("ammoAndLife/heal")->setText(convertToString(p_unitHandler->getUnit(i_teamID, unitID)->getHitpoint()));
	}

	//Affichage du compas
	m_layout->getChild("compas/compas_img")->setPosition(CEGUI::UVector2(CEGUI::UDim(0, -150 - 480 * ip_camera->getTheta()/360.0f), CEGUI::UDim(0, 0)));
}