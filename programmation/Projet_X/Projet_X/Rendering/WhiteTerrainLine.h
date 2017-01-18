#ifndef WHITE_TERRAIN_LINE_H
#define WHITE_TERRAIN_LINE_H

#include <vector>
#include "WhiteLine.h"

class Camera;
class Matrix;
class Chunk;


class WhiteTerrainLine: public WhiteLine
{
	public:
		WhiteTerrainLine(Chunk* ip_chunk);
		~WhiteTerrainLine();

		virtual void animate(Uint32 i_timestep){}
	protected:
		virtual void transformModelview(Matrix& i_modelview, const Camera* ipk_camera)const;
		void generateBuffersData(std::vector<Vector>& i_vertices, std::vector<GLuint>& i_indices);

		Chunk* mp_chunk;
};

#endif //WHITE_TERRAIN_LINE_H
