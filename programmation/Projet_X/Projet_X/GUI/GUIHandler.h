#ifndef GUI_HANDLER_H
#define GUI_HANDLER_H

#include <SDL/SDL.h>
#include "../Definition.h"

#include "DebugHUD.h"

#include <CEGUI/CEGUI.h>
#include <CEGUI/System.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include <CEGUI/DefaultResourceProvider.h>
#include <CEGUI/colour.h>
#include <CEGUI/SchemeManager.h>
#include <CEGUI/RendererModules/OpenGL/Texture.h>

#include "../IconManager.h"

class GUI;
class Camera;

enum LayoutType{FPS_LAYOUT, RTS_LAYOUT, CONSOLE_LAYOUT, DEBUG_HUD_LAYOUT, WEAPONSHOP_LAYOUT, MATERIAL_SELECTION_LAYOUT, ESCAPE_MENU_LAYOUT, MAIN_MENU_LAYOUT, TEMPLATE_EDITOR_LAYOUT, MAP_LAYOUT, NB_LAYOUT};

class GUIHandler
{
	public:
		//Singleton
		static GUIHandler* getInstance()
		{
			if(mp_instance == 0)
			{
				mp_instance = new GUIHandler();
				if(!mp_instance->initCEGUI())
					exit(-12);
			}
			return mp_instance;
		}

		static void releaseInstance()
		{
			delete mp_instance;
			mp_instance = 0;
		}

		void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera);
		void draw();

		bool getVisibility(LayoutType i_layoutType);
		bool toggleVisibility(LayoutType i_layoutType);
		void setVisibility(LayoutType i_layoutType, bool i_visibility);

		template <class T>
		void addToDebugHUD(const T& i_message)
		{
			static_cast<DebugHUD*>(mpa_GUI[DEBUG_HUD_LAYOUT])->addToBuffer(i_message);
		}
		void printDebugHUD();

		const GUI* getLayout(LayoutType i_layoutType){return mpa_GUI[i_layoutType];}

		void setNeedToReload(bool i_needToReload){m_needToReload = i_needToReload;}

		int getIconNum(){return iconManager.getIconNum();}

		void preGUIDraw();
		void postGUIDraw();

		CEGUI::OpenGL3Renderer* getOpenGLRenderer(){return mp_openGLRenderer;}
		void createOpenGLImage(int i_textureID, LayoutType i_layoutType, const std::string& i_name);
		void updateOpenGLImage(int i_textureID, LayoutType i_layoutType, const std::string& i_name, float i_scalling, float i_offsetX, float i_offsetY);

	private:
		GUIHandler();
		~GUIHandler();

		bool initCEGUI();
		void createGUI();
		void reloadLayout();

		GUI* mpa_GUI[NB_LAYOUT];
		CEGUI::Window* m_root;
		static GUIHandler* mp_instance;

		bool m_needToReload;
		IconManager iconManager;
		CEGUI::OpenGL3Renderer* mp_openGLRenderer;
};


#endif //GUI_HANDLER_H
