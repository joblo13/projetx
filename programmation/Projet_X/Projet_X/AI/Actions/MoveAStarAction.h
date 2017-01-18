#ifndef MOVE_ASTAR_ACTION_H
#define MOVE_ASTAR_ACTION_H

#include "Action.h"

class Vector;
class Vector3I;
class Unit;

//Action qui calcule avec A* les MoveAction � faire par l'unit� pour se d�placer � un point 
class MoveAStarAction : public Action
{
	public:
		MoveAStarAction(const Vector3I &i_targetCube);
		virtual ~MoveAStarAction();
		virtual void executeAction(Unit* ip_unit, Uint32 i_timestep);
		Vector3I getTargetCube();
	
	private:
		//En cube
		Vector3I* m_targetCube;
};

#endif //MOVE_ASTAR_ACTION_H