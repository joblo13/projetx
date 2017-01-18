 #ifndef RTS_ACTION_H
#define RTS_ACTION_H

#include <string>
#include <vector>
#include <SDL/SDL.h>
#include <CEGUI/CEGUI.h>

class RTSGUI;

class RTSAction
{
	public:
		RTSAction(RTSGUI* ip_GUIPointer, const std::string& i_name);
		~RTSAction();
		virtual void createButton() = 0;
		virtual void doAction(const std::string& i_param) = 0;
		const std::vector<std::vector<SDLKey>>* getShortcuts(){return &m_shortcuts;}
		void activateButton();
		void desactivateButton();

	protected:
		std::vector<std::vector<SDLKey>> m_shortcuts;

		RTSGUI* mp_GUIPointer;

		std::string m_name;

		std::vector<CEGUI::Window*> mp_buttons;
};


#endif //RTS_ACTION_H
