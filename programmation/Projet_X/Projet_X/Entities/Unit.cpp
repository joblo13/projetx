#include "Unit.h"

#include "../AI/Actions/Action.h"
#include "../Camera/Camera.h"
#include "../Math/Matrix.h"
#include "Wearable/Armor.h"
#include "Wieldable/Weapon.h"
#include "../Terrain/Terrain.h"
#include "../Math/Vector.h"
#include "../HitBox.h"
#include "../Position.h"
#include "../Shaders/Shader.h"
#include "ItemFactory.h"
#include "../Rendering/Mesh/Mesh.h"
#include "UnitHandler.h"
#include "../AI/BoidGroup.h"
#include "../SceneOpenGL.h"

#include "Bullet/btBulletDynamicsCommon.h"
#include "../GUI/GUIHandler.h"
#include "../GUI/MapGUI.h"


#include "BulletDynamics/Character/btKinematicCharacterController.h"

#include "../Debugger/ProgTools.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn Unit()
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Unit::Unit(Mesh* ip_mesh, HitpointBarMesh* ip_hitpointBarMesh, WhiteCircle* ip_whiteCircles)
:m_currentWeapon(0), m_isFalling(true),
//Physique
m_timeSinceLastJump(0),
m_isBoosting(false),
m_speedBonus(1),
m_character(0),
m_ghostObject(0),
m_boidGroupID(-1),
m_renderInformation(this),
m_selectorRenderInformation(this),
m_hitpointBarRenderInformation(this),
m_isSelected(false),
mp_mesh(ip_mesh),
mp_hitpointBarMesh(ip_hitpointBarMesh),
mp_whiteCircles(ip_whiteCircles),
m_hasMoved(true)
{
	for(int i(0); i < 4 ; ++i)
		mpa_wearable[i] = 0;

	for(int i(0); i < 10 ; ++i)
		mpa_wieldable[i] = 0;	
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Unit()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Unit::~Unit()
{
	removeElement();
	for(int i(0); i < 10 ; ++i)
	{
		if(mpa_wieldable[i] != 0)
			delete mpa_wieldable[i];
	}
	
	for(int i(0); i < NUM_ARMOR ; ++i)
	{
		if(mpa_wieldable[i] != 0)
			delete mpa_wearable[i];
	}

	//On détruit tout les buts
	while(!mp_targetList.empty())
	{
		delete mp_targetList.back();
		mp_targetList.pop_back();
	}

	exitPhysics();
}
void Unit::animate(Uint32 i_timestep)
{
	Matrix baseTransform;
	baseTransform.loadIdentity();

	baseTransform.translate(m_position.x, m_position.y - m_hitBox.getDimension(HEIGHT), m_position.z);
	m_renderInformation.setTranslation(Vector(m_position.x, m_position.y - m_hitBox.getDimension(HEIGHT), m_position.z));
	baseTransform.scale(m_hitBox.getDimension(HEIGHT));

	baseTransform.rotate(90.0f, 0.0f, 1.0f, 0.0f);
	baseTransform.rotate(-m_theta, 0.0f, 1.0f, 0.0f);
	m_renderInformation.setRotation(-m_theta, Vector(0.0f, 1.0f, 0.0f));

	m_renderInformation.setBaseTransform(baseTransform);

	m_selectorRenderInformation.setBaseTransform(m_renderInformation.getBaseTransform());


	//On calcule le temps total depuis le début de l'animation
	m_renderInformation.m_animationInfo.m_totalAnimationTimeSecond += ((float)i_timestep)/1000.0f;

	//On effectue la transformation sur les bones selon le temps
	mp_mesh->boneTransformBaked(this);

	updatePositionWithPhysics();

	//On effectue les actions
	updateCubeIn();

	if(mp_targetList.size() != 0)
		(*--mp_targetList.end())->executeAction(this, i_timestep);
	else if(!SceneOpenGL::getInstance()->getCamera()->isUnitUsed(this))
		stopMoving();

	animateWeapon(i_timestep);
}
void Unit::stopMoving()
{
	m_walkingDirection.setX(0.f);
	m_walkingDirection.setY(0.f);
	m_walkingDirection.setZ(0.f);
	m_character->setWalkDirection(m_walkingDirection);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void move(const Vector & ik_deplacement)
///
/// Fonction gérant le mouvement sur les trois axes en considérant les collisions
/// sur le plan x/z
/// 
/// @param[in] ik_deplacement : vecteur décrivant le déplacement
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Unit::move( const Vector& ik_deplacement, bool i_isJumping, Uint32 i_timestep )
{
	Vector leftVect(ik_deplacement);
	leftVect.crossProduct(Vector(0.f, 1.f, 0.f));

	m_theta = std::acos((Vector(1.f, 0.f, 0.f) * Vector(ik_deplacement.x, 0.f, ik_deplacement.z).normalize())) * (180.f/MATH_PI);
	if(ik_deplacement.z < 0)
		m_theta = 360.f - m_theta;

	moveFPS(ik_deplacement, leftVect, 0, 1, false, i_isJumping, i_timestep);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void moveFPS(const Vector& ik_walkingDirection, const Vector& ik_left, int i_sideDir, int i_frontDir, bool i_speedBoost, bool i_isJumping);
///
/// Fonction gérant le mouvement sur les trois axes en considérant les collisions
/// sur le plan x/z
/// 
/// @param[in] ik_walkingDirection : vecteur décrivant le déplacement avant
/// @param[in] ik_left : vecteur décrivant le déplacement de côté
/// @param[in] i_sideDir : -1, 0 ou 1 pour droite, fixe ou gauche
/// @param[in] i_frontDir : -1, 0 ou 1 pour recule, fixe ou avance
/// @param[in] i_speedBoost : true si on a un boost de vitesse
/// @param[in] i_isJumping : true si on saute
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Unit::moveFPS( const Vector& ik_walkingDirection, const Vector& ik_left, int i_sideDir, int i_frontDir, bool i_speedBoost, bool i_isJumping, Uint32 i_timestep)
{
	btDynamicsWorld* p_dynamicWorld(Terrain::getInstance()->getDynamicWorld());
	if (p_dynamicWorld != 0)
	{
		int maxSimSubSteps = 2;

		btTransform xform;
		xform = m_ghostObject->getWorldTransform ();

		btVector3 test2 = xform.getOrigin();
		char test[100];
		sprintf(test, "move(%f, %f, %f)", test2.getX(), test2.getY(), test2.getZ());
		GUIHandler::getInstance()->addToDebugHUD(test);

		Vector walkingDirection((ik_walkingDirection * i_frontDir + ik_left * i_sideDir));
		btVector3 btWalkingDirection = btVector3(walkingDirection.x, walkingDirection.y, walkingDirection.z);

		btScalar walkSpeed = m_speed * i_timestep * (i_speedBoost?m_speedBonus:1.f);

		if(!m_character->onGround())
			walkSpeed /=2;

		if(btWalkingDirection.getX() != 0.f || btWalkingDirection.getY() != 0.f || btWalkingDirection.getZ() != 0.f)
		{
			m_walkingDirection = btWalkingDirection * walkSpeed;
		}
		else
		{
			m_walkingDirection.setX(0.f);
			m_walkingDirection.setY(0.f);
			m_walkingDirection.setZ(0.f);
		}

		m_character->setWalkDirection(m_walkingDirection);
		if(i_isJumping)
			m_character->jump();
	}
}


void Unit::switchWeapon(int i_index)
{
	assert(i_index >= 0 && i_index < 10);

	if(mpa_wieldable[m_currentWeapon] != 0)
		mpa_wieldable[m_currentWeapon]->removeElement();

	m_currentWeapon = i_index;

	if(mpa_wieldable[m_currentWeapon] != 0)
		mpa_wieldable[m_currentWeapon]->addElement();
}

void Unit::animateWeapon(Uint32 i_timestep)
{
	if(mpa_wieldable[m_currentWeapon] != 0)
		mpa_wieldable[m_currentWeapon]->animate(i_timestep);
}
void Unit::drawWeapon(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)
{
	if(mpa_wieldable[m_currentWeapon] != 0)
		mpa_wieldable[m_currentWeapon]->draw(ik_projection, i_modelview, ipk_camera);
}

void Unit::setIsShooting(int i_index, bool i_isShooting, bool i_isChangedShooting)
{
	if(mpa_wieldable[m_currentWeapon] != 0)
		mpa_wieldable[m_currentWeapon]->setIsShooting(i_index, i_isShooting, i_isChangedShooting);
}

void Unit::setIsReloading(bool i_isReloading)
{
	if(mpa_wieldable[m_currentWeapon] != 0)
		mpa_wieldable[m_currentWeapon]->setIsReloading(i_isReloading);
}

int Unit::getCurrentAmmo()
{
	if(mpa_wieldable[m_currentWeapon] != 0)
		return mpa_wieldable[m_currentWeapon]->getCurrentAmmo();
	return 0;
}
int Unit::getCurrentBackupAmmo()
{
	if(mpa_wieldable[m_currentWeapon] != 0)
		return mpa_wieldable[m_currentWeapon]->getCurrentBackupAmmo();
	return 0;
}

float Unit::getViewAngle()
{
	if(mpa_wieldable[m_currentWeapon] != 0)
		return mpa_wieldable[m_currentWeapon]->getViewAngle();
	return DEFAULT_VIEW_ANGLE;
}

bool Unit::damage(float i_damage)
{
	m_hitPoint -= i_damage;
	if(m_hitPoint <= 0)
		return true;

	return false;
}

void Unit::updateCubeIn()
{
	Terrain* p_terrain(Terrain::getInstance());

	float width(m_hitBox.getDimension(WIDTH));
	float height(m_hitBox.getDimension(HEIGHT));
	float length(m_hitBox.getDimension(LENGTH));
}

Vector* const  Unit::getLeft()
{
	return &m_left;
}

Vector* const  Unit::setLeft()
{
	static const Vector k_up(0,1,0);
	m_left = k_up.crossProduct(m_target - m_position);
    m_left.normalize();
	return &m_left;
}

void Unit::addElement()
{
	mp_mesh->addToRenderInfoList(m_renderInformation);

	if(m_isSelected)
	{
		mp_hitpointBarMesh->addToRenderInfoList(m_hitpointBarRenderInformation);
	}

	if(mpa_wieldable[m_currentWeapon] != 0)
		mpa_wieldable[m_currentWeapon]->addElement();
}

void Unit::removeElement()
{
	mp_mesh->removeFromRenderInfoList(m_renderInformation);

	if(m_isSelected)
	{
		mp_hitpointBarMesh->removeFromRenderInfoList(m_hitpointBarRenderInformation);
	}

	if(mpa_wieldable[m_currentWeapon] != 0)
		mpa_wieldable[m_currentWeapon]->removeElement();
}

void Unit::setItem(ItemType i_itemType, ItemInformation* i_itemInformation)
{
	if(i_itemType < WEAPON0_ITEMS)
	{
		if(mpa_wieldable[i_itemType] == 0)
			delete mpa_wieldable[i_itemType];

		mpa_wearable[i_itemType] = (Armor*)ItemFactory::getInstance()->createItem(i_itemInformation);
		mpa_wearable[i_itemType]->setUnit(this);
	}
	else
	{
		if(mpa_wieldable[i_itemType - WEAPON0_ITEMS] == 0)
			delete mpa_wieldable[i_itemType - WEAPON0_ITEMS];
		mpa_wieldable[i_itemType - WEAPON0_ITEMS] = (Weapon*)ItemFactory::getInstance()->createItem(i_itemInformation);
		mpa_wieldable[i_itemType - WEAPON0_ITEMS]->setUnit(this);
		switchWeapon(m_currentWeapon);
	}
}
const ItemInformation* Unit::getItem(ItemType i_itemType)
{
	if(i_itemType < WEAPON0_ITEMS)
	{	
		if(mpa_wearable[i_itemType] == 0)
			return 0;
		else
			return mpa_wearable[i_itemType]->getItemInformation();
	}
	else
	{
		if(mpa_wieldable[i_itemType - WEAPON0_ITEMS] == 0)
			return 0;
		else
			return mpa_wieldable[i_itemType - WEAPON0_ITEMS]->getItemInformation();
	}
}

//Physique

void Unit::initPhysicsAndAddToWorld()
{
	Terrain* p_terrain(Terrain::getInstance());
	p_terrain->getDynamicWorld()->getDispatchInfo().m_allowedCcdPenetration=0.0001f;


	btTransform startTransform;
	startTransform.setIdentity ();
	startTransform.setOrigin(btGetPosition());


	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setWorldTransform(startTransform);
	p_terrain->getBroadphaseInterface()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

	btConvexShape* colShape = new btBoxShape(btVector3(m_hitBox.getDimension(WIDTH),
	m_hitBox.getDimension(HEIGHT),m_hitBox.getDimension(LENGTH)));
	m_collisionShapes.push_back(colShape);
	m_ghostObject->setCollisionShape (colShape);
	m_ghostObject->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);

	btScalar stepHeight = btScalar(0.35);
	m_character = new btKinematicCharacterController(m_ghostObject, colShape, stepHeight);

	m_character->setMaxSlope(btRadians(90.0));

	///only collide with static for now (no interaction with dynamic objects)
	p_terrain->getDynamicWorld()->addCollisionObject(m_ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter | COL_WALL);

	p_terrain->getDynamicWorld()->addAction(m_character);
}

void Unit::reloadPhysic()
{
	btDynamicsWorld* p_dynamicWorld(Terrain::getInstance()->getDynamicWorld());
	if(m_ghostObject != 0)
	{
		p_dynamicWorld->addCollisionObject(m_ghostObject,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter | COL_WALL);
		p_dynamicWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(m_ghostObject->getBroadphaseHandle(), p_dynamicWorld->getDispatcher());
	}

	if(m_character != 0)
	{
		p_dynamicWorld->addAction(m_character);
		m_character->reset (p_dynamicWorld);
	}
}

void Unit::exitPhysics()
{
	//cleanup in the reverse order of creation/initialization

	//delete collision shapes
	for (int j=0;j<m_collisionShapes.size();j++)
	{
		btCollisionShape* shape = m_collisionShapes[j];
		delete shape;
		shape = 0;		
	}
	m_collisionShapes.resize(0);




	if(m_character != 0)
	{
		delete m_character;
		m_character = 0;
	}

	if(m_ghostObject != 0)
	{
		delete m_ghostObject;
		m_ghostObject = 0;
	}
}

void Unit::updatePositionWithPhysics()
{
	if(!m_ghostObject)
		return;

	btVector3 pos = m_ghostObject->getWorldTransform().getOrigin();

	updateMapTarget();

	setPosition(Vector(pos.getX(), pos.getY(), pos.getZ()));

	setTarget(m_position + Vector(cos(m_theta * MATH_PI/180.f), 0.f, sin(m_theta * MATH_PI/180.f)), true);
}

void Unit::updateMapTarget()
{
	std::vector<PixelPosition> mapDirection;
	getMapDirection(mapDirection);

	MapGUI* p_mapGUI(((MapGUI*)GUIHandler::getInstance()->getLayout(MAP_LAYOUT)));
	for(int i(0), endMapDirection(mapDirection.size()); i != endMapDirection; ++i)
		p_mapGUI->addPixelToUpdate(mapDirection[i].first / MAP_PIXEL_SIZE, mapDirection[i].second / MAP_PIXEL_SIZE);
}

void Unit::setTarget(const Vector& i_target, bool i_skipMapUpgrade)
{
	bool hasTurned(m_target.x != i_target.x || m_target.z != i_target.z || m_position.x != m_lastPosition.x || m_position.z != m_lastPosition.z);

	if(hasTurned && !i_skipMapUpgrade)
	{
		updateMapTarget();
	}

	m_target = i_target;
}
void Unit::setPosition(const Vector& i_position)
{
	m_hasMoved = m_position.x != i_position.x || m_position.z != i_position.z;
	if(m_hasMoved)
	{
		std::hash_set<PixelPosition> mapShape;
		getMapShape(mapShape);

		MapGUI* p_mapGUI(((MapGUI*)GUIHandler::getInstance()->getLayout(MAP_LAYOUT)));
		for(std::hash_set<PixelPosition>::iterator it(mapShape.begin()), endMapShape(mapShape.end()); it != endMapShape; ++it)
			p_mapGUI->addPixelToUpdate(it->first / MAP_PIXEL_SIZE, it->second / MAP_PIXEL_SIZE);
	}


	m_lastPosition = m_position;
	m_position = i_position;

	if(m_character != 0)
	{
		btTransform transform;
		transform.setOrigin(btVector3(i_position.x, i_position.y, i_position.z));
		m_character->getGhostObject()->setWorldTransform(transform);
	}
}

const Vector& Unit::getPosition()
{
	return m_position;
}

btVector3 Unit::btGetPosition()
{
	btVector3 conversion;
	conversion.setX(m_position.x);
	conversion.setY(m_position.y);
	conversion.setZ(m_position.z);

	return conversion;
}

void Unit::boidBehaviors(Uint32 i_timestep)
{

	float coherencyWeight = 0;
	float separationWeight = 0.2f;


	UnitHandler* p_unitHandler(UnitHandler::getInstance());

	Vector relativePos;
	Vector coherency(0.f, 0.f, 0.f);
	if(m_boidGroupID != -1)
		coherency = p_unitHandler->getBoidGroupCenter(m_boidGroupID) - getPosition();
	Vector separation(0.f, 0.f, 0.f);
	

	for(int i(0), neighborsSize(m_neighbors.size()); i < neighborsSize; ++i)
	{
		if (m_neighbors[i] != this) 
		{
 			relativePos = (getPosition() - m_neighbors[i]->getPosition());
			separation += relativePos / (relativePos * relativePos);
		}
	}

	Vector boidBehaviorForce((coherency * coherencyWeight) + (separation * separationWeight));
	boidBehaviorForce.y = 0;

	btVector3 btWalkingDirection = btVector3(boidBehaviorForce.x, boidBehaviorForce.y, boidBehaviorForce.z);

	m_walkingDirection += btWalkingDirection * m_speed * i_timestep;

	m_character->setWalkDirection(m_walkingDirection);
	
}

void Unit::radarScan()
{
	{
		std::vector<Unit*> emptyNeighbors;
		m_neighbors.swap(emptyNeighbors);
	}

	const float distance(0.5f);
	Unit* p_unit(0);
	UnitHandler* p_unitHandler(UnitHandler::getInstance());
	for(int i(0), teamCount(p_unitHandler->getTeamCount()); i < teamCount; ++i)
	{
		for(int j(0), unitCount(p_unitHandler->getUnitCount(i)); j < unitCount; ++j)
		{
			p_unit = p_unitHandler->getUnit(i, j);
			if(distance > (p_unit->getPosition() - getPosition()).length() && p_unit != this)
				m_neighbors.push_back(p_unit);
		}
	}
}

void Unit::updateHitpointBar(const Vector& i_cameraForward)
{
	Matrix baseTransform;
	baseTransform.loadIdentity();
	Vector translation(m_renderInformation.getTranslation());
	baseTransform.translate(translation.x, translation.y + 2 * m_hitBox.getDimension(HEIGHT) + 0.25, translation.z);
	
	//baseTransform.rotate(90.0f, 0.0f, 1.0f, 0.0f);



	baseTransform.rotate(90.f - i_cameraForward.getPhi(), 0.0f, 1.0f, 0.0f);

	Vector cameraWalking(i_cameraForward.x, 0.f, i_cameraForward.z);
	cameraWalking = cameraWalking.crossProduct(Vector(0.f, 1.f, 0.f));
	baseTransform.rotate(-i_cameraForward.getTheta(), 1, 0, 0);
	baseTransform.scale(0.5f, 0.10, 0.5f);
	m_hitpointBarRenderInformation.setBaseTransform(baseTransform);	
}

void Unit::setIsSelected(bool i_isSeleted)
{
	if(i_isSeleted != m_isSelected)
	{
		m_isSelected = i_isSeleted;

		if(m_isSelected)
		{
			mp_whiteCircles->addToRenderInfoList(m_selectorRenderInformation);
			mp_hitpointBarMesh->addToRenderInfoList(m_hitpointBarRenderInformation);
		}
		else
		{
			mp_whiteCircles->removeFromRenderInfoList(m_selectorRenderInformation);
			mp_hitpointBarMesh->removeFromRenderInfoList(m_hitpointBarRenderInformation);
		}
	}
}

void Unit::addAnimation(const std::string& i_animationPath)
{
	mp_mesh->addAnimation(i_animationPath);
}

void Unit::getMapShape(std::hash_set<PixelPosition>& i_mapShape)
{
	Vector3I center(Terrain::positionToCube(m_position * MAP_PIXEL_SIZE));
	int rayon(m_hitBox.getDimension(WIDTH));

	//On trace le contour du cercle(bresenham)
	int x,z,p;
	x = 0;
	z = rayon;
	PixelPosition pixelPosition(0, 0);
	pixelPosition.first = center.x + x;
	pixelPosition.second = center.z - z;
	i_mapShape.insert(pixelPosition);
	p = 3 - (2 * rayon);
	for(int x(0); x <= z ;x++)
	{
		if (p < 0)
			p=(p+(4*x)+6);
		else
			z=z-1;

		p = p + ((4 * (x - z) + 10));

		pixelPosition.first = center.x + x;
		pixelPosition.second = center.z - z;
		i_mapShape.insert(pixelPosition);
		pixelPosition.first = center.x - x;
		pixelPosition.second = center.z - z;
		i_mapShape.insert(pixelPosition);
		pixelPosition.first = center.x + x;
		pixelPosition.second = center.z + z;
		i_mapShape.insert(pixelPosition);
		pixelPosition.first = center.x - x;
		pixelPosition.second = center.z + z;
		i_mapShape.insert(pixelPosition);
		pixelPosition.first = center.x + z;
		pixelPosition.second = center.z - x;
		i_mapShape.insert(pixelPosition);
		pixelPosition.first = center.x-z;
		pixelPosition.second = center.z-x;
		i_mapShape.insert(pixelPosition);
		pixelPosition.first = center.x+z;
		pixelPosition.second = center.z+x;
		i_mapShape.insert(pixelPosition);
		pixelPosition.first = center.x-z;
		pixelPosition.second = center.z+x;
		i_mapShape.insert(pixelPosition);
	}
}

void Unit::getMapFullShape(std::vector<std::vector<PixelPosition>>& i_mapFullShape)
{
	std::hash_set<PixelPosition> mapShape;
	getMapShape(mapShape);

	Vector3I center(Terrain::positionToCube(m_position * MAP_PIXEL_SIZE));

	for(std::hash_set<PixelPosition>::iterator it(mapShape.begin()), itEnd(mapShape.end()); it != itEnd; ++it)
	{
		int currentLine(i_mapFullShape.size());
		i_mapFullShape.resize(currentLine + 1);
		i_mapFullShape[currentLine].push_back(*it);

		if(it->first < center.x)
		{
			for(PixelPosition pixelPositon(it->first + 1, it->second); ; ++pixelPositon.first)
			{
				if(mapShape.find(pixelPositon) == mapShape.end())
					i_mapFullShape[currentLine].push_back(pixelPositon);
				else
					break;
			}
		}
	}
}

//http://fr.wikipedia.org/wiki/Algorithme_de_trac%C3%A9_de_segment_de_Bresenham

void traceSegment(int x1, int y1, int x2, int y2, std::vector<PixelPosition>& o_result)
{
	int dx;
	int dy;
	PixelPosition pixelPosition(0, 0);
  
	if( (dx = x2 - x1) != 0)
	{
		if( dx > 0 )
		{
			if( (dy = y2 - y1) != 0 )
			{
				if( dy > 0 )
				{
					// vecteur oblique dans le 1er quadran 
					if( dx >= dy )
					{
						// vecteur diagonal ou oblique proche de l’horizontale, dans le 1er octant
						int e ;
						dx = (e = dx) * 2 ; 
						dy = dy * 2 ;  // e est poif(tif
						for(;;)
						{  // déplacements horizontaux
							pixelPosition.first = x1;
							pixelPosition.second = y1;
							o_result.push_back(pixelPosition);

							if( (x1 = x1 + 1) == x2)
								break;
							if( (e = e - dy) < 0 )
							{
								y1 = y1 + 1 ;  // déplacement diagonal
								e = e + dx ;
							}
						}
					}
					else
					{
						// vecteur oblique proche de la verticale, dans le 2nd octant
						int e ;
						dy = (e = dy) * 2 ; 
						dx = dx * 2 ;  // e est poif(tif
						for(;;)
						{  // déplacements verticaux
							pixelPosition.first = x1;
							pixelPosition.second = y1;
							o_result.push_back(pixelPosition);
							if( (y1 = y1 + 1) == y2)
								break;
							if( (e = e - dx) < 0 )
							{
								x1 = x1 + 1 ;  // déplacement diagonal
								e = e + dy ;
							}
						}
					}        
				}
				else
				{  // dy < 0 (et dx > 0)
					// vecteur oblique dans le 4e cadran      
					if( dx >= -dy )
					{
						// vecteur diagonal ou oblique proche de l’horizontale, dans le 8e octant
						int e ;
						dx = (e = dx) * 2 ; 
						dy = dy * 2 ;  // e est poif(tif
						for(;;)
						{  // déplacements horizontaux
							pixelPosition.first = x1;
							pixelPosition.second = y1;
							o_result.push_back(pixelPosition);
							if( (x1 = x1 + 1) == x2)
								break;
							if( (e = e + dy) < 0 )
							{
								y1 = y1 - 1 ;  // déplacement diagonal
								e = e + dx ;
							}
						}
					}
					else
					{  // vecteur oblique proche de la verticale, dans le 7e octant
						int e ;
						dy = (e = dy) * 2 ; 
						dx = dx * 2 ;  // e est négatif
						for(;;)
						{  // déplacements verticaux
							pixelPosition.first = x1;
							pixelPosition.second = y1;
							o_result.push_back(pixelPosition);
							if( (y1 = y1 - 1) == y2)
								break;
							if( (e = e + dx) > 0 )
							{
								x1 = x1 + 1 ;  // déplacement diagonal
								e = e + dy ;
							}
						}
					}      
				}
			}
			else
			{  // dy = 0 (et dx > 0)     
				// vecteur horizontal vers la droite
				do
				{
					pixelPosition.first = x1;
					pixelPosition.second = y1;
					o_result.push_back(pixelPosition);
				}
				while((x1 = x1 + 1) != x2) ;     
			}
		}
		else
		{  // dx < 0
			if( (dy = y2 - y1) != 0 )
			{
				if( dy > 0 )
				{
					// vecteur oblique dans le 2nd quadran      
					if( -dx >= dy )
					{
						// vecteur diagonal ou oblique proche de l’horizontale, dans le 4e octant
						int e ;
						dx = (e = dx) * 2 ; 
						dy = dy * 2 ;  // e est négatif
						for(;;)
						{  // déplacements horizontaux
							pixelPosition.first = x1;
							pixelPosition.second = y1;
							o_result.push_back(pixelPosition);
							if( (x1 = x1 - 1) == x2)
								break;
							if( (e = e + dy) >= 0 )
							{
								y1 = y1 + 1 ;  // déplacement diagonal
								e = e + dx ;
							}
						}
					}
					else
					{
						// vecteur oblique proche de la verticale, dans le 3e octant
						int e ;
						dy = (e = dy) * 2 ; 
						dx = dx * 2 ;  // e est poif(tif
						for(;;)
						{  // déplacements verticaux
							pixelPosition.first = x1;
							pixelPosition.second = y1;
							o_result.push_back(pixelPosition);
							if( (y1 = y1 + 1) == y2)
								break;
							if( (e = e + dx) <= 0 )
							{
								x1 = x1 - 1 ;  // déplacement diagonal
								e = e + dy ;
							}
						}
					}        
				}
				else
				{  // dy < 0 (et dx < 0)
					// vecteur oblique dans le 3e cadran        
					if( dx <= dy )
					{
						// vecteur diagonal ou oblique proche de l’horizontale, dans le 5e octant
						int e ;
						dx = (e = dx) * 2 ; 
						dy = dy * 2 ;  // e est négatif
						for(;;)
						{  // déplacements horizontaux
							pixelPosition.first = x1;
							pixelPosition.second = y1;
							o_result.push_back(pixelPosition);
							if( (x1 = x1 - 1) == x2)
								break;
							if( (e = e - dy) >= 0 )
							{
								y1 = y1 - 1 ;  // déplacement diagonal
								e = e + dx;
							}
						}
					}
					else
					{  // vecteur oblique proche de la verticale, dans le 6e octant
						int e ;
						dy = (e = dy) * 2 ; 
						dx = dx * 2 ;  // e est négatif
						for(;;)
						{  // déplacements verticaux
							pixelPosition.first = x1;
							pixelPosition.second = y1;
							o_result.push_back(pixelPosition);
							if( (y1 = y1 - 1) == y2)
								break;
							if( (e = e - dx) >= 0 )
							{
								x1 = x1 - 1 ;  // déplacement diagonal
								e = e + dy ;
							}
						}
					}   
				}
			}
			else
			{  // dy = 0 (et dx < 0) 
				// vecteur horizontal vers la gauche
				do
				{
					pixelPosition.first = x1;
					pixelPosition.second = y1;
					o_result.push_back(pixelPosition);
				}
				while((x1 = x1 - 1) != x2) ;
			}
		}
	}
	else  // dx = 0
	{
		if( (dy = y2 - y1) != 0 )
		{
			if( dy > 0 )
			{ 
				// vecteur vertical croissant
				do
				{
					pixelPosition.first = x1;
					pixelPosition.second = y1;
					o_result.push_back(pixelPosition);
				}
				while((y1 = y1 + 1) != y2) ;
			}
			else
			{  // dy < 0 (et dx = 0)  
				// vecteur vertical décroissant
				do
				{
					pixelPosition.first = x1;
					pixelPosition.second = y1;
					o_result.push_back(pixelPosition);
				}
				while ((y1 = y1 - 1) != y2) ;    
			}
		}
	}
	// le pixel final (x2, y2) n’est pas tracé.
}

void Unit::getMapDirection(std::vector<PixelPosition>& o_result)
{
	Vector3I center(Terrain::positionToCube(m_position * MAP_PIXEL_SIZE));
	Vector3I target(Terrain::positionToCube((m_position + (m_target - m_position).normalize()) * MAP_PIXEL_SIZE));

	traceSegment(center.x, center.z, target.x, target.z, o_result);
}