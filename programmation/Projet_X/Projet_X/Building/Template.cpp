#include "Template.h"

#include "../Terrain/Chunk.h"
#include "../Terrain/Terrain.h"
#include "../Rendering/RenderInformation.h"
#include "../Rendering/WhitePrismeInstanciable.h"
#include "../Entities/Unit.h"
#include "../Entities/Wieldable/Weapon.h"
#include "../Math/Vector.h"
#include "../Entities/UnitHandler.h"

#include "../Rendering/Mesh/HitpointBarMesh.h"

#include "../GUI/GUIHandler.h"

Template::Template(const Chunk* ikp_shape, bool i_useCustomShape, WhitePrismeInstanciable* ip_whitePrismeInstanciable, HitpointBarMesh* ip_hitpointBarMesh)
:HitpointEntity(100, 100), mkp_shape(ikp_shape), m_cubePosition(ikp_shape->getCubePosition()), m_currentBuildingMode(PREPARING_REMOVE), m_useCustomShape(i_useCustomShape),
m_renderInformation(this), m_hitpointBarRenderInformation(this), mp_whitePrismeInstanciable(ip_whitePrismeInstanciable), m_isSelected(false), mp_hitpointBarMesh(ip_hitpointBarMesh),
m_targetUpgradeLevel(0)
{
	Matrix matrix;
	matrix.loadIdentity();

	Vector translation(Terrain::cubeToPosition(m_cubePosition));
	
	matrix.translate(translation.x - HALFSIDE, translation.y - HALFSIDE, translation.z - HALFSIDE);
	matrix.scale(ikp_shape->getSizeX() * SIDE, ikp_shape->getSizeY() * SIDE, ikp_shape->getSizeZ() * SIDE);

	m_renderInformation.setBaseTransform(matrix);

	calculateMaxHitpoint();
	calculateMaxArmor();
}
Template::~Template()
{
	setIsSelected(false);
	if(m_useCustomShape)
		delete mkp_shape;
}
float Template::getDimension(int i_index)
{
	return Terrain::cubeToPosition(getCubeDimension(i_index));
}

int Template::getCubeDimension(int i_index)
{
	switch(i_index)
	{
		case WIDTH:
			return mkp_shape->getSizeX();
		case HEIGHT:
			return mkp_shape->getSizeY();
		case LENGTH:
			return mkp_shape->getSizeZ();
		default:
			assert(0);
	}
	assert(0);
	return -1;
}

Vector Template::getPosition()
{
	return Terrain::cubeToPosition(m_cubePosition);
}

const Vector3I& Template::getCubePosition()
{
	return m_cubePosition;
}

void Template::resetCurrentCube(Vector3I& i_currentCube)
{
	switch(m_currentBuildingMode)
	{
		case PREPARING_REMOVE:
			i_currentCube.x = 0.f;
			i_currentCube.y = mkp_shape->getSizeY() - 1.f;
			i_currentCube.z = 0.f;
			break;
		case PREPARING_ADD:
			i_currentCube.x = 0.f;
			i_currentCube.y = -1.f;
			i_currentCube.z = 0.f;
			break;
		case UPGRADING:
		case BUILDING:
			i_currentCube.x = 0.f;
			i_currentCube.y = 0.f;
			i_currentCube.z = 0.f;
			break;
	}
}
bool Template::nextCurrentCube(Vector3I& i_currentCube)
{
	int yDirection(1);
	switch(m_currentBuildingMode)
	{
		case PREPARING_REMOVE:
			yDirection = -1;
		break;
		case UPGRADING:
		case PREPARING_ADD:
		case BUILDING:
			yDirection = 1;
		break;
	}
	++i_currentCube.x;
	if(i_currentCube.x >= mkp_shape->getSizeX())
	{
		i_currentCube.x = 0.f;
		++i_currentCube.z;
		if(i_currentCube.z >= mkp_shape->getSizeZ())
		{
			i_currentCube.z = 0.f;
			i_currentCube.y += yDirection;
			bool endReached;
			switch(m_currentBuildingMode)
			{
			case PREPARING_REMOVE:
				endReached = i_currentCube.y < 0.f;
				break;
			case PREPARING_ADD:
				endReached = i_currentCube.y == 0;
				break;
			case UPGRADING:
			case BUILDING:
				endReached = i_currentCube.y >= mkp_shape->getSizeY();
				break;
			}
			if(endReached)
				return true;
		}
	}
	return false;
}
void Template::resetBuildingState()
{
	m_currentBuildingMode = PREPARING_REMOVE;
}


