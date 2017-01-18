#include "WhiteCircle.h"
#include "../Math/Matrix.h"
#include "../Camera/Camera.h"
#include "../Shaders/Shader.h"

#define SELECTOR_RADIUS 0.3f
#define SELECTOR_NUM_SEGMENT 20

WhiteCircle::WhiteCircle()
{
	fillBuffer();
}

WhiteCircle::~WhiteCircle()
{

}

void WhiteCircle::generateBuffersData(std::vector<Vector>& i_vertices, std::vector<GLuint>& i_indices)
{   
	Vector vertex(0,0,0);

	float theta = 2 * MATH_PI / float(SELECTOR_NUM_SEGMENT); 
	float cosPerIteration = cosf(theta);
	float sinPerIteration = sinf(theta);
	float oldX;
	float x = SELECTOR_RADIUS;
	float z = 0; 

	for(int i(0); i < SELECTOR_NUM_SEGMENT; ++i) 
	{ 
		i_indices.push_back(i);
		vertex.x = x;
		vertex.z = z;
		i_vertices.push_back(vertex);
        
		//apply the rotation matrix
		oldX = x;
		x = cosPerIteration * x - sinPerIteration * z;
		z = sinPerIteration * oldX + cosPerIteration * z;
	} 
	MeshEntry meshEntry;
	meshEntry.baseIndex = 0;
	meshEntry.baseVertex = 0;
	meshEntry.materialIndex = 0;
	meshEntry.numIndices = i_indices.size();
	m_entries.push_back(meshEntry);
}

void WhiteCircle::updateAdditionnalBuffers()const
{
}

