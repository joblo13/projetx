#ifndef CYCLE_H
#define CYCLE_H

#include <vector>

class BuildingNode;
class Vector;

class Cycle
{
public:
	Cycle( std::vector<BuildingNode*> ivectx_cycleNodes );
	virtual ~Cycle();
	bool containsPoint(Vector& ix_point);
	void getContainingSquare(Vector& ix_corner1, Vector& ix_corner2);
	
private:
	std::vector<BuildingNode*> mvectx_cycleNodes;
};

#endif //CYCLE_H