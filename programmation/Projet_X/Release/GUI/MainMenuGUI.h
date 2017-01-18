#ifndef MAIN_MENU_GUI_H
#define MAIN_MENU_GUI_H

#include "GUI.h"

class Camera;

class MainMenuGUI : public GUI
{
	public:
		MainMenuGUI();
		virtual ~MainMenuGUI();

		virtual void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera);

		virtual void registerHandlers();


	protected:
		//Commande
		bool startButHandler(const CEGUI::EventArgs& i_event);
};


#endif //MAIN_MENU_GUI_H