bool Template::build(Unit* ip_unit, Uint32 i_timestep)
{
	Terrain* p_terrain(Terrain::getInstance());
	Vector3I absolutCurrentCube;
	UnitHandler* p_unitHandler(UnitHandler::getInstance());

	Vector3I currentCube;

	resetCurrentCube(currentCube);
	

	bool actionMade(false);
	//On trouve une arme qui peut causer des dommage
	if(ip_unit->getCurrentWeapon()->getTerrainDamage() == 0.f)
	{
		ip_unit->switchWeapon(0);
		for(int i(0); i < 10; ++i)
		{
			Weapon* p_weapon(ip_unit->getCurrentWeapon());
			if(p_weapon != 0)
			{
				if(p_weapon->getTerrainDamage() != 0.f)
					break;
				else
					ip_unit->switchWeapon(i);
			}
			else
			{
				break;
			}
		}
	}
	//On calcule les dommages de l'arme
	float damage(ip_unit->getCurrentWeapon()->getTerrainDamage() * (float)i_timestep/1000.f);
	int hpRemains(0);
	//On continue tant qu'on a pas effectué une action
	while(!actionMade /*|| m_currentBuildingMode == DONE*/)
	{
		absolutCurrentCube = getTerrainAbsolutPosition(currentCube.x, currentCube.y, currentCube.z);
		//On regarde à quel étape on est rendu
		switch(m_currentBuildingMode)
		{
			case PREPARING_REMOVE:
				{
					//On cherche un cube qui doit être detruit
					int terrainType(p_terrain->getCubeType(absolutCurrentCube.x, absolutCurrentCube.y, absolutCurrentCube.z));
					int shapeType(mkp_shape->getCube(currentCube.x, currentCube.y, currentCube.z));


					bool densityMatch(shapeType<=0);
					if(!densityMatch)
					{
						float a_densityShape[8];
						float a_densityTerrain[8];
						p_terrain->getDensity(absolutCurrentCube.x, absolutCurrentCube.y, absolutCurrentCube.z, a_densityShape);
						mkp_shape->getDensity(currentCube.x, currentCube.y, currentCube.z, a_densityTerrain);
						densityMatch = true;
						for(int i(0); i < 8; ++i)
							if(a_densityShape[i] != a_densityTerrain[i])
							{
								densityMatch = false;
								break;
							}
					}

					if(shapeType != terrainType && 
						densityMatch &&
						p_terrain->isInLimit(absolutCurrentCube.x, absolutCurrentCube.y, absolutCurrentCube.z))
					{
						actionMade = true;
						//On endomage le cube
						hpRemains = p_terrain->damageCube(absolutCurrentCube.x, absolutCurrentCube.y, absolutCurrentCube.z, damage);
						//Si les hp sont plus petit que zéro, on doit detruire le cube et transférer les dommages
						if(hpRemains <= 0)
						{
							p_unitHandler->addRessources(ip_unit->m_teamID, HUMAN_CUBES, 1);
							//On transfère les dommages si l'on en a à transférer
							if(hpRemains != 0)
							{
								actionMade = false;
								damage = -hpRemains;
							}
							//On passe au prochain cube
							if(nextCurrentCube(currentCube))
							{
								//On passe à la prochaine étape lorsque l'on a fini d'itérer
								m_currentBuildingMode = PREPARING_ADD;
								damage = ip_unit->getCurrentWeapon()->getTerrainDamage() * (float)i_timestep/1000.f;
							}
						}
			
					}
					else
					{
						//On passe au prochain cube
						if(nextCurrentCube(currentCube))
						{
							//On passe à la prochaine étape lorsque l'on a fini d'itérer
							m_currentBuildingMode = PREPARING_ADD;
							damage = ip_unit->getCurrentWeapon()->getTerrainDamage() * (float)i_timestep/1000.f;
						}
					}
				}
				break;
			case PREPARING_ADD:
			{
				//On skip les cubes à l'exterieur des limites
				if(p_terrain->isInLimit(absolutCurrentCube.x, absolutCurrentCube.y, absolutCurrentCube.z))
				{

					//On recherche la hauteur du cube à modifier
					int i(0);
					//On descend jusqu'à ce que l'on trouve un cube qui n'est pas de l'air et que le dessus n'est pas rendable ou que le cube n'est pas plein
					while(p_terrain->isAir(absolutCurrentCube.x, absolutCurrentCube.y + i, absolutCurrentCube.z) || 
						p_terrain->getCubeHitpoint(absolutCurrentCube.x, absolutCurrentCube.y + i, absolutCurrentCube.z) != p_terrain->getCubeMaxHitpoint(absolutCurrentCube.x, absolutCurrentCube.y + i, absolutCurrentCube.z) || 
							(p_terrain->isSmoothTopRenderable(absolutCurrentCube.x, absolutCurrentCube.y + i, absolutCurrentCube.z) && !p_terrain->isFull(absolutCurrentCube.x, absolutCurrentCube.y + i, absolutCurrentCube.z)))
					{
						--i;
					}
					//Si l'on est pas au max, on remonte d'un coups pour modifier le bon cube
					if(i != 0)
						++i;
					

					//Si le cube a modifier est naturel
					if(!p_terrain->isNatural(absolutCurrentCube.x, absolutCurrentCube.y + i, absolutCurrentCube.z))
					{

						//Si le cube est de l'air, on créer un cube
						if(p_terrain->isAir(absolutCurrentCube.x, absolutCurrentCube.y + i, absolutCurrentCube.z))
						{
							actionMade = true;
							if(p_unitHandler->getRessources(ip_unit->m_teamID, HUMAN_CUBES) > 0)
							{
								p_terrain->createCube(absolutCurrentCube.x, absolutCurrentCube.y + i, absolutCurrentCube.z, -1, 1, 100);
								p_unitHandler->removeRessources(ip_unit->m_teamID, HUMAN_CUBES, 1);
							}
						}
						else
						{
							hpRemains = p_terrain->repairCube(absolutCurrentCube.x, absolutCurrentCube.y + i, absolutCurrentCube.z, damage);
							if(hpRemains != 0)
							{
								damage = hpRemains;
							}
							else
							{
								actionMade = true;
								damage = ip_unit->getCurrentWeapon()->getTerrainDamage() * (float)i_timestep/1000.f;
							}
						}
					}
					//On change de cube seulement quand on remonte jusqu'au cube le plus haut et qu'une action a été fait
					if(i == 0 && p_terrain->getCubeHitpoint(absolutCurrentCube.x, absolutCurrentCube.y + i, absolutCurrentCube.z) == p_terrain->getCubeMaxHitpoint(absolutCurrentCube.x, absolutCurrentCube.y + i, absolutCurrentCube.z))
					{
						//On change de mode lorsque l'on a fini les actions
						if(nextCurrentCube(currentCube))
							m_currentBuildingMode = BUILDING;
					}
				}
				else
				{
					//On change de mode lorsque l'on a fini les actions
					if(nextCurrentCube(currentCube))
						m_currentBuildingMode = BUILDING;
				}
			}
			break;
			case BUILDING:
			{
				//Si le materiel utilisé est different de celui déjà en place, on créer le cube
				if(mkp_shape->getCube(currentCube.x, currentCube.y, currentCube.z) != p_terrain->getCubeType(absolutCurrentCube.x, absolutCurrentCube.y, absolutCurrentCube.z) && 
					p_terrain->isInLimit(absolutCurrentCube.x, absolutCurrentCube.y, absolutCurrentCube.z))
				{
					actionMade = true;
					//On prend la densité de la shape
					float a_density[8];
					mkp_shape->getDensity(currentCube.x, currentCube.y, currentCube.z, a_density);
					//On crée le cube
					if(p_unitHandler->getRessources(ip_unit->m_teamID, HUMAN_CUBES) > 0)
					{
						p_terrain->createCube(absolutCurrentCube.x, absolutCurrentCube.y, absolutCurrentCube.z, mkp_shape->getCube(currentCube.x, currentCube.y, currentCube.z), 1, 100, a_density);
						p_unitHandler->removeRessources(ip_unit->m_teamID, HUMAN_CUBES, 1);
					}
			
				}
				else
				{
					GUIHandler::getInstance()->addToDebugHUD(damage);
					hpRemains = p_terrain->repairCube(absolutCurrentCube.x, absolutCurrentCube.y, absolutCurrentCube.z, damage);
					if(hpRemains != 0)
					{
						damage = hpRemains;
						if(nextCurrentCube(currentCube))
						{
							actionMade = true;
							m_currentBuildingMode = UPGRADING;
						}
					}
					else
					{
						actionMade = true;
						damage = ip_unit->getCurrentWeapon()->getTerrainDamage() * (float)i_timestep/1000.f;
					}
				}
			}
			break;
			case UPGRADING:
			{
				int renforcement((*p_terrain->getFullCube(absolutCurrentCube.x, absolutCurrentCube.y, absolutCurrentCube.z))[CUBE_RENFORCEMENT]);
				if(m_targetUpgradeLevel > renforcement)
				{
					actionMade = true;
					p_terrain->setRenforcement(absolutCurrentCube.x, absolutCurrentCube.y, absolutCurrentCube.z, ++renforcement);
					m_currentBuildingMode = BUILDING;
				}
				else
				{
					if(nextCurrentCube(currentCube))
					{
						m_currentBuildingMode = DONE;
						return true;
					}
				}
			}
			break;
		}
	}
	return false;
}

