#ifndef RTS_GUI_H
#define RTS_GUI_H

#include "GUI.h"


class Chunk;
class RTSAction;


class RTSGUI : public GUI
{
	public:
		RTSGUI();
		virtual ~RTSGUI();

		virtual void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera);

		virtual void registerHandlers(){}

		Chunk* getTemplateShape(int i_index);
		//On ne doit pas pouvoir enlever des templates
		void addTemplate(const std::string& i_filePath);

		CEGUI::Window* createRTSButton(const std::string& i_container, const std::string& i_name, const std::string& imageName);
		void generateButton(const std::string& i_name);

		int getLastButtonPosition(){return m_lastButtonPosition;}
		void setLastButtonPosition(int i_lastButtonPosition){m_lastButtonPosition = i_lastButtonPosition;}

		void clearCurrentAction();

		void addCurrentAction(const std::string& i_action);

	protected:
	private:
		void chooseButtons();
		bool doAction(const CEGUI::EventArgs& i_event);

		std::map<std::string, RTSAction*> m_RTSActionList;
		std::vector<std::string> currentActions;
		int m_lastButtonPosition;


};


#endif //RTS_GUI_H
