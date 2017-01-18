#ifndef WHITE_CIRCLE_H
#define WHITE_CIRCLE_H

#include <vector>
#include "WhiteLineInstanciable.h"

class Matrix;


class WhiteCircle: public WhiteLineInstanciable
{
	public:
		WhiteCircle();
		~WhiteCircle();

		virtual void animate(Uint32 i_timestep){}
	protected:
		virtual void updateAdditionnalBuffers()const;
		void generateBuffersData(std::vector<Vector>& i_vertices, std::vector<GLuint>& i_indices);	
};

#endif //WHITE_CIRCLE_H
