#ifndef DELETE_ACTION_H
#define DELETE_ACTION_H

#include "Action.h"

class Vector;
class Vector3I;
class Unit;

//Action qui supprime un cube
class DeleteAction :
	public Action
{
public:
	DeleteAction(Vector3I& i_targetCube);
	virtual ~DeleteAction();
	virtual void executeAction(Unit* ip_unit, Uint32 i_timestep);
	
private:
	//Position du cube à supprimer
	Vector3I* mx_targetCubePos;
};

#endif //DELETE_ACTION_H