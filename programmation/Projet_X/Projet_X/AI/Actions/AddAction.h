#ifndef ADD_ACTION_H
#define ADD_ACTION_H

#include "Action.h"

class Vector;
class Vector3I;
class Unit;

//Action pour ajouter un cube
class AddAction :
	public Action
{
public:
	AddAction(Vector3I& i_targetCube);
	virtual ~AddAction();
	virtual void executeAction(Unit* ip_unit, Uint32 i_timestep);
	
private:
	//Position du cube à ajouter
	Vector3I* m_targetCubePos;
};

#endif //ADD_ACTION_H