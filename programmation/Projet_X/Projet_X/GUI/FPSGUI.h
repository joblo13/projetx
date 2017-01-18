#ifndef FPS_GUI_H
#define FPS_GUI_H

#include "GUI.h"
#include "../Timer.h"

class FPSGUI : public GUI
{
	public:
		FPSGUI();
		virtual ~FPSGUI();

		virtual void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera);

		virtual void registerHandlers(){}

	protected:
		Timer m_weaponAlphaTimer;
};


#endif //FPS_GUI_H
