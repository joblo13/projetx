#ifndef DEBUG_HUD_H
#define DEBUG_HUD_H

#include "GUI.h"

#include <sstream>

class Camera;

class DebugHUD : public GUI
{
	public:
		DebugHUD();
		virtual ~DebugHUD();

		virtual void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera);

		virtual void registerHandlers();

		template <class T>
		void addToBuffer(const T& ik_text){m_buffer << ik_text;}
		void printBuffer();

	protected:
		std::stringstream m_buffer;
};


#endif //DEBUG_HUD_H
