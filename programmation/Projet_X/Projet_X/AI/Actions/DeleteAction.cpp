#include "DeleteAction.h"

#include "../../Math/Vector.h"
#include "../../Math/Vector3I.h"
#include "..\..\Entities\Unit.h"
#include "../../Terrain/Terrain.h"

DeleteAction::DeleteAction(Vector3I& i_targetCubePos)
{
	mx_targetCubePos = new Vector3I(i_targetCubePos);
	m_type = DELETE_ACTION;
}


DeleteAction::~DeleteAction(void)
{
	delete mx_targetCubePos;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void executeAction(Unit* ipx_unit)
///
/// Fonction exécutant l'action de DeleteAction
/// 
/// @param[in] ipx_unit : unité à déplacer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void DeleteAction::executeAction(Unit* ipx_unit, Uint32 i_timestep)
{
	if(ipx_unit != NULL)
	{
		//On oriente l'unité vers le cube à détruire.
		Vector3I vector;
		Vector3I vectorAngle;
		float vectorLength;

		vector = *mx_targetCubePos - Terrain::positionToCube(ipx_unit->getPosition());
		vector.y = 0;
				
		vectorLength = vector.length();
		if(vectorLength != 0)
		{
			//On oriente l'unité vers le cube à détruire.
			vectorAngle = vector;
			vectorAngle.normalize();
			ipx_unit->m_theta = std::acos((float)(Vector3I(0, 0, 1) * vectorAngle)) * (180/MATH_PI);
			
		}

		//On détruit le cube devant l'unité.
		Terrain* p_terrain(Terrain::getInstance());
		p_terrain->removeCube( *mx_targetCubePos );

		//Target atteint
		ipx_unit->mp_targetList.pop_back();
	}
}
