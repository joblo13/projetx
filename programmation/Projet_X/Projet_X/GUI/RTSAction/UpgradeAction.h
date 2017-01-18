#ifndef UPGRADE_ACTION_H
#define UPGRADE_ACTION_H

#include "RTSAction.h"

class UpgradeAction: public RTSAction
{
	public:
		UpgradeAction(RTSGUI* ip_GUIPointer, const std::string& i_name);
		~UpgradeAction();
		
		virtual void doAction(const std::string& i_param);
		virtual void createButton();
	protected:

	private:
};


#endif //UPGRADE_ACTION_H
