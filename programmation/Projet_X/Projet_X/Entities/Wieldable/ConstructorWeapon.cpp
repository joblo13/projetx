#include "ConstructorWeapon.h"
#include "../../Rendering/Mesh/Mesh.h"
#include "../../HitBox.h"
#include "../../Terrain/Terrain.h"
#include "../Unit.h"
#include "../UnitHandler.h"

#include "../../GUI/GUIHandler.h"
#include "../../GUI/MaterialSelectionGUI.h"
#include "../../Input.h"

ConstructorWeapon::ConstructorWeapon()
{
	for(int i(0); i < 2; ++i)
		m_canDestroyTerrain[i] = true;

	mp_mesh = Mesh::getMesh("../../../graphique/mesh/mac10wip_low.obj", GEOM_PASS_SHADER);
	mp_unit = 0;

	m_rechargeSpeed = 0.5f;

	m_rateOfFire[0] = 10.0f;
	m_rateOfFire[1] = 1.0f;

	m_damage[0] = 100.f;
	m_damage[1] = 100.f;

	m_maxBackupAmmo = 200;
	m_currentBackupAmmo = 100;

	m_maxAmmo = 20;
	m_currentAmmo = 10;

	m_horizontalRecoil = 0;
	m_verticalRecoil = 0;

	for(int i(0); i < 3; ++i)
		m_isShooting[i] = false;
	m_isReloading = false;

	m_primaryFireTimer = Timer(1/m_rateOfFire[0] * 1000, 1/m_rateOfFire[0] * 1000);
	m_secondaryFireTimer = Timer(1/m_rateOfFire[1] * 1000, 1/m_rateOfFire[1] * 1000);
}

ConstructorWeapon::ConstructorWeapon(const ConstructorWeapon& i_constructorWeapon)
:Weapon(i_constructorWeapon)
{

}

ConstructorWeapon::~ConstructorWeapon()
{

}

void ConstructorWeapon::animate(Uint32 i_timestep)
{
	Input* p_input(Input::getInstance());
	if(p_input->getKeyPress("thirdGunFunction") && p_input->getInputState() != CONSOLE_INPUT)
	{
		GUIHandler::getInstance()->toggleVisibility(MATERIAL_SELECTION_LAYOUT);
	}
	else
	{
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




		float nbSeconds((float)i_timestep/1000.0f);
		float secondsPerShot[2];
		for(int i(0); i < 2; ++i)
			secondsPerShot[i] = (1/m_rateOfFire[i]);

		//Si la souris est enfoncé, on crée un cube
		if(!m_isReloading)
		{
			m_primaryFireTimer.increment(i_timestep);
			m_secondaryFireTimer.increment(i_timestep);

			if(m_isShooting[2])
			{
				if(((const MaterialSelectionGUI*)GUIHandler::getInstance()->getLayout(MATERIAL_SELECTION_LAYOUT))->getCurrentCubeType() == 0)
					repairCube(m_damage[1] * (float)i_timestep/1000.f);
				else
				{
					if(m_secondaryFireTimer.doAction())
						createCube(((const MaterialSelectionGUI*)GUIHandler::getInstance()->getLayout(MATERIAL_SELECTION_LAYOUT))->getCurrentCubeType(), m_damage[1], m_damage[1]);
				}
			}
			else
			{
				//Si la souris est enfoncé, on detruit un cube
				if(m_isShooting[0])
				{
					if(m_primaryFireTimer.doAction())
						damageCube(m_damage[0] * (float)i_timestep/1000.f);
				}
			}
		}
	}
}

