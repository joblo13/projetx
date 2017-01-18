#ifndef GUI_H
#define GUI_H

#include <string>

#include "../Definition.h"

#include <SDL/SDL.h>

#include <CEGUI/CEGUI.h>
#include <CEGUI/System.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>
#include <CEGUI/DefaultResourceProvider.h>
#include <CEGUI/colour.h>
#include <CEGUI/SchemeManager.h>

class Camera;

class GUI
{
	public:
		GUI();
		virtual ~GUI() = 0;

		bool getVisible()const{assert(m_layout->isVisible() == m_isVisible); return m_isVisible;}

		virtual void setVisible(bool i_isVisible);
		
		CEGUI::Window* getLayout(){return m_layout;}

		virtual void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera) = 0;

		virtual void registerHandlers() = 0;

		bool isMouseVisible(){return m_isVisible && m_needMouse;}

		void activate();

		std::pair<float, float> getAbsoluteSize(const std::string& i_path)const;
		void moveButton(const std::string& i_containerName, const std::string& i_componantName, int i_position);

		virtual void load(){}

	protected:

		CEGUI::Window* createButton(const std::string& i_contener, const std::string& i_name, const std::string& i_imageName);

		std::string convertToString(unsigned int i_value);
		bool m_isVisible;
		CEGUI::Window* m_layout;
		bool m_needMouse;
		bool m_onTop;
};


#endif //GUI_H
