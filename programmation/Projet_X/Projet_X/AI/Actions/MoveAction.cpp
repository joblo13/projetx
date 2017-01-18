#include "MoveAction.h"

#include "../../Math/Vector.h"
#include "../../Entities/Unit.h"
#include "../../Definition.h"

#include "../../GUI/GUIHandler.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn MoveAction(Vector &ix_targetCube)
///
/// Constructeur
///
/// @param[in] ix_targetCube : position visée
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
MoveAction::MoveAction(Vector &ix_targetCube)
{
	m_targetCubePos = new Vector(ix_targetCube);
	m_type = MOVE_ACTION;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~MoveAction()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
MoveAction::~MoveAction()
{
	delete m_targetCubePos;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void executeAction(Unit* ip_unit)
///
/// Fonction exécutant l'action de MoveAction
/// 
/// @param[in] ip_unit : unité à déplacer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void MoveAction::executeAction(Unit* ip_unit, Uint32 i_timestep)
{	
	if(abs(ip_unit->getPosition().x - m_targetCubePos->x) + abs(ip_unit->getPosition().z - m_targetCubePos->z) > MOVE_TRESHOLD)
	{
		MoveAction* nextMoveAction = dynamic_cast<MoveAction*>( (ip_unit->mp_targetList)[0]);
		ip_unit->move((*m_targetCubePos - ip_unit->getPosition()).normalize(), false, i_timestep);
	}
	else
	{
		ip_unit->mp_targetList.pop_back();
		if(ip_unit->mp_targetList.size() == 0)
			ip_unit->stopMoving();
		else if((*(--ip_unit->mp_targetList.end()))->getType() != MOVE_ACTION)
			ip_unit->stopMoving();
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector getTargetCube()
///
/// Accesseur de la position du position du cube cible
///
/// @return Position du cube cible
///
////////////////////////////////////////////////////////////////////////
Vector MoveAction::getTargetCubePos()
{ 
	return *m_targetCubePos;
}