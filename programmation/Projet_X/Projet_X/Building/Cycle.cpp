#include "Cycle.h"

#include "BuildingNode.h"
#include "../Math/Vector.h"

Cycle::Cycle( std::vector<BuildingNode*> ivectx_cycleNodes )
{
	mvectx_cycleNodes = ivectx_cycleNodes;
}


Cycle::~Cycle()
{
}

void Cycle::getContainingSquare(Vector& ix_corner1, Vector& ix_corner2)
{
	Vector x_vector1((float)INT_MAX, 0.0f, (float)INT_MAX), x_vector2((float)INT_MIN, 0.0f , (float)INT_MIN);
	//On trouve les points extremes du cycle pour trouver le carré englobant.
	for(unsigned int i(0), cycleNodesSize(mvectx_cycleNodes.size()); i < cycleNodesSize; ++i)
	{
		x_vector1.x = x_vector1.x > mvectx_cycleNodes[i]->getCubePosition().x?
			mvectx_cycleNodes[i]->getCubePosition().x: x_vector1.x;
		x_vector1.z = x_vector1.z > mvectx_cycleNodes[i]->getCubePosition().z?
			mvectx_cycleNodes[i]->getCubePosition().z: x_vector1.z;
		x_vector2.x = x_vector2.x < mvectx_cycleNodes[i]->getCubePosition().x?
			mvectx_cycleNodes[i]->getCubePosition().x: x_vector2.x;
		x_vector2.z = x_vector2.z < mvectx_cycleNodes[i]->getCubePosition().z?
			mvectx_cycleNodes[i]->getCubePosition().z: x_vector2.z;
	}
	ix_corner1 = x_vector1;
	ix_corner2 = x_vector2;
}

bool Cycle::containsPoint(Vector& ix_point)
{
	Vector x_vector1, x_vector2;
	//On trouve les points extremes du cycle pour trouver le carré englobant.
	getContainingSquare(x_vector1, x_vector2);

	//On aligne un point d'extrémité du carré en x.
	Vector x_checkpoint = x_vector1;
	x_checkpoint.x = ix_point.x;

	int n_nbLineCollision = 0;

	//On regarde sur chaque point en z pour trouver les croisements avec les lignes du cycle.
	while( x_checkpoint.z != ix_point.z )
	{
		++x_checkpoint.z;
		for(unsigned int i(0), cycleNodesSize(mvectx_cycleNodes.size()); i < cycleNodesSize; ++i)
		{
			if(x_checkpoint.x == mvectx_cycleNodes[i]->getCubePosition().x
				&& x_checkpoint.z == mvectx_cycleNodes[i]->getCubePosition().z)
				++n_nbLineCollision;
		}
	}

	if(n_nbLineCollision % 2 == 1)
		//Le point est dans le cycle.
		return true;
	else
		//Le point n'est pas dans le cycle.
		return false;
}