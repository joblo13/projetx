#include "MoveGroupAction.h"

#include "../../Math/Vector.h"
#include "..\..\Entities\Unit.h"
#include "../../Definition.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn MoveGroupAction(Vector &ix_targetCube)
///
/// Constructeur
///
/// @param[in] ix_targetCube : position visée
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
MoveGroupAction::MoveGroupAction(Vector &ix_targetCube)
{
	m_targetCube = new Vector(ix_targetCube);
	m_type = MOVE_GROUP_ACTION;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~MoveGroupAction()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
MoveGroupAction::~MoveGroupAction()
{
	delete m_targetCube;
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
void MoveGroupAction::executeAction(Unit* ip_unit, Uint32 i_timestep)
{
	if(ip_unit != NULL)
	{
		Vector nextMovement(*m_targetCube - ip_unit->getPosition());
		Vector lastMovement(*m_targetCube - ip_unit->m_lastPosition);

		//On vérifie si on est plus près du prochain objectif
		MoveGroupAction* nextMoveAction = dynamic_cast<MoveGroupAction*>( (ip_unit->mp_targetList)[1] );
		if(nextMoveAction != NULL)
		{
			Vector distToNextCube = nextMoveAction->getTargetCube() - ip_unit->getPosition();
			Vector distBetweenCubes = nextMoveAction->getTargetCube() - *m_targetCube;

			distToNextCube.y = 0;
			distBetweenCubes.y = 0;

			if(distToNextCube.length() < distBetweenCubes.length())
			{
				ip_unit->mp_targetList.pop_back();
				return;
			}
		}		

		//On regarde si on a atteint notre objectif
		nextMovement.y = 0;
		lastMovement.y = 0;

		if(nextMovement.length() > MOVE_TRESHOLD)
		{
			if(nextMovement.length() <= lastMovement.length())
			{
				//On effectue une partie du déplacement vers la position à atteindre.
				ip_unit->move(nextMovement, false, i_timestep);
			}
			else
			{
				//On saute pour se rendre puisqu'on a reculé
				ip_unit->move(nextMovement, true, i_timestep);
				ip_unit->m_lastPosition = Vector(FLT_MAX , FLT_MAX, FLT_MAX ); //on ne veut pas sauter 2 fois de suite
			}
		}
		else
		{
			ip_unit->mp_targetList.pop_back();
		}
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
Vector MoveGroupAction::getTargetCube()
{ 
	return *m_targetCube;
}