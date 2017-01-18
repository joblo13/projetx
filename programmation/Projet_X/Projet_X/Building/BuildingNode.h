#ifndef BUILDING_NODE_H
#define BUILDING_NODE_H

#include <vector>
#include "../Math/Vector.h"

//Classe représentant un emplacement associé à une partie d'un mur d'un bâtiment à construire
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
	//Vrai si le noeud fait partie d'une pièce fermée
	bool mb_isInCycle;
};

#endif //BUILDING_NODE_H