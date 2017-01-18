#ifndef MAP_GUI_H
#define MAP_GUI_H

#include "GUI.h"

#define MAP_PIXEL_SIZE 3

#include <vector>
#include <hash_set>

class PixelPosition
{
public:
	PixelPosition(int i_first, int i_second)
	:first(i_first), second(i_second)
	{

	}
	operator size_t()const 
	{
		return (size_t)first | ((size_t)second) << sizeof(first);
	}
	bool operator<(const PixelPosition& i_pixelPosition)const
	{
		if(first == i_pixelPosition.first)
		{
			return second < i_pixelPosition.second;
		}
		else
		{
			return first < i_pixelPosition.first;
		}
	}
		int first;
		int second;
};

class MapGUI : public GUI
{
	public:
		MapGUI();
		virtual ~MapGUI();

		virtual void updateLayout(Uint32 i_timestep, int i_teamID, const Camera* ip_camera);
		virtual void registerHandlers();
		virtual void load();
		void updateImage();
		void reloadImage();
		std::vector<int> getColorAverage(int i_cubeType);
		void addPixelToUpdate(int i_x, int i_y);

		bool isMouseInside(){return m_mouseInside;}

	protected:
	private:
		void generateBaseColor();

		bool mouseEnterMap(const CEGUI::EventArgs& i_event);
		bool mouseExitMap(const CEGUI::EventArgs& i_event);
		bool mouseRelease(const CEGUI::EventArgs& i_event);

		std::vector<std::vector<int>> m_colorAverage;
		std::vector<VertexPosition> m_vertices;

		std::vector<int> m_texturesIndex;

		std::hash_set<PixelPosition> m_pixelToUpdate;
		float m_scaling;
		float m_offsetX;
		float m_offsetY;

		bool m_mouseInside;
};


#endif //MAP_GUI_H
