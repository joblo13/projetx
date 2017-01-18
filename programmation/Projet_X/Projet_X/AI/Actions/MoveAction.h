#ifndef MOVE_ACTION_H
#define MOVE_ACTION_H

#include "Action.h"

class Vector;
class Unit;

//Action qui déplace l'unité à un point précis sans passer par A*
class MoveAction :
	public Action
{
public:
	MoveAction(Vector &i_targetCube);
	virtual ~MoveAction();
	virtual void executeAction(Unit* ip_unit, Uint32 i_timestep);
	Vector getTargetCubePos();
	
private:
	//Position réelle
	Vector* m_targetCubePos;
};

#endif //MOVE_ACTION_H