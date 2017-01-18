#include "Weapon.h"

#include "../../Math/Matrix.h"
#include "../../Camera/Camera.h"
#include "../Unit.h"
#include "../Item.h"
#include "../../Rendering/Mesh/Mesh.h"

Weapon::Weapon()
:mp_mesh(0),

m_rechargeSpeed(0.0f),

m_maxBackupAmmo(0),
m_currentBackupAmmo(0),

m_maxAmmo(0),
m_currentAmmo(0),

m_horizontalRecoil(0),
m_verticalRecoil(0),

m_isReloading(false),

m_nbBulletShot(0),

m_primaryFireTimer(0, 0),
m_secondaryFireTimer(0, 0),
m_rechargeTimer(0, 0),

m_verticalInterpolator(0, 0),

m_horizontalInterpolator(0, 0),

m_viewAngle(DEFAULT_VIEW_ANGLE),
m_viewAngleInterpolator(&m_viewAngle, 0.1), 
m_rViewAngleInterpolator(&m_viewAngle, 0.1), 
m_zoom(0),
m_renderInformation(this)
{
	for(int i(0); i < 2; ++i)
		m_damage[i] = 0.f;

	for(int i(0); i < 2; ++i)
		m_rateOfFire[i] = 0.0f;

	for(int i(0); i < 3; ++i)
		m_isShooting[i] = false;

	for(int i(0); i < 3; ++i)
		m_isChangedShooting[i] = false;
}
Weapon::Weapon(const Weapon& i_weapon)
:Item(i_weapon),

mp_mesh(i_weapon.mp_mesh),

m_renderInformation(i_weapon.m_renderInformation),


m_rechargeSpeed(i_weapon.m_rechargeSpeed),		

m_verticalRecoil(i_weapon.m_verticalRecoil),
m_horizontalRecoil(i_weapon.m_horizontalRecoil),

m_zoom(i_weapon.m_zoom),

//Munition
m_maxBackupAmmo(i_weapon.m_maxBackupAmmo),
m_currentBackupAmmo(i_weapon.m_currentBackupAmmo),

m_maxAmmo(i_weapon.m_maxAmmo),
m_currentAmmo(i_weapon.m_currentAmmo),


//Variable de gestion	
m_isReloading(i_weapon.m_isReloading),
m_viewAngle(i_weapon.m_viewAngle),
m_nbBulletShot(i_weapon.m_nbBulletShot),
	
//timer
m_primaryFireTimer(i_weapon.m_primaryFireTimer),
m_secondaryFireTimer(i_weapon.m_secondaryFireTimer),
m_rechargeTimer(i_weapon.m_rechargeTimer),

//interpolateur
m_verticalInterpolator(i_weapon.m_verticalInterpolator),

m_horizontalInterpolator(i_weapon.m_horizontalInterpolator),

m_viewAngleInterpolator(i_weapon.m_viewAngleInterpolator),
m_rViewAngleInterpolator(i_weapon.m_viewAngleInterpolator)
{	
	for(int i(0); i < 2; ++i)
		m_rateOfFire[i] = i_weapon.m_rateOfFire[i];

	for(int i(0); i < 2; ++i)
		m_damage[i] = i_weapon.m_damage[i];

	for(int i(0); i < 3; ++i)
		m_isShooting[i] = i_weapon.m_isShooting[i];

	for(int i(0); i < 2; ++i)
		m_canDestroyTerrain[i] = i_weapon.m_canDestroyTerrain[i];

	m_viewAngleInterpolator.setvalueToModify(&m_viewAngle);
	m_rViewAngleInterpolator.setvalueToModify(&m_viewAngle);
}
Weapon::~Weapon()
{

}

void Weapon::reload(Uint32 i_timestep)
{
	m_rechargeTimer.increment(i_timestep);
	if(m_rechargeTimer.doAction())
	{
		int ammoToTransfer(min(m_maxAmmo - m_currentAmmo, m_currentBackupAmmo));
		m_currentBackupAmmo -= ammoToTransfer;
		m_currentAmmo += ammoToTransfer;
		m_isReloading = false;
	}
}

void Weapon::addElement()
{
	mp_mesh->addToRenderInfoList(m_renderInformation);
}

void Weapon::removeElement()
{
	mp_mesh->removeFromRenderInfoList(m_renderInformation);
}

void Weapon::setUnit(Unit* ip_unit)
{
	Item::setUnit(ip_unit);
	if(mp_unit != 0)
	{
		m_verticalInterpolator.setvalueToModify(&mp_unit->m_phi);
		m_horizontalInterpolator.setvalueToModify(&mp_unit->m_theta);
	}
}

float Weapon::getTerrainDamage()
{
	for(int i(0); i < 2; ++i)
	{
		if(m_canDestroyTerrain[i])
			return m_damage[i];
	}
	return 0;
}

void Weapon::setIsShooting(int i_index, bool i_isShooting, bool i_isChangedShooting)
{
	m_isShooting[i_index] = i_isShooting;
	m_isChangedShooting[i_index] = i_isChangedShooting;
}