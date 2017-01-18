#include "BuildingNode.h"



BuildingNode::BuildingNode(Vector& x_cubePos)
{
	mx_cubePos = x_cubePos;
	mb_isInCycle = false;
}


BuildingNode::~BuildingNode()
{
}

std::string BuildingNode::toString()
{
	char text[128];
	sprintf_s(text, "%i,%i", (int)mx_cubePos.x, (int)mx_cubePos.z);
	return text;
}

Vector BuildingNode::getCubePosition()
{
	return mx_cubePos;
}

void BuildingNode::setIsInCycle(bool ib_isInCycle)
{
	mb_isInCycle = ib_isInCycle;
}


bool BuildingNode::getIsInCycle() const
{
	return mb_isInCycle;
}