#ifndef BUILD_TEMPLATE_ACTION_H
#define BUILD_TEMPLATE_ACTION_H

#include "Action.h"

class Template;

//Action pour ajouter un cube
class BuildTemplateAction :
	public Action
{
public:
	BuildTemplateAction(Template* ip_template);
	virtual ~BuildTemplateAction();
	virtual void executeAction(Unit* ip_unit, Uint32 i_timestep);
	
private:
	Template* mp_template;
};

#endif //BUILD_TEMPLATE_ACTION_H