#ifndef TEMPLATE_CREATION_ACTION_H
#define TEMPLATE_CREATION_ACTION_H

#include "RTSAction.h"
#include <map>

class Chunk;

class TemplateCreationAction: public RTSAction
{
	public:
		TemplateCreationAction(RTSGUI* ip_GUIPointer, const std::string& i_name);
		~TemplateCreationAction();
		
		virtual void doAction(const std::string& i_param);
		virtual void createButton();
	protected:

	private:
};


#endif //TEMPLATE_CREATION_ACTION_H
