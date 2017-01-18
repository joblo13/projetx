#include "MoveAStarAction.h"

#include "AddAction.h"
#include "DeleteAction.h"
#include "MoveAction.h"
#include "../../Math/Vector.h"
#include "../../Entities/Unit.h"
#include "../../Terrain/Terrain.h"
#include "../Node.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn MoveAStarAction(Vector &i_targetCube)
///
/// Constructeur
/// 
/// @param[in] i_targetCube : cube visé
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
MoveAStarAction::MoveAStarAction(const Vector3I &i_targetCube)
{
	m_targetCube = new Vector3I(i_targetCube);
	m_type = MOVEASTAR_ACTION;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void ~MoveAStarAction()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
MoveAStarAction::~MoveAStarAction()
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
void MoveAStarAction::executeAction(Unit* ip_unit, Uint32 i_timestep)
{
	if(ip_unit != NULL)
	{
		if(!ip_unit->m_aStar.getIsInit())
		{	
			//on trouve le chemin à partir de la position de l'unité
			Vector3I pathStartPos(Terrain::positionToCube(ip_unit->getPosition()));

			//Init du A*
			ip_unit->m_aStar.init(pathStartPos, *m_targetCube, WALKING_TYPE);
		}
		//Si le A* est initialisé, on effectue une itération de calcul
		else
		{
			AStar* p_aStar = &ip_unit->m_aStar;
	
			Vector targetPostion = p_aStar->getTargetPosition();
			std::vector<Action*> p_tempActionList;
			if(targetPostion.x >= 0 && targetPostion.y >= 0 && targetPostion.z >= 0)
			{
				Node* node;
				if(!p_aStar->getIsDone())
					node = p_aStar->executeSearch();

				if(p_aStar->getIsDone())
				{
					//Calcul du A* fini

					while(node->getParent() != 0)
					{
						p_tempActionList.push_back(new MoveAction(node->getPosition()));
						//On modifie le terrain au besoin
						Vector3I vector;
						if(node->getIsChanging(0))
						{
							vector = node->getCubePosition();
							vector.y -= 2;
							p_tempActionList.push_back(new AddAction(vector));
						}		
						if(node->getIsChanging(1))
						{
							vector = node->getCubePosition();
							--vector.y;
							p_tempActionList.push_back(new AddAction(vector));
						}
						if(node->getIsChanging(2))
						{
							vector = node->getCubePosition();
							p_tempActionList.push_back(new DeleteAction(vector));
						}
						if(node->getIsChanging(3))
						{
							vector = node->getCubePosition();
							++vector.y;
							p_tempActionList.push_back(new DeleteAction(vector));
						}
						if(node->getIsChanging(4))
						{
							vector = node->getCubePosition();
							vector.y += 2;
							p_tempActionList.push_back(new DeleteAction(vector));
						}
						node = node->getParent();
					}
					
					ip_unit->mp_targetList.pop_back();
					ip_unit->mp_targetList.insert(ip_unit->mp_targetList.end(), p_tempActionList.begin(), p_tempActionList.end());

					ip_unit->m_aStar.setIsInit(false);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I getTargetCube()
///
/// Accesseur de la position du position du cube cible
///
/// @return Position du cube cible
///
////////////////////////////////////////////////////////////////////////
Vector3I MoveAStarAction::getTargetCube()
{ 
	return *m_targetCube;
}