#ifndef BUILDING_NODE_H
#define BUILDING_NODE_H

#include <vector>
#include "../Math/Vector.h"

//Classe repr�sentant un emplacement associ� � une partie d'un mur d'un b�timent � construire
class BuildingNode
{
public:
	BuildingNode(Vector& x_cubePos);
	virtual ~BuildingNode();
	std::string toString();

	Vector getCubePosition();

	void setIsInCycle(bool ib_isInCycle);
	bool getIsInCycle() const;
	
private:
	Vector mx_cubePos;
	//Vrai si le noeud fait partie d'une pi�ce ferm�e
	bool mb_isInCycle;
};

#endif //BUILDING_NODE_H