Vector3I Template::getTerrainAbsolutPosition(int i_x, int i_y, int i_z)
{
	return Vector3I(m_cubePosition.x + i_x, m_cubePosition.y + i_y, m_cubePosition.z + i_z);
}

Vector Template::getBottomCenterPosition()
{
	return Terrain::cubeToPosition(
		Vector3I(m_cubePosition.x + mkp_shape->getSizeX()/2, 
				 m_cubePosition.y + mkp_shape->getSizeY()/2, 
				 m_cubePosition.z + mkp_shape->getSizeZ()/2));
}

void Template::calculateCurrentHitpoint()
{
	m_hitPoint = 0.f;
	m_armor = 0.f;
	Terrain* p_terrain(Terrain::getInstance());

	for(int i(m_cubePosition.x), xEnd(i + mkp_shape->getSizeX()); i < xEnd; ++i)
		for(int j(m_cubePosition.y), yEnd(j + mkp_shape->getSizeY()); j < yEnd; ++j)
			for(int k(m_cubePosition.z), zEnd(k + mkp_shape->getSizeZ()); k < zEnd; ++k)
			{
				if(mkp_shape->getCube(i - m_cubePosition.x, j - m_cubePosition.y, k - m_cubePosition.z) != 0)
				{
					if(p_terrain->isInLimit(i, j, k))
					{
						const std::vector<int>* p_fullCube = p_terrain->getFullCube(i, j, k);
						if((*p_fullCube)[CUBE_TYPE] != 0)
							if((*p_fullCube)[CUBE_RENFORCEMENT] > 0)
							{
								m_hitPoint += p_terrain->getCubeMaxHitpoint(i, j, k);
								m_armor += p_terrain->getCubeHitpoint(i, j, k);
							}
							else
							{
								m_hitPoint += p_terrain->getCubeHitpoint(i, j, k);
							}
					}
				}
			}
}
void Template::calculateMaxHitpoint()
{
	m_maxHitpoint = 0;
	for(int i(0); i < mkp_shape->getSizeX(); ++i)
		for(int j(0); j < mkp_shape->getSizeY(); ++j)
			for(int k(0); k < mkp_shape->getSizeZ(); ++k)
				m_maxHitpoint += mkp_shape->getCubeMaxHitpoint(i, j, k);
}

