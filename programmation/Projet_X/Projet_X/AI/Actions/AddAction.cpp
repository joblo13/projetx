#include "AddAction.h"

#include "../../Math/Vector.h"
#include "../../Math/Vector3I.h"
#include "..\..\Entities\Unit.h"
#include "../../Terrain/Terrain.h"

AddAction::AddAction(Vector3I& i_targetCubePos)
{
	m_targetCubePos = new Vector3I(i_targetCubePos);
	m_type = ADD_ACTION;
}


AddAction::~AddAction(void)
{
	delete m_targetCubePos;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void executeAction(Unit* ipx_unit)
///
/// Fonction exécutant l'action de AddAction
/// 
/// @param[in] ipx_unit : unité à déplacer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void AddAction::executeAction(Unit* ip_unit, Uint32 i_timestep)
{
	if(ip_unit != NULL /*&& ipx_unit->mn_nbRessources > 0 */)
	{
		//On oriente l'unité vers le cube à détruire.
		Vector3I vector;
		Vector3I vectorAngle;
		float vectorLength;

		vector = *m_targetCubePos - Terrain::positionToCube(ip_unit->getPosition());
		vector.y = 0;
				
		vectorLength = vector.length();
		if(vectorLength != 0)
		{
			//On oriente l'unité vers le cube à détruire.
			vectorAngle = vector;
			vectorAngle.normalize();
			ip_unit->m_theta = std::acos((float)(Vector3I(0, 0, 1) * vectorAngle)) * (180/MATH_PI);
			
		}

		//On détruit le cube devant l'unité.
		Terrain* p_terrain(Terrain::getInstance());
		p_terrain->createCube( *m_targetCubePos, 1, 100 , 100);

		//Target atteint
		ip_unit->mp_targetList.pop_back();
	}
}