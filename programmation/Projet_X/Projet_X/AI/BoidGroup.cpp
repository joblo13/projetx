#include "BoidGroup.h"

#include "../Entities/Unit.h"
#include "../Entities/UnitHandler.h"
#include "../Math/Vector3I.h"

BoidGroup::BoidGroup(int i_groupID)
:m_groupID(i_groupID)
{

}

BoidGroup::~BoidGroup()
{
}

void BoidGroup::addBoid(Unit* ip_unit)
{
	m_boidList.push_back(ip_unit);
	ip_unit->setBoidGroup(m_groupID);
}

bool BoidGroup::removeBoid(Unit* ip_unit)
{
	for(int i(0), boidListSize(m_boidList.size()); i < boidListSize; ++i)
		if(ip_unit == m_boidList[i])
		{
			m_boidList[i] = m_boidList[boidListSize - 1];
			m_boidList.pop_back();
			return m_boidList.size() == 0;
		}
	return false;
}

void BoidGroup::update()
{
	m_center.x = 0.f;
	m_center.y = 0.f;
	m_center.z = 0.f;

	int boidListSize(m_boidList.size());
	for(int i(0); i < boidListSize; ++i)
	{
		m_center += m_boidList[i]->getPosition();

	}
	m_center /= boidListSize;
}

void BoidGroup::setGroupID(int i_groupID)
{
	m_groupID = i_groupID;
	for(int i(0), boidListSize(m_boidList.size()); i < boidListSize; ++i)
	{
		m_boidList[i]->setBoidGroup(m_groupID);	
	}
}

void BoidGroup::applyBoidBehaviors(Uint32 i_timestep)
{
	for(int i(0), boidListSize(m_boidList.size()); i < boidListSize; ++i)
	{
		m_boidList[i]->radarScan();
		m_boidList[i]->boidBehaviors(i_timestep);
	}
}

void BoidGroup::findPath(const std::vector<Vector3I>& i_endPosition)
{
	UnitHandler* p_unitHandler(UnitHandler::getInstance());
	
	for(int i(0), boidListSize(m_boidList.size()); i < boidListSize; ++i)
	{
		p_unitHandler->resetActionList(m_boidList[i]->m_teamID, m_boidList[i]->m_unitID);
		p_unitHandler->moveAStarAction(m_boidList[i]->m_teamID, m_boidList[i]->m_unitID, i_endPosition[i]);
	}

}