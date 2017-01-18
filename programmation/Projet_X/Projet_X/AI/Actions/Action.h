#ifndef ACTION_H
#define ACTION_H

#include <SDL/SDL.h>

class Unit;


enum Action_Type
{
	ADD_ACTION,
	DELETE_ACTION,
	MOVE_ACTION,
	MOVEASTAR_ACTION,
	MOVE_GROUP_ACTION,
	BUILD_ACTION
};

//Classe abstraite d'action
class Action
{
public:
	Action(void);
	virtual ~Action(void);
	virtual void executeAction(Unit* ip_unit, Uint32 i_timestep) = 0;

	Action_Type getType(){ return m_type;};

protected:
	Action_Type m_type;
};

#endif //ACTION_H