void Template::calculateMaxArmor()
{
	m_maxArmor = 0;
	for(int i(0); i < mkp_shape->getSizeX(); ++i)
		for(int j(0); j < mkp_shape->getSizeY(); ++j)
			for(int k(0); k < mkp_shape->getSizeZ(); ++k)
				m_maxArmor += mkp_shape->getCubeMaxHitpoint(i, j, k);
}

void Template::setIsSelected(bool i_isSeleted)
{
	if(i_isSeleted != m_isSelected)
	{
		m_isSelected = i_isSeleted;

		if(m_isSelected)
		{
			mp_hitpointBarMesh->addToRenderInfoList(m_hitpointBarRenderInformation);
			mp_whitePrismeInstanciable->addToRenderInfoList(m_renderInformation);
		}
		else
		{
			mp_hitpointBarMesh->removeFromRenderInfoList(m_hitpointBarRenderInformation);
			mp_whitePrismeInstanciable->removeFromRenderInfoList(m_renderInformation);
		}
	}
}

bool Template::updateHitpointBar(const Vector& i_cameraForward)
{
	
	Matrix baseTransform;
	baseTransform.loadIdentity();
	Vector position(Terrain::cubeToPosition(m_cubePosition));
	baseTransform.translate(position.x + Terrain::cubeToPosition(mkp_shape->getSizeX())/2.f - 0.25f, 
							position.y + Terrain::cubeToPosition(mkp_shape->getSizeY()) + 0.25f, 
							position.z + Terrain::cubeToPosition(mkp_shape->getSizeZ())/2.f - 0.25f);
	

	baseTransform.rotate(90.f - i_cameraForward.getPhi(), 0.0f, 1.0f, 0.0f);

	Vector cameraWalking(i_cameraForward.x, 0.f, i_cameraForward.z);
	cameraWalking = cameraWalking.crossProduct(Vector(0.f, 1.f, 0.f));
	baseTransform.rotate(-i_cameraForward.getTheta(), 1, 0, 0);
	baseTransform.scale(0.75f, 0.1f, 0.5f);
	m_hitpointBarRenderInformation.setBaseTransform(baseTransform);	

	calculateCurrentHitpoint();
	if(m_hitPoint <= 0.f && m_currentBuildingMode == DONE)
		return true;
	return false;
}

