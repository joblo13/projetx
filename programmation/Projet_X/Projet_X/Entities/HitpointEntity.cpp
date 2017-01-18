#include "HitpointEntity.h"

HitpointEntity::HitpointEntity(float i_maxHitpoint, float i_maxArmor)
:m_hitPoint(i_maxHitpoint), m_maxHitpoint(i_maxHitpoint),
m_armor(0), m_maxArmor(i_maxArmor)
{

}

HitpointEntity::~HitpointEntity()
{

}

float HitpointEntity::getHealthPercent()
{
	return m_hitPoint/m_maxHitpoint;
}

float HitpointEntity::getArmorPercent()
{
	if(m_maxArmor == 0.f)
		return 0.f;
	return m_armor/m_maxArmor;
}

int HitpointEntity::getHitpoint()
{
	return m_hitPoint;
}

void HitpointEntity::setMaxHitpoint(float i_maxHitpoint, bool i_setHitpoint)
{
	if(i_setHitpoint)
		m_hitPoint = i_maxHitpoint;
	m_maxHitpoint = i_maxHitpoint;
}

void HitpointEntity::setMaxArmor(float i_maxArmor, bool i_setArmor)
{
	if(i_setArmor)
		m_hitPoint = i_maxArmor;
	m_maxHitpoint = i_maxArmor;
}
