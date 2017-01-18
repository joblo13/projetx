#ifndef FILL_ACTION_H
#define FILL_ACTION_H

#include "RTSAction.h"
#include <map>

class Chunk;

class FillAction: public RTSAction
{
	public:
		FillAction(RTSGUI* ip_GUIPointer, const std::string& i_name);
		~FillAction();
		
		virtual void doAction(const std::string& i_param);
		virtual void createButton();
	protected:

	private:
};


#endif //FILL_ACTION_H
