#include "Mac10.h"
#include "../../Rendering/Mesh/Mesh.h"
#include "../Unit.h"
#include <time.h>
#include <algorithm>
#include "../UnitHandler.h"
#include "../../Terrain/Terrain.h"
#include "../../Position.h"

Mac10::Mac10()
{
	mp_mesh = Mesh::getMesh("../../../graphique/mesh/mac10wip_low.obj", GEOM_PASS_SHADER);
	mp_unit = 0;

	m_rechargeSpeed = 0.5f;

	for(int i(0); i < 2; ++i)
		m_canDestroyTerrain[i] = false;

	for(int i(0); i < 2; ++i)
		m_rateOfFire[i] = 8.0f;

	for(int i(0); i < 2; ++i)
		m_damage[i] = 10.0f;

	m_maxBackupAmmo = 200;
	m_currentBackupAmmo = 100;

	m_maxAmmo = 20;
	m_currentAmmo = 10;

	m_horizontalRecoil = 5;
	m_verticalRecoil = 10;

	m_primaryFireTimer = Timer(1/m_rateOfFire[0] * 1000, 1/m_rateOfFire[0] * 1000);
	m_secondaryFireTimer = Timer(1/m_rateOfFire[1] * 1000, 1/m_rateOfFire[1] * 1000);
	m_rechargeTimer = Timer(0, m_rechargeSpeed * 1000);

	m_verticalInterpolator = Interpolator(&mp_unit->m_phi, 0.0f);

	m_horizontalInterpolator = Interpolator(&mp_unit->m_theta, 0.0f);

	m_zoom = 20;
}
Mac10::Mac10(const Mac10& i_mac10)
:Weapon(i_mac10)
{
}
Mac10::~Mac10()
{

}

void Mac10::animate(Uint32 i_timestep)
{
	float nbSeconds((float)i_timestep/1000.0f);


	//Si la souris est enfoncé, effectue un tire secondaire
	if(!m_isReloading)
	{
		m_rechargeTimer.setCurrentTime(0);

		m_primaryFireTimer.increment(i_timestep);
		m_secondaryFireTimer.increment(i_timestep);
		if(m_isShooting[2])
		{
			m_viewAngleInterpolator.setExtraTargetValue(-m_zoom);
		}
		else
		{
			m_rViewAngleInterpolator.addToTargetValue(-m_viewAngleInterpolator.getExtraValue());
			m_viewAngleInterpolator.reset();
		}
		//Si la souris est enfoncé, on effectue un tire
		if(m_isShooting[0] && m_currentAmmo > 0)
		{
			if(m_primaryFireTimer.doAction())
			{
				primaryFire(m_damage[0]);
			}
			m_verticalInterpolator.interpolate(i_timestep);
			m_horizontalInterpolator.interpolate(i_timestep);
		}
		else
		{
			m_nbBulletShot = 0;
			m_verticalInterpolator.reset();
			m_horizontalInterpolator.reset();
		}
	}
	else
	{
		reload(i_timestep);
	}
	

	m_viewAngleInterpolator.interpolate(i_timestep);
	m_rViewAngleInterpolator.interpolate(i_timestep);


	if(mp_unit != 0)
	{
		Matrix baseTransform;
		baseTransform.loadIdentity();
		baseTransform.translate(mp_unit->getPosition().x, mp_unit->getPosition().y + mp_unit->m_hitBox.getDimension(HEIGHT), mp_unit->getPosition().z);

		Vector* leftVect(mp_unit->getLeft());
		baseTransform.rotate(-mp_unit->m_phi, leftVect->x, leftVect->y, leftVect->z);
		baseTransform.rotate(-mp_unit->m_theta + 180.0f, 0.0f, 1.0f, 0.0f);
		baseTransform.translate(0.0f, -0.06f, 0.0f);
		baseTransform.translate(-1.0f, 0.0f, 0.0f);
		baseTransform.rotate(4.0f, 0.0f, 1.0f, 0.0f);
		baseTransform.translate(0.8f, 0.0f, 0.0f);
		baseTransform.scale(0.2f);
		m_renderInformation.setBaseTransform(baseTransform);
	}
}

void Mac10::draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)
{
	mp_mesh->draw(ik_projection, i_modelview, ipk_camera);
}

void Mac10::primaryFire(float i_damage)
{
	--m_currentAmmo;
	float recoil((rand() % m_verticalRecoil) * (m_nbBulletShot/10.0f));
	m_verticalInterpolator.addToTargetValue(recoil);
	m_verticalInterpolator.setInterpolationSpeed(0.03f * m_nbBulletShot/10.0f);

	recoil = ((rand() % 2)?1:-1) * (rand() % m_horizontalRecoil) * (m_nbBulletShot/10.0f);
	m_horizontalInterpolator.addToTargetValue(recoil);
	m_horizontalInterpolator.setInterpolationSpeed(0.02f * m_nbBulletShot/10.0f);

	damageUnit(i_damage);
	++m_nbBulletShot;
}

