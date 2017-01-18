#include "WhiteTerrainLine.h"
#include "../Math/Matrix.h"
#include "../Camera/Camera.h"
#include "../Shaders/Shader.h"
#include "../Terrain/Chunk.h"

WhiteTerrainLine::WhiteTerrainLine(Chunk* ip_chunk)
:mp_chunk(ip_chunk)
{
	m_drawingMode = GL_LINES;
	fillBuffer();
}

WhiteTerrainLine::~WhiteTerrainLine()
{

}

void WhiteTerrainLine::generateBuffersData(std::vector<Vector>& i_vertices, std::vector<GLuint>& i_indices)
{   
	for(int i(0); i < mp_chunk->getSizeX() ; ++i)
		for(int j(0) ; j < mp_chunk->getSizeY() ; ++j)
			for(int k(0); k < mp_chunk->getSizeZ() ; ++k)
			{
					mp_chunk->createLineMesh(i, j, k, i_vertices, i_indices);
			}
}

void WhiteTerrainLine::transformModelview(Matrix& i_modelview, const Camera* ipk_camera)const
{
	i_modelview.translate(mp_chunk->getX() * mp_chunk->getSizeX() * SIDE, mp_chunk->getY() * mp_chunk->getSizeY() * SIDE, mp_chunk->getZ() * mp_chunk->getSizeZ() * SIDE);
}