HitBox Template::getHitBox(bool i_extended)
{
	HitBox hitBox;
	Vector templatePosition;
	if(i_extended)
	{
		templatePosition.x = Terrain::cubeToPosition(getCubePosition().x - 1);
		templatePosition.y = Terrain::cubeToPosition(getCubePosition().y - 1);
		templatePosition.z = Terrain::cubeToPosition(getCubePosition().z - 1);
	}
	else
	{
		templatePosition = Terrain::cubeToPosition(getCubePosition());
	}

	templatePosition.x += getDimension(WIDTH)/2;
	templatePosition.y += getDimension(HEIGHT)/2;
	templatePosition.z += getDimension(LENGTH)/2;

	if(i_extended)
	{
		templatePosition.x += 1;
		templatePosition.y += 1;
		templatePosition.z += 1;
	}

	hitBox.setPosition(templatePosition);
	if(i_extended)
	{	
		hitBox.setDimension(getDimension(WIDTH)/2 + 1, 
			getDimension(HEIGHT)/2 + 1, 
			getDimension(LENGTH)/2 + 1);
	}
	else
	{
		hitBox.setDimension(getDimension(WIDTH)/2, 
			getDimension(HEIGHT)/2, 
			getDimension(LENGTH)/2);
	}


	return hitBox;
}

bool Template::isPositionOccupied(const Vector3I& i_position)
{
	return m_workerUsedPosition.find(i_position) != m_workerUsedPosition.end();
}

void Template::addWorkerPosition(const Vector3I& i_position, Unit* ip_unit)
{
	m_workerUsedPosition.insert(std::pair<Vector3I, Unit*>(i_position, ip_unit));
}

void Template::removeWorkerPosition(Unit* ip_unit)
{
	for(std::hash_map<Vector3I, Unit*>::iterator it(m_workerUsedPosition.begin()), workerUsedPositionEnd(m_workerUsedPosition.end()); it != workerUsedPositionEnd; ++it)
		if(it->second == ip_unit)
		{
			m_workerUsedPosition.erase(it->first);
			if(m_workerUsedPosition.size() == 0)
				resetBuildingState();
			return;
		}
}

void Template::upgrade()
{
	++m_targetUpgradeLevel;
	if(m_targetUpgradeLevel > 1)
		m_targetUpgradeLevel = 1;
}