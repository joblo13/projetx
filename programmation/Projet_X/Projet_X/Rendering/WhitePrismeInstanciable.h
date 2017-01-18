#ifndef WHITE_RECTANGLE_INSTANCIABLE_H
#define WHITE_RECTANGLE_INSTANCIABLE_H

#include <vector>
#include "WhiteLineInstanciable.h"

class Matrix;


class WhitePrismeInstanciable: public WhiteLineInstanciable
{
	public:
		WhitePrismeInstanciable();
		~WhitePrismeInstanciable();

		virtual void animate(Uint32 i_timestep){}
	protected:
		virtual void updateAdditionnalBuffers()const;
		void generateBuffersData(std::vector<Vector>& i_vertices, std::vector<GLuint>& i_indices);	
};

#endif //WHITE_RECTANGLE_INSTANCIABLE_H
