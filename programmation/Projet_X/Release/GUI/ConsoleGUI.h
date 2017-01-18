#ifndef CONSOLE_GUI_H
#define CONSOLE_GUI_H

#include "GUI.h"

#include <list>

class Camera;

class ConsoleGUI : public GUI
{
	public:
		ConsoleGUI();
		virtual ~ConsoleGUI();

		virtual void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera);

		virtual void registerHandlers();

	protected:
		bool textSubmittedHandler(const CEGUI::EventArgs& i_event);
		void parseText(CEGUI::String i_message);
		void outputText(CEGUI::String i_message, CEGUI::Colour i_colour = CEGUI::Colour( 0xFFFFFFFF));

		//Commande
		void save(const std::vector<std::string>& i_arguments);
		void load(const std::vector<std::string>& i_arguments);
		void giveRessources(const std::vector<std::string>& i_arguments);

		void changeMesh(const std::vector<std::string>& i_arguments);
		void saveMesh(const std::vector<std::string>& i_arguments);
		void getMeshes();
		void getCube(const std::vector<std::string>& i_arguments);
		void updateCube(const std::vector<std::string>& i_arguments);
		void reloadGUI(const std::vector<std::string>& i_arguments);

		std::list<CEGUI::String> m_oldMessage;
		std::list<CEGUI::String>::iterator m_currentMessage;
};


#endif //CONSOLE_GUI_H
