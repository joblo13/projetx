#ifndef MOVE_GROUP_ACTION_H
#define MOVE_GROUP_ACTION_H

#include "Action.h"

class Vector;
class Unit;

//Action qui déplace l'unité à un point précis sans passer par A*
class MoveGroupAction : public Action
{
public:
	MoveGroupAction(Vector &i_targetCube);
	virtual ~MoveGroupAction();
	virtual void executeAction(Unit* ip_unit, Uint32 i_timestep);
	Vector getTargetCube();
	
private:
	//Position réelle
	Vector* m_targetCube;
	int m_unitIDInGroup;
	int m_nbUnitInGroup;

};

#endif //MOVE_GROUP_ACTION_H