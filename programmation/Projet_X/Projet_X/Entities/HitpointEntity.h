#ifndef HITPOINT_ENTITY_H
#define HITPOINT_ENTITY_H

class HitpointEntity
{
	public:
		HitpointEntity(float i_maxHitpoint = 100, float i_maxArmor = 0);
		~HitpointEntity();
		float getHealthPercent();
		float getArmorPercent();
		int getHitpoint();
		void setMaxHitpoint(float i_maxHitpoint, bool i_setHitpoint = false);
		void setMaxArmor(float i_maxArmor, bool i_setArmor = false);
	protected:
		float m_hitPoint;
		float m_maxHitpoint;
		float m_armor;
		float m_maxArmor;
};

#endif //HITPOINT_ENTITY_H
