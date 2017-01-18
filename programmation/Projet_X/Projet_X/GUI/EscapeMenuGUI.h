#ifndef ESCAPE_MENU_GUI_H
#define ESCAPE_MENU_GUI_H

#include "GUI.h"

class Camera;

class EscapeMenuGUI : public GUI
{
	public:
		EscapeMenuGUI();
		virtual ~EscapeMenuGUI();

		virtual void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera);

		virtual void registerHandlers();

	protected:
		bool resumeHandler(const CEGUI::EventArgs& i_event);
		bool optionHandler(const CEGUI::EventArgs& i_event);
		bool mainMenuHandler(const CEGUI::EventArgs& i_event);
		bool exitHandler(const CEGUI::EventArgs& i_event);
};


#endif //ESCAPE_MENU_GUI_H
