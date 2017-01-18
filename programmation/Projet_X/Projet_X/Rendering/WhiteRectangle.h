#ifndef WHITE_RECTANGLE_H
#define WHITE_RECTANGLE_H

#include <vector>
#include "WhiteLine.h"

class Camera;
class Matrix;


class WhiteRectangle: public WhiteLine
{
	public:
		WhiteRectangle();
		~WhiteRectangle();

		virtual void animate(Uint32 i_timestep){}
	protected:
		void generateBuffersData(std::vector<Vector>& i_vertices, std::vector<GLuint>& i_indices);
};

#endif //WHITE_RECTANGLE_H
