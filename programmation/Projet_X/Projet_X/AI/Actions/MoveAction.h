#ifndef MOVE_ACTION_H
#define MOVE_ACTION_H

#include "Action.h"

class Vector;
class Unit;

//Action qui d�place l'unit� � un point pr�cis sans passer par A*
class MoveAction :
	public Action
{
public:
	MoveAction(Vector &i_targetCube);
	virtual ~MoveAction();
	virtual void executeAction(Unit* ip_unit, Uint32 i_timestep);
	Vector getTargetCubePos();
	
private:
	//Position r�elle
	Vector* m_targetCubePos;
};

#endif //MOVE_ACTION_H