void Mac10::damageUnit(float i_damage)
{
	Vector sourceVect(mp_unit->getPosition());
	Vector targetVect(sourceVect + (mp_unit->getTarget() - sourceVect) * 20);

	Terrain* p_terrain(Terrain::getInstance());
	Vector3I sourceCube(Terrain::positionToCube(sourceVect));
	Vector3I targetCube(Terrain::positionToCube(targetVect));

	Vector direction(targetVect - sourceVect);
	direction.normalize();

	float tDeltaX(abs(SIDE/direction.x));
	float tDeltaY(abs(SIDE/direction.y));
	float tDeltaZ(abs(SIDE/direction.z));

	int editPositionX;
	int editPositionY;
	int editPositionZ;

	if(sourceVect.x < targetVect.x)
		editPositionX = 1;
	else
		editPositionX = -1;

	if(sourceVect.y < targetVect.y)
		editPositionY = 1;
	else
		editPositionY = -1;

	if(sourceVect.z < targetVect.z)
		editPositionZ = 1;
	else
		editPositionZ = -1;

	float tMaxX(abs(((sourceCube.x * SIDE + HALFSIDE * editPositionX) - sourceVect.x)/direction.x));
	float tMaxY(abs(((sourceCube.y * SIDE + HALFSIDE * editPositionY) - sourceVect.y)/direction.y));
	float tMaxZ(abs(((sourceCube.z * SIDE + HALFSIDE * editPositionZ) - sourceVect.z)/direction.z));

	Unit* unitToDamage(0);

	HitBox hitBoxCube(HALFSIDE, HALFSIDE, HALFSIDE);
	Vector center;

	Position position;
	do
	{
		position.x = sourceCube.x;
		position.y = sourceCube.y;
		position.z = sourceCube.z;
		if(!p_terrain->isAir(position.x, position.y, position.z))
		{
			center.x = sourceCube.x * SIDE;
			center.y = sourceCube.y * SIDE;
			center.z = sourceCube.z * SIDE;
			hitBoxCube.setPosition(center);

			if(hitBoxCube.rayCollision(sourceVect, targetVect))
			{
				unitToDamage = 0;
				goto UnitFound;
			}
		}

		//if(p_terrain->isUnitAtPosition(position))
		//{
		//	std::vector<Unit*> unitList(p_terrain->getUnitAtPosition(position));

		//	for(int i(0), unitListSize(unitList.size()); i < unitListSize; ++i)
		//	{
		//		if(unitList[i] != mp_unit)
		//		{
		//			if(unitList[i]->m_hitBox.rayCollision(sourceVect, targetVect))
		//			{
		//				unitToDamage = unitList[i];
		//				goto UnitFound;
		//			}
		//		}
		//	}
		//}

		if(tMaxX < tMaxY) 
		{
			if(tMaxX < tMaxZ) 
			{
				sourceCube.x = sourceCube.x + editPositionX;
				tMaxX = tMaxX + tDeltaX;
			} 
			else 
			{
				sourceCube.z = sourceCube.z + editPositionZ;
				tMaxZ = tMaxZ + tDeltaZ;
			}
		} 
		else 
		{
			if(tMaxY < tMaxZ) 
			{
				sourceCube.y = sourceCube.y + editPositionY;
				tMaxY = tMaxY + tDeltaY;
			}
			else 
			{
				sourceCube.z = sourceCube.z + editPositionZ;
				tMaxZ = tMaxZ + tDeltaZ;
			}
		}
	} 
	while(((sourceCube.x <= targetCube.x && editPositionX == 1) || (sourceCube.x > targetCube.x && editPositionX == -1)) && 
		  ((sourceCube.y <= targetCube.y && editPositionY == 1) || (sourceCube.y > targetCube.y && editPositionY == -1)) && 
		  ((sourceCube.z <= targetCube.z && editPositionZ == 1) || (sourceCube.z > targetCube.z && editPositionZ == -1)) );

	UnitFound:

	UnitHandler* p_unitHandler(UnitHandler::getInstance());
	if(unitToDamage != 0)
		if(unitToDamage->damage(i_damage))
			p_unitHandler->removeUnit(unitToDamage->m_teamID, unitToDamage->m_unitID);			
}

void Mac10::secondaryFire(float i_damage)
{

}

Item* Mac10::getCopy()
{
	return new Mac10(*this);
}