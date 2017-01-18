#ifndef MATERIAL_SELECTION_GUI_H
#define MATERIAL_SELECTION_GUI_H

#include "GUI.h"

class MaterialSelectionGUI : public GUI
{
	public:
		MaterialSelectionGUI();
		virtual ~MaterialSelectionGUI();

		virtual void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera);

		virtual void registerHandlers();

		int getCurrentCubeType()const{return -m_currentSelectedCubeTypeID;}

	protected:
		void init();
		bool selectionChanged(const CEGUI::EventArgs& i_event);

		int m_currentSelectedCubeTypeID;
		std::vector<std::string> m_textureNameList;
		int m_imageCount;
};


#endif //MATERIAL_SELECTION_GUI_H
