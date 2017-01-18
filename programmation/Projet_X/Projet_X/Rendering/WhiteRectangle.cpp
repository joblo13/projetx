#include "WhiteRectangle.h"
#include "../Math/Matrix.h"
#include "../Camera/Camera.h"
#include "../Shaders/Shader.h"

WhiteRectangle::WhiteRectangle()
{
	fillBuffer();
}

WhiteRectangle::~WhiteRectangle()
{

}

void WhiteRectangle::generateBuffersData(std::vector<Vector>& i_vertices, std::vector<GLuint>& i_indices)
{   

	//On extrait les informations de la scene
	Vector position;
	int index(-1);

	position.x = -0.5f;
	position.y = -0.5f;
	position.z = 0.0f;
	i_vertices.push_back(position);
	i_indices.push_back(++index);

	position.x = 0.5f;
	position.y = -0.5f;
	position.z = 0.0f;
	i_vertices.push_back(position);
	i_indices.push_back(++index);

	position.x = 0.5f;
	position.y = 0.5f;
	position.z = 0.0f;
	i_vertices.push_back(position);
	i_indices.push_back(++index);
	
	position.x = -0.5f;
	position.y = 0.5f;
	position.z = 0.0f;
	i_vertices.push_back(position);
	i_indices.push_back(++index);
}