void ConstructorWeapon::draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)
{
	mp_mesh->draw(ik_projection, i_modelview, ipk_camera);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void createCube()
///
/// Fonction permettant de créer un cube selon la direction de la caméra
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void ConstructorWeapon::createCube(int i_cubeType, float i_hitpoint, float i_maxHitpoint)
{
	HitBox hitBoxCube(HALFSIDE, HALFSIDE, HALFSIDE);
	Vector center;
	Terrain* p_terrain(Terrain::getInstance());
	Vector3I cubePosition(Terrain::positionToCube(mp_unit->getPosition()));
	UnitHandler* p_unitHandler(UnitHandler::getInstance());

	int editPositionX;
	int editPositionY;
	int editPositionZ;

	//On approxime la direction du cube
	if(mp_unit->getPosition().x < mp_unit->getTarget().x)
		editPositionX = 1;
	else
		editPositionX = -1;

	if(mp_unit->getPosition().y < mp_unit->getTarget().y)
		editPositionY = 1;
	else
		editPositionY = -1;

	if(mp_unit->getPosition().z < mp_unit->getTarget().z)
		editPositionZ = 1;
	else
		editPositionZ = -1;

	//On test tout les cubes dans la zone 
	for(int i = cubePosition.x; 
		(editPositionX == 1)?(i < cubePosition.x + CHUNKLENGTH/3 * editPositionX):(i > cubePosition.x + CHUNKLENGTH/3 * editPositionX);
		i += editPositionX)

		for(int j = cubePosition.y; 
			(editPositionY == 1)?(j < cubePosition.y + CHUNKLENGTH/3 * editPositionY):(j > cubePosition.y + CHUNKLENGTH/3 * editPositionY); 
			j += editPositionY)

			for(int k = cubePosition.z; 
				(editPositionZ == 1)?(k < cubePosition.z + CHUNKLENGTH/3 * editPositionZ):(k > cubePosition.z + CHUNKLENGTH/3 * editPositionZ); 
				k += editPositionZ)
			{
				//Lorsque l'on trouve un cube qui n'est pas de l'air, on regarde si le rayon entre en collision avec le cube
				if(!p_terrain->isAir(i, j, k))
				{
					center.x = i * SIDE;
					center.y = j * SIDE;
					center.z = k * SIDE;
					hitBoxCube.setPosition(center);
					int cubeLocation = hitBoxCube.rayCollisionSide(mp_unit->getPosition(), mp_unit->getPosition() + (mp_unit->getTarget() - mp_unit->getPosition())*5);

					//S'il entre en collision, on trouve la position du nouveau cube et on le crée
					switch(cubeLocation)
					{
						case BOTTOM:
							hitBoxCube.setPosition(Terrain::cubeToPosition(Vector3I(i, j - 1, k)));
							if(!mp_unit->m_hitBox.collision(hitBoxCube))
							{
								if(p_unitHandler->getRessources(mp_unit->m_teamID, HUMAN_CUBES) >= 1)
								{
									if(Terrain::getInstance()->createCube(i, j - 1, k, i_cubeType, i_hitpoint, i_maxHitpoint))
										p_unitHandler->removeRessources(mp_unit->m_teamID, HUMAN_CUBES, 1);
								}
							}
							return;
						case TOP:
							hitBoxCube.setPosition(Terrain::cubeToPosition(Vector3I(i, j + 1, k)));
							if(!mp_unit->m_hitBox.collision(hitBoxCube))
							{
								if(p_unitHandler->getRessources(mp_unit->m_teamID, HUMAN_CUBES) >= 1)
								{
									if(Terrain::getInstance()->createCube(i, j + 1, k, i_cubeType, i_hitpoint, i_maxHitpoint))
										p_unitHandler->removeRessources(mp_unit->m_teamID, HUMAN_CUBES, 1);
								}
							}
							return;
						case LEFT:
							hitBoxCube.setPosition(Terrain::cubeToPosition(Vector3I(i + 1, j, k)));
							if(!mp_unit->m_hitBox.collision(hitBoxCube))
							{
								if(p_unitHandler->getRessources(mp_unit->m_teamID, HUMAN_CUBES) >= 1)
								{
									if(Terrain::getInstance()->createCube(i + 1, j, k, i_cubeType, i_hitpoint, i_maxHitpoint))
										p_unitHandler->removeRessources(mp_unit->m_teamID, HUMAN_CUBES, 1);
								}
							}
							return;
						case RIGHT:
							hitBoxCube.setPosition(Terrain::cubeToPosition(Vector3I(i - 1, j, k)));
							if(!mp_unit->m_hitBox.collision(hitBoxCube))
							{
								if(p_unitHandler->getRessources(mp_unit->m_teamID, HUMAN_CUBES) >= 1)
								{
									if(Terrain::getInstance()->createCube(i - 1, j, k, i_cubeType, i_hitpoint, i_maxHitpoint))
										p_unitHandler->removeRessources(mp_unit->m_teamID, HUMAN_CUBES, 1);
								}
							}
							return;
						case FRONT:
							hitBoxCube.setPosition(Terrain::cubeToPosition(Vector3I(i, j, k - 1)));
							if(!mp_unit->m_hitBox.collision(hitBoxCube))
							{
								if(p_unitHandler->getRessources(mp_unit->m_teamID, HUMAN_CUBES) >= 1)
								{
									if(Terrain::getInstance()->createCube(i, j, k - 1, i_cubeType, i_hitpoint, i_maxHitpoint))
										p_unitHandler->removeRessources(mp_unit->m_teamID, HUMAN_CUBES, 1);
								}
							}
							return;
						case BACK:
							hitBoxCube.setPosition(Terrain::cubeToPosition(Vector3I(i, j, k + 1)));
							if(!mp_unit->m_hitBox.collision(hitBoxCube))
							{
								if(p_unitHandler->getRessources(mp_unit->m_teamID, HUMAN_CUBES) >= 1)
								{
									if(Terrain::getInstance()->createCube(i, j, k + 1, i_cubeType, i_hitpoint, i_maxHitpoint))
										p_unitHandler->removeRessources(mp_unit->m_teamID, HUMAN_CUBES, 1);
								}
							}
							return;
						default:
							;
					}
					
				}
			}

}

void ConstructorWeapon::damageCube(float i_damage)
{
	Terrain* p_terrain(Terrain::getInstance());

	std::vector<Vector3I> listOfCubeToDamage(p_terrain->getCollision(mp_unit->getPosition() + (mp_unit->getTarget() - mp_unit->getPosition()) * RENDERING_DISTANCE_NEAR,
												  mp_unit->getPosition() + (mp_unit->getTarget() - mp_unit->getPosition()) * RENDERING_DISTANCE_FAR, true));
	if(listOfCubeToDamage.size() == 0)
		return;

	listOfCubeToDamage[0];

	if(listOfCubeToDamage[0].x >= 0)
	{
		//On ajoute une vitesse de descente minime pour pouvoir calculé si l'on doit tomber
		if(p_terrain->damageCube(listOfCubeToDamage[0].x, listOfCubeToDamage[0].y, listOfCubeToDamage[0].z, i_damage) <= 0)
		{
			UnitHandler::getInstance()->addRessources(mp_unit->m_teamID, HUMAN_CUBES, 1);
		}
	}
}

Item* ConstructorWeapon::getCopy()
{
	return new ConstructorWeapon(*this);
}

void ConstructorWeapon::repairCube(float i_damage)
{
	Terrain* p_terrain(Terrain::getInstance());

	std::vector<Vector3I> listOfCubeToDamage(p_terrain->getCollision(mp_unit->getPosition() + (mp_unit->getTarget() - mp_unit->getPosition()) * RENDERING_DISTANCE_NEAR,
												  mp_unit->getPosition() + (mp_unit->getTarget() - mp_unit->getPosition()) * RENDERING_DISTANCE_FAR, true));
	if(listOfCubeToDamage.size() == 0)
		return;

	listOfCubeToDamage[0];

	if(listOfCubeToDamage[0].x >= 0)
	{
		if(m_isChangedShooting[2])
			p_terrain->upgradeCube(listOfCubeToDamage[0].x, listOfCubeToDamage[0].y, listOfCubeToDamage[0].z);
		p_terrain->repairCube(listOfCubeToDamage[0].x, listOfCubeToDamage[0].y, listOfCubeToDamage[0].z, i_damage);
	}
}