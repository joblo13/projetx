#include "WhitePrismeInstanciable.h"
#include "../Math/Matrix.h"
#include "../Camera/Camera.h"
#include "../Shaders/Shader.h"

#define SELECTOR_RADIUS 0.3f
#define SELECTOR_NUM_SEGMENT 20

WhitePrismeInstanciable::WhitePrismeInstanciable()
{
	fillBuffer();
	m_drawingMode = GL_LINES;
}

WhitePrismeInstanciable::~WhitePrismeInstanciable()
{

}

void WhitePrismeInstanciable::generateBuffersData(std::vector<Vector>& i_vertices, std::vector<GLuint>& i_indices)
{   
	Vector vertex;

	//sommet0
	vertex.x = 0.f;
	vertex.y = 0.f;
	vertex.z = 0.f;
	i_vertices.push_back(vertex);
	//sommet1
	vertex.x = 1.f;
	vertex.y = 0.f;
	vertex.z = 0.f;
	i_vertices.push_back(vertex);
	//sommet2
	vertex.x = 1.f;
	vertex.y = 0.f;
	vertex.z = 1.f;
	i_vertices.push_back(vertex);
	//sommet3
	vertex.x = 0.f;
	vertex.y = 0.f;
	vertex.z = 1.f;
	i_vertices.push_back(vertex);

	//sommet4
	vertex.x = 0.f;
	vertex.y = 1.f;
	vertex.z = 0.f;
	i_vertices.push_back(vertex);
	//sommet5
	vertex.x = 1.f;
	vertex.y = 1.f;
	vertex.z = 0.f;
	i_vertices.push_back(vertex);
	//sommet6
	vertex.x = 1.f;
	vertex.y = 1.f;
	vertex.z = 1.f;
	i_vertices.push_back(vertex);
	//sommet7
	vertex.x = 0.f;
	vertex.y = 1.f;
	vertex.z = 1.f;
	i_vertices.push_back(vertex);

	i_indices.push_back(0);
	i_indices.push_back(1);

	i_indices.push_back(1);
	i_indices.push_back(2);

	i_indices.push_back(2);
	i_indices.push_back(3);

	i_indices.push_back(3);
	i_indices.push_back(0);


	i_indices.push_back(4);
	i_indices.push_back(5);

	i_indices.push_back(5);
	i_indices.push_back(6);

	i_indices.push_back(6);
	i_indices.push_back(7);

	i_indices.push_back(7);
	i_indices.push_back(4);


	i_indices.push_back(0);
	i_indices.push_back(4);

	i_indices.push_back(1);
	i_indices.push_back(5);

	i_indices.push_back(2);
	i_indices.push_back(6);

	i_indices.push_back(3);
	i_indices.push_back(7);

	MeshEntry meshEntry;
	meshEntry.baseIndex = 0;
	meshEntry.baseVertex = 0;
	meshEntry.materialIndex = 0;
	meshEntry.numIndices = i_indices.size();
	m_entries.push_back(meshEntry);
}

void WhitePrismeInstanciable::updateAdditionnalBuffers()const
{
}

