#ifndef TEMPLATE_EDITOR_GUI_H
#define TEMPLATE_EDITOR_GUI_H

#include "GUI.h"

#include "../Entities/ItemInformation.h"
#include "../Observer.h"

#include <set>

class Camera;
class Unit;
class Chunk;

enum ChoosingKeyState {IDLE_CHOOSING_KEY_STATE, RECORDING_CHOOSING_KEY_STATE, SAVING_CHOOSING_KEY_STATE};

class TemplateEditorGUI : public GUI
{
	public:
		TemplateEditorGUI();
		virtual ~TemplateEditorGUI();

		virtual void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera);

		virtual void registerHandlers();	

		void addSelectorChunk(Chunk* ip_selectorChunk);

	protected:
		void loadIcon();

		bool saveTemplate(const CEGUI::EventArgs& i_event);
		bool openIconSelection(const CEGUI::EventArgs& i_event);
		bool startSelectingShortcut(const CEGUI::EventArgs& i_event);

		void saveTemplateDescription(const std::string& ik_templateName, const std::string& ik_imageName);

		bool chooseIcon(const CEGUI::EventArgs& i_event);

		Chunk* mp_selectorChunk;

		std::string m_selectedIcon;

		ChoosingKeyState m_choosingKeyState;
		std::set<SDLKey> m_totalKeyList;
};


#endif //TEMPLATE_EDITOR_GUI_H
