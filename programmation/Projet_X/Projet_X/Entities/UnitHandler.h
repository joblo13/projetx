#ifndef UNIT_HANDLER_H
#define UNIT_HANDLER_H

#include <vector>
#include <hash_set>
#include <SDL/SDL.h>

#include <time.h>
#include <limits.h>
#include <hash_set>

#include "Ressources/Ressources.h"
#include "../Rendering/WhiteCircle.h"
#include "../Rendering/Mesh/HitpointBarMesh.h"

class Unit;
class House;
class Vector;
class Vector3I;
class Position;
class Camera;
class Matrix;
class Mesh;
class btDynamicsWorld;
class BoidGroup;
class Template;

#define MAX_TEAM_COUNT 1

class UnitHandler
{
	public:
		//Singleton
		static UnitHandler* getInstance()
		{
			if(mp_instance == 0)
			{
				mp_instance = new UnitHandler(MAX_TEAM_COUNT);
			}
			return mp_instance;
		}

		static void releaseInstance()
		{
			delete mp_instance;
			mp_instance = 0;
		}

		void addCamera(Camera* ip_camera);

		//Ajout d'unité
		void addUnit(Unit* ip_unit);
		void addHuman(int i_teamID, const Vector3I& i_position);

		//Supression d'unité
		void removeUnit(int i_teamID, int i_unitID);
		
		/*void falling(int i_teamID, int i_unitID, Uint32 i_timestep);*/

		void startPathFind(int i_teamID, int i_unitID, const Vector3I& i_targetCube);

		void buildHouse(int i_teamID, int i_unitID, Vector3I& i_housePosition);
		void buildWall(int i_teamID, int i_unitID, Vector3I& i_wallCubePos1, Vector3I& i_wallCubePos2, Vector3I& i_unitStartCubePos);
		
		//Ajout d'action
		void moveTemplateAction(int i_teamID, int i_unitID, Template* ip_template);
		void buildTemplateAction(int i_teamID, int i_unitID, Template* ip_template);
		void addAction(int i_teamID, int i_unitID, Vector3I& i_targetCube);
		void deleteAction(int i_teamID, int i_unitID, Vector3I& i_targetCube);
		void moveAction(int i_teamID, int i_unitID, Vector& i_targetCube);
		void moveAStarAction(int i_teamID, int i_unitID, const Vector3I& i_targetCube);
		
		
		//void getRessources(int i_teamID, int i_unitID, int i_nbOfRessources);
		
		void findFlatRectangleTerritory(Unit* ip_unit, int i_nbRows, int i_nbColumns, Vector3I& i_corner1, Vector3I& i_corner2);

		void animate(Uint32 i_timestep);

		Unit* getUnit(int i_teamID, int i_unitID);
		int getTeamCount(){return mp_unitList.size();}
		int getUnitCount(int i_teamIndex){return mp_unitList[i_teamIndex].size();}

		std::vector<Unit*> rayCollision(Vector i_sourceVect, Vector i_targetVect, Unit* i_excludedUnit = 0, bool i_onlyFirst = true);
		//std::vector<Unit*> volumeCollision( const Vector i_sourceVect[4], const Vector i_targetVect[4]);
		
		void resetActionList(int i_teamID, int i_unitID);

		unsigned int getRessources(int i_teamID, RessourcesType i_resssourcesType);
		void addRessources(int i_teamID, RessourcesType i_resssourcesType, unsigned int i_ammount);
		bool removeRessources(int i_teamID, RessourcesType i_resssourcesType, unsigned int i_ammount);

		void reloadPhysic();

		//boid
		int createBoidGroup(const std::hash_set<Unit*>& ip_boidList);
		Vector getBoidGroupCenter(int i_boidGroupID);
		void deleteBoidGroup(int i_boidGroupID);

		void applyBoidBehaviors(Uint32 i_timestep);

		void moveBoidGroup(int i_boidGroupID, const Vector3I& i_centerCube);
		//formations
		void rectangleFormation(const Vector3I& i_centerCube, int i_groupSize, const Vector& i_direction, std::vector<Vector3I>& o_endPosition);

		void updateHitpointBar();
		void setIsSelected(Unit* ip_unit, bool i_isSelected);
		void deselectAll();

	private:
		UnitHandler(int in_teamCount);
		~UnitHandler();

		void rayCollision(const Vector3I& i_sourceCubeVect, const Vector3I& i_targetCubeVect, const Vector& i_direction, std::hash_set<Vector3I>& i_visitedCubeList);

		int testTerrain(const Vector3I& ik_center, int i_nbRows, int i_nbColumns);

		std::vector<std::vector<int>> m_freeSpace;
		std::vector<std::vector<Unit*>> mp_unitList;
		std::vector<Ressources*> mp_ressources;
		std::vector<House*> mp_houseList;

		Camera* mp_camera;

		//Instance du singleton
		static UnitHandler* mp_instance;

		WhiteCircle m_whiteCircles;
		HitpointBarMesh* mp_hitpointBarMesh;
		//Boid
		std::vector<BoidGroup*> mp_boidGroups;
};


#endif //UNIT_HANDLER_H
