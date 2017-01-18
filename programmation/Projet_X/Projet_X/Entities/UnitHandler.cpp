#include "UnitHandler.h"	

#include "../Rendering/Mesh/Mesh.h"
#include "../Terrain/Terrain.h"
#include "Unit.h"
#include "House.h"
#include "../AI/AStar.h"
#include "../Shaders/Shader.h"
#include "../Camera/Camera.h"
#include "../Math/Vector3I.h"
#include "../AI/Node.h"
#include "Ressources/HumanRessources.h"
#include "../Position.h"
#include "../Input.h"
#include "ItemFactory.h"
#include "../Rendering/DeferredRenderer/DeferredRenderer.h"
#include "../Rendering/Texture.h"
#include "../Building/Template.h"
#include "../SceneOpenGL.h"
#include "../Building/TemplateManager.h"

//boid
#include "../Ai/BoidGroup.h"

//Armes
#include "Wieldable/Mac10.h"
#include "Wieldable/ConstructorWeapon.h"

//Action
#include "../AI/Actions/MoveAction.h"
#include "../AI/Actions/AddAction.h"
#include "../AI/Actions/DeleteAction.h"
#include "../AI/Actions/MoveAStarAction.h"
#include "../AI/Actions/BuildTemplateAction.h"

#define NOMINMAX
#define MAX_FALLING_SPEED 10
#define MAX_CLIMBING 0.1f
#define MIN_DISTANCE_FALLING 0.2f

//#define SELECTOR_RADIUS 0.3f
//#define SELECTOR_NUM_SEGMENT 20

UnitHandler* UnitHandler::mp_instance = 0;

//Fonction interne pour la physique
void myTickCallback(btDynamicsWorld *world, btScalar timeStep) {

	//UnitHandler* p_unitHandler(UnitHandler::getInstance());

	//int teamCount = p_unitHandler->getTeamCount();
	//int unitCount;
	//Unit* p_unit;

	////On itère pour tous les unités
	//for(int i(0); i < teamCount; ++i)
	//{
	//	unitCount = p_unitHandler->getUnitCount(i);
	//	for(int j(0); j < unitCount; ++j)
	//	{
	//		p_unit = p_unitHandler->getUnit(i,j);
	//		if(p_unit != 0)
	//		{	
	//			btRigidBody* p_rigidBody = p_unit->getPhysicBody();
	//			btVector3 velocity = p_rigidBody->getLinearVelocity();

	//			float tempY(velocity.getY());
	//			velocity.setY(0.f);

	//			btScalar speed = velocity.length();

	//			int maxSpeed = p_unit->getMaxSpeed();
	//			if(speed > maxSpeed) 
	//			{
	//				velocity *= maxSpeed/speed;
	//				velocity.setY(tempY);
	//				p_rigidBody->setLinearVelocity(velocity);
	//			}				
	//		}
	//	}
	//}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn UnitHandler(int i_teamCount)
///
/// Constructeur
/// 
/// @param[in] i_teamCount : Nombre d'équipe
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
UnitHandler::UnitHandler(int i_teamCount)
:mp_camera(0)
{
	mp_hitpointBarMesh = SceneOpenGL::getInstance()->getHitpointBarMesh();
	mp_unitList.resize(i_teamCount);

	m_freeSpace.resize(i_teamCount);

	mp_ressources.resize(i_teamCount);

	for(int i(0); i < i_teamCount; ++i)
		mp_ressources[i] = new HumanRessources();

	//Le terrain doit être créé en premier
	Terrain::getInstance()->getDynamicWorld()->setInternalTickCallback(myTickCallback);

	DeferredRenderer::getInstance()->addWhiteLine((Instanciable*)(&m_whiteCircles));
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~UnitHandler()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
UnitHandler::~UnitHandler()
{
	for(int i(0), boidGroupSize(mp_boidGroups.size()); i < boidGroupSize; ++i)
		delete mp_boidGroups[i];
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void addUnit(Unit* ip_unit)
///
/// Ajout d'une unité
/// 
/// @param[in] ip_unit : position de la caméra
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void UnitHandler::addUnit(Unit* ip_unit)
{
	//Si la liste des places vide est vide on ajoute à la fin
	if(m_freeSpace[ip_unit->m_teamID].size() == 0)
	{
		mp_unitList[ip_unit->m_teamID].push_back(ip_unit);
	}
	//Sinon, on ajoute à l'endroit libre
	else
	{
		mp_unitList[ip_unit->m_teamID][m_freeSpace[ip_unit->m_teamID].back()] = ip_unit;
		m_freeSpace[ip_unit->m_teamID].pop_back();
	}
	ip_unit->addElement();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void addHuman(int i_teamID, const Vector& i_position)
///
/// Constructeur
/// 
/// @param[in] i_teamID : index d'équipe
/// @param[in] i_position : position de départ
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void UnitHandler::addHuman(int i_teamID, const Vector3I& i_position)
{
	//Création de l'unité
	Unit* p_unit(new Unit(Mesh::getMesh("../../../graphique/mesh/test/boblampclean.md5mesh", GEOM_PASS_SHADER), mp_hitpointBarMesh, &m_whiteCircles));

	//Caractéristique d'un humain
	p_unit->addAnimation("../../../graphique/mesh/test/boblampclean.md5mesh");
	p_unit->m_theta = 0;
	p_unit->m_phi = 0;
	p_unit->m_hitBox = HitBox(HALFSIDE - 0.1f, HALFSIDE * 1.2f, HALFSIDE - 0.1f, p_unit->getPosition());
	p_unit->m_fallingSpeed = 0;
	p_unit->m_speed = 0.002f;
	p_unit->setMaxHitpoint(100.f, true);

	//Paramètre
	p_unit->m_teamID = i_teamID;
	p_unit->setPosition(Terrain::cubeToPosition(i_position));
	p_unit->m_lastPosition = p_unit->getPosition();

	ItemInformation* itemInformation( new ItemInformation());
	itemInformation->information = "A tool";
	itemInformation->name = "Spade";
	itemInformation->price[0] = 1;
	itemInformation->price[1] = 0;
	itemInformation->price[2] = 0;
	itemInformation->price[3] = 0;
	itemInformation->weight = 1;
	itemInformation->ID = 10;
	ItemFactory* p_itemFactory(ItemFactory::getInstance());
	p_itemFactory->addItem(*itemInformation, new ConstructorWeapon());
	p_unit->setItem(WEAPON1_ITEMS, itemInformation);

	//On l'ajoute dans la structure de données selon la place disponible
	if(m_freeSpace[i_teamID].size() == 0)
		p_unit->m_unitID = mp_unitList[i_teamID].size();
	else
		p_unit->m_unitID = m_freeSpace[i_teamID][m_freeSpace.size() - 1];


	itemInformation->information = "Gun";
	itemInformation->name = "Mac10";
	itemInformation->price[0] = 1;
	itemInformation->price[1] = 0;
	itemInformation->price[2] = 0;
	itemInformation->price[3] = 0;
	itemInformation->weight = 1;
	itemInformation->ID = 20;

	p_itemFactory->addItem(*itemInformation, new Mac10());
	p_unit->setItem(WEAPON0_ITEMS, itemInformation);

	//TODOJO load
	p_unit->initPhysicsAndAddToWorld();

	//On ajoute l'unité
	addUnit(p_unit);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void removeUnit(int i_teamID, int i_unitID)
///
/// Constructeur
/// 
/// @param[in] i_teamID : index de l'équipe
/// @param[in] i_unitID : index de l'unité
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void UnitHandler::removeUnit(int i_teamID, int i_unitID)
{
	//Si l'unité est la derniere à enlever on enleve l'unité
	if(mp_unitList[i_teamID].size() - 1 == i_unitID)
	{
		delete mp_unitList[i_teamID][mp_unitList[i_teamID].size() - 1];
		mp_unitList[i_teamID].pop_back();
	}
	//Sinon on conserve la positione pour la prochaine insertion
	else
	{
		m_freeSpace[i_teamID].push_back(i_unitID);
		delete mp_unitList[i_teamID][i_unitID];
		mp_unitList[i_teamID][i_unitID] = 0;
	}

}

/*////////////////////////////////////////////////////////////////////////
///
/// @fn void getRessources(int i_teamID, int i_unitID, int i_nbOfRessources)
///
/// Fonction permettant de creuser pour trouver des ressources
/// 
/// @param[in] i_teamID : index de l'équipe
/// @param[in] i_unitID : index de l'unité
/// @param[in] i_nbOfRessources : nombre de ressource a trouver
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void UnitHandler::getRessources(int i_teamID, int i_unitID, int i_nbOfRessources)
{

	Unit* p_unit(mp_unitList[i_teamID][i_unitID]);
	int holeWidth(8);
	int stairsNb(1);
	for(int i(1); i <= holeWidth; ++i)
	{
		stairsNb += i;
	}
	stairsNb *= 3;

	int nbOfRows = 8;

	int nbRessourcesCollected = 0;

	Terrain* p_terrain(Terrain::getInstance());
	while( nbRessourcesCollected < i_nbOfRessources )
	{
		//On chercher une place où creuser un trou avec le nombre de rangées trouvé
		Vector3I corner1, corner2;
		findFlatRectangleTerritory (p_unit, nbOfRows, holeWidth, corner1, corner2);

		int highestCube = p_terrain->getHighestCube((corner2.m_x - corner1.m_x)/2 + corner1.m_x, (corner2.m_z - corner1.m_z)/2 + corner1.m_z);
		startPathFind(i_teamID, i_unitID, Vector3I(corner1.m_x, highestCube, corner1.m_z));

		mp_houseList.push_back(new House(Vector3I((corner2.m_x - corner1.m_x)/2 + corner1.m_x, (Terrain::positionToCube(p_unit->m_position)).m_y - 2, (corner2.m_z - corner1.m_z)/2 + corner1.m_z)));

		bool widthIsInX;
		if(corner2.m_x - corner1.m_x == holeWidth)
		{
			//la largeur du trou suit l'axe des x
			widthIsInX = true;
		}
		else
		{
			//la largeur du trou suit l'axe des y
			widthIsInX = false;
		}

		//profondeur
		for(int i(0); i < holeWidth; ++i)
		{

			//rangée
			for(int j(0); j < nbOfRows; ++j)
			{
				//une case de rangée
				for(int k(0); k < holeWidth; ++k)
				{
					if( !( j < 3 && k < i ) )
					{
						Vector3I pos;
						pos = corner1;
						pos.m_y = highestCube;
						if(widthIsInX)
						{
							pos += Vector3I(j, -i, k);
						}
						else
						{
							pos += Vector3I(k, -i, j);
						}

						moveAction(i_teamID, i_unitID, Terrain::cubeToPosition(pos) );
						++nbRessourcesCollected;
						deleteAction(i_teamID, i_unitID, pos); 
						if( nbRessourcesCollected >= i_nbOfRessources )
							goto ENOUGH_RESSOURCES;
					}
				}
			}
		}
	}
	ENOUGH_RESSOURCES:
	return;
}*/

////////////////////////////////////////////////////////////////////////
///
/// @fn int testTerrain(const Vector& ik_centerCube, int i_nbRows, int i_nbColumns)
///
/// Constructeur
/// 
/// @param[in] ik_centerCube : le cube central
/// @param[in] i_nbRows : largeur en x
/// @param[in] i_nbColumns : longeur en z
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
int UnitHandler::testTerrain(const Vector3I& ik_centerCube, int i_nbRows, int i_nbColumns)
{	
	Terrain* p_terrain(Terrain::getInstance());

	//On trouve le coin inférieur
	Vector3I x_corner(ik_centerCube);
	x_corner.x -= i_nbRows/2;
	x_corner.z -= i_nbColumns/2;

	std::hash_map<int, int> ann_highestList;
	int n_average(0);
	int n_highestCube;

	//Pour chaque cube de l'aire, on calcule la différence avec la hauteur du centre
	for(int n_i(0); n_i < i_nbRows; ++n_i)
		for(int n_j(0); n_j < i_nbColumns; ++n_j)
		{
			n_highestCube = p_terrain->getHighestCube(x_corner.x + n_i, x_corner.z + n_j);
			ann_highestList[n_highestCube] += 1;
			n_average += n_highestCube;
		}
	//On calcule la moyenne
	n_average /= i_nbRows * i_nbColumns;

	//Si on trouve un -1, il y a un troue sans fonds
	if(ann_highestList.find(-1) != ann_highestList.end())
		return INT_MAX;

	//Sinon on calcule l'écart type
	int n_deviation(0);
	for(std::hash_map<int, int>::iterator itann_highestList(ann_highestList.begin()); itann_highestList != ann_highestList.end(); ++itann_highestList)
	{
		 n_deviation += abs(itann_highestList->first - n_average) * itann_highestList->second;
	}
	
	return n_deviation;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void findFlatRectangleTerritory (Unit* ipx_unit, int in_nbRows, int in_nbColumns, Vector3I& iox_corner1, Vector3I& iox_corner2)
///
/// Fonction permettant de choisir automatiquement un terrain pour construire un bâtiment
/// 
/// @param[in] ip_unit : position de la caméra
/// @param[in] i_nbRows : largeur du batiment (en x)
/// @param[in] i_nbColumns : longueur du batiment (en z)
/// @param[in] i_corner1 : premier coin de la zone trouvé
/// @param[in] i_corner2 : deuxième point de la zone trouvé
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void UnitHandler::findFlatRectangleTerritory (Unit* ip_unit, int i_nbRows, int i_nbColumns, Vector3I& i_corner1, Vector3I& i_corner2)
{
	//On démarre le generateur
	srand((unsigned int)time(NULL));

	int numHouse(mp_houseList.size());
	Vector3I x_centerCube[8];

	//On choisi une maison ou une position près de l'unité
	if(numHouse == 0)
		x_centerCube[0] = Terrain::positionToCube(ip_unit->getPosition());
	else
		x_centerCube[0] = mp_houseList[rand() % numHouse]->getPosition();

	Vector3I bestCenterCube;

	//On choisie la distance entre le centre et la prochaine maison
	int xShift = i_nbColumns + 2;
	int	zShift = i_nbRows + 2;

	//On défini les points à tester
	x_centerCube[1].x = (int)(x_centerCube[0].x + 1.5f * xShift);
	x_centerCube[1].z = (int)(x_centerCube[0].z - 1.5f * zShift);

	x_centerCube[2].x = (int)(x_centerCube[0].x - 1.5f * xShift);
	x_centerCube[2].z = (int)(x_centerCube[0].z + 1.5f * zShift);

	x_centerCube[3].x = (int)(x_centerCube[0].x + 1.5f * xShift);
	x_centerCube[3].z = (int)(x_centerCube[0].z + 1.5f * zShift);

	x_centerCube[4].x = (int)(x_centerCube[0].x + xShift);
	x_centerCube[4].z = (int)(x_centerCube[0].z);

	x_centerCube[5].x = (int)(x_centerCube[0].x - xShift);
	x_centerCube[5].z = (int)(x_centerCube[0].z);

	x_centerCube[6].x = (int)(x_centerCube[0].x);
	x_centerCube[6].z = (int)(x_centerCube[0].z + zShift);

	x_centerCube[6].x = (int)(x_centerCube[0].x);
	x_centerCube[6].z = (int)(x_centerCube[0].z - zShift);

	x_centerCube[0].x -= xShift;
	x_centerCube[0].z -= zShift;


	//On trouve le terrain le plus propice
	bestCenterCube = x_centerCube[0];
	int start(rand()%8);
	int tempValue;
	int bestValue(testTerrain(x_centerCube[start], i_nbRows, i_nbColumns));

	for(int n_i(1); n_i < 8; ++n_i)
	{
		tempValue = testTerrain(x_centerCube[(start + n_i) % 8], i_nbRows, i_nbColumns);
		if(tempValue < bestValue)
		{
			bestValue = tempValue;
			bestCenterCube = x_centerCube[(start + n_i) % 8];
		}
	}

	//On défini les coins de la zone choisi
	i_corner1 = Vector3I(bestCenterCube.x - xShift, bestCenterCube.y, bestCenterCube.z - zShift);
	i_corner2 = Vector3I(bestCenterCube.x + xShift, bestCenterCube.y, bestCenterCube.z + zShift);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void buildHouse(int i_teamID, int i_unitID, Vector& i_housePosition)
///
/// Fonction permettant de construire une maison simple
/// 
/// @param[in] i_teamID : index de l'équipe
/// @param[in] i_unitID : index de l'unité
/// @param[in] i_housePosition : centre inférieur de la maison
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void UnitHandler::buildHouse(int i_teamID, int i_unitID, Vector3I& i_housePosition)
{
	House* house = new House(i_housePosition);
	mp_houseList.push_back(house);

	const int k_halfHouseSize(house->getSize()/2);
	const int k_cubeSize(1);

	//Construction des 4 murs de la maison
	buildWall(i_teamID, i_unitID, i_housePosition + Vector3I(-k_halfHouseSize, 0, -k_halfHouseSize), i_housePosition + Vector3I(-k_halfHouseSize, 0, k_halfHouseSize), 
									i_housePosition + Vector3I(-k_halfHouseSize - k_cubeSize, 0, -k_halfHouseSize));
	buildWall(i_teamID, i_unitID, i_housePosition + Vector3I(- k_halfHouseSize + k_cubeSize, 0, k_halfHouseSize), i_housePosition + Vector3I(k_halfHouseSize - k_cubeSize, 0, k_halfHouseSize), 
									i_housePosition + Vector3I(- k_halfHouseSize + k_cubeSize, 0, k_halfHouseSize + k_cubeSize));
	buildWall(i_teamID, i_unitID, i_housePosition + Vector3I(k_halfHouseSize, 0, k_halfHouseSize), i_housePosition + Vector3I(k_halfHouseSize, 0, -k_halfHouseSize),
									i_housePosition + Vector3I(k_halfHouseSize + k_cubeSize, 0, k_halfHouseSize));
	buildWall(i_teamID, i_unitID, i_housePosition + Vector3I(k_halfHouseSize - k_cubeSize, 0, -k_halfHouseSize), i_housePosition + Vector3I(-k_halfHouseSize + k_cubeSize, 0, -k_halfHouseSize), 
									i_housePosition + Vector3I(k_halfHouseSize - k_cubeSize, 0, -k_halfHouseSize - k_cubeSize));
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void buildWall (int i_teamID, int i_unitID, Vector& i_wallCubePos1, Vector& i_wallCubePos2, Vector& i_unitStartCubePos )
///
/// Fonction permettant à une unité de construire un mur
/// 
/// @param[in] i_teamID : index de l'équipe
/// @param[in] i_unitID : index de l'unité
/// @param[in] i_wallCubePos1 : index de l'équipe
/// @param[in] i_wallCubePos2 : index de l'unité
/// @param[in] i_unitStartCubePos : index de l'équipe
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void UnitHandler::buildWall(int i_teamID, int i_unitID, Vector3I& i_wallCubePos1, Vector3I& i_wallCubePos2, Vector3I& i_unitStartCubePos)
{
	/*const int k_cubeSize(1);


	Vector3I direction = i_wallCubePos2 - i_wallCubePos1;
	int length = (int)direction.length();
	direction = direction.normalize();

	//On effectue le pathfind @@@@@ blockant!
	startPathFind(i_teamID, i_unitID,  i_unitStartCubePos );
	while( !mp_unitList[i_teamID][i_unitID]->m_aStar.getReaded() )
	{
		if( !mp_unitList[i_teamID][i_unitID]->m_aStar.getIsDone() )
			executePathFind(mp_unitList[i_teamID][i_unitID]);
	}

	//On ajoute les murs
	for(int i(0); i < length; ++i)
	{
		if( i != 0)
			moveAction(i_teamID, i_unitID, Terrain::cubeToPosition( i_unitStartCubePos + direction * i ) );

		addAction(i_teamID, i_unitID, i_wallCubePos1 + direction * i);
		addAction(i_teamID, i_unitID, i_wallCubePos1 + direction * i + Vector3I(0,k_cubeSize,0));
		addAction(i_teamID, i_unitID, i_wallCubePos1 + direction * i + Vector3I(0, 2 * k_cubeSize,0));
	}
	length = 0;*/
}



////////////////////////////////////////////////////////////////////////
///
/// @fn void startPathFind(int i_teamID, int i_unitID, const Vector& i_targetCube)
///
/// Constructeur
/// 
/// @param[in] i_teamID : index de l'équipe
/// @param[in] i_unitID : index de l'unité
/// @param[in] i_targetCube : position visée
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void UnitHandler::startPathFind(int i_teamID, int i_unitID, const Vector3I& i_targetCube)
{
	Unit* p_unit(mp_unitList[i_teamID][i_unitID]);

	Vector3I pathStartPos;
	std::deque<Action*>* p_targetList = &p_unit->mp_targetList;
	
	//Par défaut, on trouve le chemin à partir de la position de l'unité
	pathStartPos = Terrain::positionToCube(p_unit->getPosition());

	//Si la liste d'actions en attente contient un moveAction, on prend la derniere position associée à cette action à la place.
	for(int i(0), n_targetListSize(p_targetList->size()); i < n_targetListSize; ++i)
	{
		if((*p_targetList)[i]->getType() == MOVE_ACTION )
		{
			MoveAction* x_moveAction = dynamic_cast<MoveAction*>( (*p_targetList)[i] );
			pathStartPos = Terrain::positionToCube(x_moveAction->getTargetCubePos());
			break;
		}
	}

	//On initialise le aStar
	p_unit->m_aStar.init(pathStartPos, i_targetCube, WALKING_TYPE);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void animate(Uint32 i_timestep)
///
/// Constructeur
/// 
/// @param[in] i_timestep : différence de temps depuis la dernière animation
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void UnitHandler::animate(Uint32 i_timestep)
{
	int teamCount = mp_unitList.size();
	Unit* p_unit;

	//On itère pour tous les unités
	for(int i(0); i < teamCount; ++i)
	{
		for(int j(0); j < mp_unitList[i].size(); ++j)
		{
			p_unit = mp_unitList[i][j];
			if(p_unit != 0)
			{	
				p_unit->animate(i_timestep);				
			}
		}
	}
	applyBoidBehaviors(i_timestep);
}

void UnitHandler::moveTemplateAction(int i_teamID, int i_unitID, Template* ip_template)
{
	HitBox hitBox(ip_template->getHitBox(true));

	Vector templatePosition(ip_template->getPosition());

	std::vector<Vector> hitsPositions;
	Vector correctionDirection((mp_unitList[i_teamID][i_unitID]->getPosition() - hitBox.getPosition()).normalize());
	float correctionScale(hitBox.getDimension(WIDTH) + hitBox.getDimension(HEIGHT) + hitBox.getDimension(LENGTH));
	hitBox.rayCollisionPosition(mp_unitList[i_teamID][i_unitID]->getPosition() + (correctionDirection * correctionScale), hitBox.getPosition(), hitsPositions);
	
	assert(hitsPositions.size() == 1);

	
	//On trouve les limites
	Vector3I templateMin(Terrain::positionToCube(templatePosition));
	Vector3I templateMax(Terrain::positionToCube(Vector(templatePosition)));
	templateMax.x +=ip_template->getCubeDimension(WIDTH);
	templateMax.y +=ip_template->getCubeDimension(HEIGHT);
	templateMax.z +=ip_template->getCubeDimension(LENGTH);

	//On se déplace de 1 dans toutes directions pour ne pas être dans le template
	--templateMin.x;
	--templateMin.y;
	--templateMin.z;

	++templateMax.x;
	++templateMax.y;
	++templateMax.z;

	//On trouve la première position à tester
	Vector3I startingTarget(Terrain::positionToCube(hitsPositions[0] + correctionDirection * HALFSIDE / 2.f ));
	startingTarget.y = templateMin.y;

	//On test les cubes propices en encerclant le template en alternant de chaque coté 
	int incrementX[2] = {0, 0};
	int incrementZ[2] = {0, 0};
	Vector3I currentTarget[2] = {startingTarget, startingTarget};

	Vector3I foundPosition(-1, -1, -1);
	//On augmente d'étage jusqu'à arriver au maximum 
	Terrain* p_terrain(Terrain::getInstance());
	for(int yOffset(0), maxYOffset(ip_template->getCubeDimension(HEIGHT)); yOffset < maxYOffset; ++yOffset)
	{
		for(int i(0); i < 2; ++i)
		{
			currentTarget[i] = startingTarget;
			currentTarget[i].y += yOffset;
		}

		incrementX[0] = 0;
		incrementZ[0] = 0;
		incrementX[1] = 0;
		incrementZ[1] = 0;

		do 
		{
			for(int i(0); i < 2; ++i)
			{
				bool xMin(currentTarget[i].x == templateMin.x);
				bool xMax(currentTarget[i].x == templateMax.x);
				bool zMin(currentTarget[i].z == templateMin.z);
				bool zMax(currentTarget[i].z == templateMax.z);

				if(xMin && ((!zMax && i) || (!zMin && !i)))
					incrementZ[i] += (i)?1:-1;
				else if(xMax && ((!zMin && i) || (!zMax && !i)))
					incrementZ[i] += (i)?-1:1;
				else if(zMin && ((!xMin && i) || (!xMax && !i)))
					incrementX[i] += (i)?-1:1;
				else if(zMax && ((!xMax && i) || (!xMin && !i)))
					incrementX[i] += (i)?1:-1;

				currentTarget[i].x = startingTarget.x + incrementX[i];
				currentTarget[i].z = startingTarget.z + incrementZ[i];

				if(p_terrain->isWalkable(currentTarget[i].x, currentTarget[i].y, currentTarget[i].z) && !ip_template->isPositionOccupied(currentTarget[i]))
				{
					foundPosition = currentTarget[i];
					goto POSITION_FOUND;
				}

				//On doit vérifier entre les deux operations pour ne pas skipper le point commun
				if(currentTarget[0] == currentTarget[1])
					break;
			}
		} 
		while(currentTarget[0] != currentTarget[1]);
	}
	POSITION_FOUND:

	//On si l'on ne trouve rien on se rapproche du template sans le construire
	if(foundPosition.x == -1 && foundPosition.y == -1 && foundPosition.z == -1)
	{
		//TODO
		return;
	}
	//On ajoute au template la position actuelles des travailleurs
	ip_template->addWorkerPosition(foundPosition, mp_unitList[i_teamID][i_unitID]);

	//On déplace les travailleurs
	moveAStarAction(i_teamID, i_unitID, foundPosition);
}

void UnitHandler::buildTemplateAction(int i_teamID, int i_unitID, Template* ip_template)
{
	ip_template->resetBuildingState();
	mp_unitList[i_teamID][i_unitID]->mp_targetList.push_front(new BuildTemplateAction(ip_template));
}

void UnitHandler::addAction(int i_teamID, int i_unitID, Vector3I& i_targetCube)
{
	mp_unitList[i_teamID][i_unitID]->mp_targetList.push_front(new AddAction(i_targetCube));
}
void UnitHandler::deleteAction(int i_teamID, int i_unitID, Vector3I& i_targetCube)
{
	mp_unitList[i_teamID][i_unitID]->mp_targetList.push_front(new DeleteAction(i_targetCube));
}
void UnitHandler::moveAction(int i_teamID, int i_unitID, Vector& i_targetCube)
{
	mp_unitList[i_teamID][i_unitID]->mp_targetList.push_front(new MoveAction(i_targetCube));
}
void UnitHandler::moveAStarAction(int i_teamID, int i_unitID, const Vector3I& i_targetCube)
{
	mp_unitList[i_teamID][i_unitID]->mp_targetList.push_front(new MoveAStarAction(i_targetCube));
}

Unit* UnitHandler::getUnit(int i_teamID, int i_unitID)
{
	return mp_unitList[i_teamID][i_unitID];
}

void UnitHandler::addCamera(Camera* ip_camera)
{
	if(mp_camera != 0)
		DeferredRenderer::getInstance()->removeWhiteLine(mp_camera);
	DeferredRenderer::getInstance()->addWhiteLine(ip_camera);
	mp_camera = ip_camera;
}

std::vector<Unit*> UnitHandler::rayCollision(Vector i_sourceVect, Vector i_targetVect, Unit* i_excludedUnit, bool i_onlyFirst)
{
	Terrain* p_terrain(Terrain::getInstance());
	Vector3I sourceCube(Terrain::positionToCube(i_sourceVect));
	Vector3I targetCube(Terrain::positionToCube(i_targetVect));

	Vector direction(i_targetVect - i_sourceVect);
	direction.normalize();

	float tDeltaX(abs(SIDE/direction.x));
	float tDeltaY(abs(SIDE/direction.y));
	float tDeltaZ(abs(SIDE/direction.z));

	int editPositionX;
	int editPositionY;
	int editPositionZ;

	if(direction.x >= 0)
		editPositionX = 1;
	else
		editPositionX = -1;

	if(direction.y >= 0)
		editPositionY = 1;
	else
		editPositionY = -1;

	if(direction.z >= 0)
		editPositionZ = 1;
	else
		editPositionZ = -1;

	float tMaxX(abs(((sourceCube.x * SIDE + HALFSIDE * editPositionX) - i_sourceVect.x)/direction.x));
	float tMaxY(abs(((sourceCube.y * SIDE + HALFSIDE * editPositionY) - i_sourceVect.y)/direction.y));
	float tMaxZ(abs(((sourceCube.z * SIDE + HALFSIDE * editPositionZ) - i_sourceVect.z)/direction.z));

	std::vector<Unit*> result;

	Position position;
	do
	{
		position.x = sourceCube.x;
		position.y = sourceCube.y;
		position.z = sourceCube.z;

		//if(p_terrain->isUnitAtPosition(position))
		//{
		//	std::vector<Unit*> unitList(p_terrain->getUnitAtPosition(position));

		//	for(int i(0), unitListSize(unitList.size()); i < unitListSize; ++i)
		//	{
		//		if(unitList[i]->m_hitBox.rayCollision(i_sourceVect, i_targetVect))
		//		{
		//			result.push_back(unitList[i]);
		//			if(i_onlyFirst)
		//				return result;
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
	while(((sourceCube.x <= targetCube.x && editPositionX == 1) || (sourceCube.x >= targetCube.x && editPositionX == -1)) && 
		  ((sourceCube.y <= targetCube.y && editPositionY == 1) || (sourceCube.y >= targetCube.y && editPositionY == -1))  && 
		  ((sourceCube.z <= targetCube.z && editPositionZ == 1) || (sourceCube.z >= targetCube.z && editPositionZ == -1)) );
	return result;
}

void UnitHandler::resetActionList(int i_teamID, int i_unitID)
{
	TemplateManager::getInstance()->removeWorkerPosition(mp_unitList[i_teamID][i_unitID]);
	std::deque<Action*> emptyDeque;
	mp_unitList[i_teamID][i_unitID]->mp_targetList.swap(emptyDeque);
}

unsigned int UnitHandler::getRessources(int i_teamID, RessourcesType i_resssourcesType)
{
	assert(i_teamID >= 0 && i_teamID < mp_ressources.size());
	return mp_ressources[i_teamID]->getRessources(i_resssourcesType);
}

void UnitHandler::addRessources(int i_teamID, RessourcesType i_resssourcesType, unsigned int i_ammount)
{
	assert(i_teamID >= 0 && i_teamID < mp_ressources.size());
	mp_ressources[i_teamID]->addRessources(i_resssourcesType, i_ammount);
}

bool UnitHandler::removeRessources(int i_teamID, RessourcesType i_resssourcesType, unsigned int i_ammount)
{
	assert(i_teamID >= 0 && i_teamID < mp_ressources.size());
	return mp_ressources[i_teamID]->removeRessources(i_resssourcesType, i_ammount);
}

//std::vector<Unit*> UnitHandler::volumeCollision( const Vector i_sourceVect[4], const Vector i_targetVect[4])
//{
//	Terrain* p_terrain(Terrain::getInstance());
//
//	Vector sourceCorner[4];
//	for(int i(0); i < 4; ++i)
//		sourceCorner[i] = i_sourceVect[i];
//
//	Vector targetCorner[4];
//	for(int i(0); i < 4; ++i)
//		targetCorner[i] = i_targetVect[i];
//
//	//On enleve les targets hors limite
//	//Négatif
//	float lenghtModifier(1);
//	float lenghtModifierGlobal(0);
//	for(int i(0); i < 4; ++i)
//	{
//		if(targetCorner[i].x < 0) 
//			lenghtModifier = min(1 - abs(targetCorner[i].x)/abs(targetCorner[i].x - sourceCorner[i].x), lenghtModifier);
//
//		if(targetCorner[i].y < 0)
//			lenghtModifier = min(1 - abs(targetCorner[i].y)/abs(targetCorner[i].y - sourceCorner[i].y), lenghtModifier);
//
//		if( targetCorner[i].z < 0)
//			lenghtModifier = min(1 - abs(targetCorner[i].z)/abs(targetCorner[i].z - sourceCorner[i].z), lenghtModifier);
//
//		lenghtModifierGlobal = max(lenghtModifier, lenghtModifierGlobal);
//		lenghtModifier = 1;
//	}
//	for(int i(0); i < 4; ++i)
//		targetCorner[i] = sourceCorner[i] + (targetCorner[i] - sourceCorner[i]) * lenghtModifierGlobal;
//	//On enleve les source hors limite
//
//	//WhiteLine::whiteLineDebug.push_back(WhiteLine(sourceCorner[0], targetCorner[0]));
//	//WhiteLine::whiteLineDebug.push_back(WhiteLine(sourceCorner[1], targetCorner[1]));
//	//WhiteLine::whiteLineDebug.push_back(WhiteLine(sourceCorner[2], targetCorner[2]));
//	//WhiteLine::whiteLineDebug.push_back(WhiteLine(sourceCorner[3], targetCorner[3]));
//
//	//Convertir les coins en cube
//	Vector3I sourceCornerCube[4];
//	Vector3I targetCornerCube[4];
//	for(int i(0); i < 4; ++i)
//	{
//		sourceCornerCube[i] = Terrain::positionToCube(sourceCorner[i]);
//		targetCornerCube[i] = Terrain::positionToCube(targetCorner[i]);
//	}
//
//	std::hash_set<Vector3I> visitedCubeListLine1;
//	std::hash_set<Vector3I> visitedCubeListLine2;
//	Vector direction((targetCorner[0] - sourceCorner[0]).normalize());
//	rayCollision(sourceCornerCube[0], targetCornerCube[0], direction, visitedCubeListLine1);
//	direction = (targetCorner[1] - sourceCorner[1]).normalize();
//	rayCollision(sourceCornerCube[1], targetCornerCube[1], direction, visitedCubeListLine2);
//
//	std::hash_set<Vector3I> visitedCubeListArea1;
//
//	std::hash_set<Vector3I>::iterator listLineIt1(visitedCubeListLine1.begin());
//	std::hash_set<Vector3I>::iterator listLineIt2(visitedCubeListLine2.begin());
//	while(listLineIt1 != visitedCubeListLine1.end() && listLineIt2 != visitedCubeListLine2.end())
//	{
//		direction = (Terrain::cubeToPosition(*listLineIt2) - Terrain::cubeToPosition(*listLineIt1)).normalize();
//		rayCollision(*listLineIt1, *listLineIt2, direction, visitedCubeListArea1);
//
//		//Vector whiteLinePos(Terrain::cubeToPosition(*listLineIt1));
//		//WhiteLine::whiteLineDebug.push_back(WhiteLine(Vector(whiteLinePos.m_x + HALFSIDE/2, whiteLinePos.m_y + HALFSIDE/2, whiteLinePos.m_z + HALFSIDE/2), 
//		//	Vector(whiteLinePos.m_x - HALFSIDE/2, whiteLinePos.m_y - HALFSIDE/2, whiteLinePos.m_z - HALFSIDE/2)));
//
//		//whiteLinePos = Terrain::cubeToPosition(*listLineIt2);
//		//WhiteLine::whiteLineDebug.push_back(WhiteLine(Vector(whiteLinePos.m_x + HALFSIDE/2, whiteLinePos.m_y + HALFSIDE/2, whiteLinePos.m_z + HALFSIDE/2), 
//		//	Vector(whiteLinePos.m_x - HALFSIDE/2, whiteLinePos.m_y - HALFSIDE/2, whiteLinePos.m_z - HALFSIDE/2)));
//
//		++listLineIt1;
//		++listLineIt2;
//	}
//
//	{
//		std::hash_set<Vector3I> emptyVect1;
//		visitedCubeListLine1.swap(emptyVect1);
//		std::hash_set<Vector3I> emptyVect2;
//		visitedCubeListLine2.swap(emptyVect2);
//	}
//
//	direction = (targetCorner[2] - sourceCorner[2]).normalize();
//	rayCollision(sourceCornerCube[2], targetCornerCube[2], direction, visitedCubeListLine1);
//	direction = (targetCorner[3] - sourceCorner[3]).normalize();
//	rayCollision(sourceCornerCube[3], targetCornerCube[3], direction, visitedCubeListLine2);
//	
//	
//	std::hash_set<Vector3I> visitedCubeListArea2;
//
//
//	listLineIt1 = visitedCubeListLine1.begin();
//	listLineIt2 = visitedCubeListLine2.begin();
//	while(listLineIt1 != visitedCubeListLine1.end() && listLineIt2 != visitedCubeListLine2.end())
//	{
//		direction = (Terrain::cubeToPosition(*listLineIt2) - Terrain::cubeToPosition(*listLineIt1)).normalize();
//		rayCollision(*listLineIt1, *listLineIt2, direction, visitedCubeListArea2);
//
//		//Vector whiteLinePos(Terrain::cubeToPosition(*listLineIt1));
//		//WhiteLine::whiteLineDebug.push_back(WhiteLine(Vector(whiteLinePos.m_x + HALFSIDE/2, whiteLinePos.m_y + HALFSIDE/2, whiteLinePos.m_z + HALFSIDE/2), 
//		//	Vector(whiteLinePos.m_x - HALFSIDE/2, whiteLinePos.m_y - HALFSIDE/2, whiteLinePos.m_z - HALFSIDE/2)));
//
//		//whiteLinePos = (Terrain::cubeToPosition(*listLineIt2));
//		//WhiteLine::whiteLineDebug.push_back(WhiteLine(Vector(whiteLinePos.m_x + HALFSIDE/2, whiteLinePos.m_y + HALFSIDE/2, whiteLinePos.m_z + HALFSIDE/2), 
//		//	Vector(whiteLinePos.m_x - HALFSIDE/2, whiteLinePos.m_y - HALFSIDE/2, whiteLinePos.m_z - HALFSIDE/2)));
//
//		++listLineIt1;
//		++listLineIt2;
//	}
//	
//	
//	std::hash_set<Vector3I> visitedCubeListVolume;
//
//	std::hash_set<Vector3I>::iterator listAreaIt1(visitedCubeListArea1.begin());
//	std::hash_set<Vector3I>::iterator listAreaIt2(visitedCubeListArea2.begin());
//	while(listAreaIt1 != visitedCubeListArea1.end() && listAreaIt2 != visitedCubeListArea2.end())
//	{
//		direction = (Terrain::cubeToPosition(*listAreaIt2) - Terrain::cubeToPosition(*listAreaIt1)).normalize();
//		rayCollision(*listAreaIt1, *listAreaIt2, direction, visitedCubeListVolume);
//
//		Vector whiteLinePos(Terrain::cubeToPosition(*listAreaIt1));
//		//WhiteLine::whiteLineDebug.push_back(WhiteLine(Vector(whiteLinePos.m_x + HALFSIDE/2, whiteLinePos.m_y + HALFSIDE/2, whiteLinePos.m_z + HALFSIDE/2), 
//		//	Vector(whiteLinePos.m_x - HALFSIDE/2, whiteLinePos.m_y - HALFSIDE/2, whiteLinePos.m_z - HALFSIDE/2)));
//
//		//whiteLinePos = (Terrain::cubeToPosition(*listAreaIt2));
//		//WhiteLine::whiteLineDebug.push_back(WhiteLine(Vector(whiteLinePos.m_x + HALFSIDE/2, whiteLinePos.m_y + HALFSIDE/2, whiteLinePos.m_z + HALFSIDE/2), 
//		//	Vector(whiteLinePos.m_x - HALFSIDE/2, whiteLinePos.m_y - HALFSIDE/2, whiteLinePos.m_z - HALFSIDE/2)));
//
//		++listAreaIt1;
//		++listAreaIt2;
//	}
//	
//	std::vector<Unit*> unitFound;
//	std::vector<Unit*> unitFoundTemp;
//	Position position;
//
//	std::hash_set<Vector3I>::iterator listVolumeIt(visitedCubeListVolume.begin());
//
//	while(listVolumeIt != visitedCubeListVolume.end() && listVolumeIt != visitedCubeListVolume.end())
//	{
//		position.x = listVolumeIt->x;
//		position.y = listVolumeIt->y;
//		position.z = listVolumeIt->z;
//		unitFoundTemp = p_terrain->getUnitAtPosition(position);
//		unitFound.insert(unitFound.end(), unitFoundTemp.begin(), unitFoundTemp.end());
//
//		//Vector whiteLinePos(Terrain::cubeToPosition(*listVolumeIt));
//		//WhiteLine::whiteLineDebug.push_back(WhiteLine(Vector(whiteLinePos.m_x + HALFSIDE/2, whiteLinePos.m_y + HALFSIDE/2, whiteLinePos.m_z + HALFSIDE/2), 
//		//								Vector(whiteLinePos.m_x - HALFSIDE/2, whiteLinePos.m_y - HALFSIDE/2, whiteLinePos.m_z - HALFSIDE/2)));
//
//		++listVolumeIt;
//	}
//	return unitFound;
//}

void UnitHandler::rayCollision(const Vector3I& i_sourceCubeVect, const Vector3I& i_targetCubeVect, const Vector& i_direction, std::hash_set<Vector3I>& i_visitedCubeList)
{
	Terrain* p_terrain(Terrain::getInstance());
	Vector3I currentCube(i_sourceCubeVect);

	//On trouve la distance maximal de deplacement pour ne pas raté de cube.
	float tDeltaX(abs(SIDE/i_direction.x));
	float tDeltaY(abs(SIDE/i_direction.y));
	float tDeltaZ(abs(SIDE/i_direction.z));

	//On trouve la direction 
	int editPositionX;
	int editPositionY;
	int editPositionZ;

	if(i_direction.x >= 0)
		editPositionX = 1;
	else
		editPositionX = -1;

	if(i_direction.y >= 0)
		editPositionY = 1;
	else
		editPositionY = -1;

	if(i_direction.z >= 0)
		editPositionZ = 1;
	else
		editPositionZ = -1;

	currentCube.setDirection(editPositionX == 1, editPositionY == 1, editPositionZ == 1);

	//De combien doit ton longer direction pour arriver à la limite du voxel 
	float tMaxX(abs(((currentCube.x * SIDE + HALFSIDE * editPositionX) - Terrain::cubeToPosition(i_sourceCubeVect.x))/i_direction.x));
	float tMaxY(abs(((currentCube.y * SIDE + HALFSIDE * editPositionY) - Terrain::cubeToPosition(i_sourceCubeVect.y))/i_direction.y));
	float tMaxZ(abs(((currentCube.z * SIDE + HALFSIDE * editPositionZ) - Terrain::cubeToPosition(i_sourceCubeVect.z))/i_direction.z));

	std::hash_set<Vector3I>::iterator itLastInsert;

	do
	{
		itLastInsert = i_visitedCubeList.insert(itLastInsert, currentCube);

		//On decide si l'on doit modifier x y ou z pour le prochain cube selon la distance avec le prochain voxel
		if(tMaxX < tMaxY) 
		{
			if(tMaxX < tMaxZ) 
			{
				currentCube.x = currentCube.x + editPositionX;
				tMaxX = tMaxX + tDeltaX;
			} 
			else 
			{
				currentCube.z = currentCube.z + editPositionZ;
				tMaxZ = tMaxZ + tDeltaZ;
			}
		} 
		else 
		{
			if(tMaxY < tMaxZ) 
			{
				currentCube.y = currentCube.y + editPositionY;
				tMaxY = tMaxY + tDeltaY;
			}
			else 
			{
				currentCube.z = currentCube.z + editPositionZ;
				tMaxZ = tMaxZ + tDeltaZ;
			}
		}
	} 
	//On vérifie si l'on a déaprer la cible
	while(((currentCube.x <= i_targetCubeVect.x && editPositionX == 1) || (currentCube.x >= i_targetCubeVect.x && editPositionX == -1)) && 
		((currentCube.y <= i_targetCubeVect.y && editPositionY == 1) || (currentCube.y >= i_targetCubeVect.y && editPositionY == -1))  && 
		((currentCube.z <= i_targetCubeVect.z && editPositionZ == 1) || (currentCube.z >= i_targetCubeVect.z && editPositionZ == -1))
		);
}

void UnitHandler::setIsSelected(Unit* ip_unit, bool i_isSelected)
{
	ip_unit->setIsSelected(i_isSelected);
}

void UnitHandler::deselectAll()
{
	for(int i(0),teamCount(mp_unitList.size()); i < teamCount; ++i)
	{
		for(int j(0), unitCount(mp_unitList[i].size()); j < unitCount; ++j)
		{
			mp_unitList[i][j]->setIsSelected(false);
		}
	}
}

void UnitHandler::reloadPhysic()
{
	//On itère pour tous les unités
	for(int i(0), teamCount(mp_unitList.size()); i < teamCount; ++i)
	{
		for(int j(0), unitCount(mp_unitList[i].size()); j < unitCount; ++j)
		{
			if(mp_unitList[i][j] != 0)
			{	
				mp_unitList[i][j]->reloadPhysic();
			}
		}
	}
}

int UnitHandler::createBoidGroup(const std::hash_set<Unit*>& ip_boidList)
{
	//On enleve les unitées de leur ancien groupe
	BoidGroup* unitBoidGroup(0);
	int unitBoidGroupID(-1);
	for(std::hash_set<Unit*>::iterator it(ip_boidList.begin()), endIt(ip_boidList.end()); it != endIt; ++it)
	{
		unitBoidGroupID = (*it)->getBoidGroup();
		if(unitBoidGroupID == -1)
			continue;

		unitBoidGroup = mp_boidGroups[unitBoidGroupID];
		if(unitBoidGroup->removeBoid(*it))
			deleteBoidGroup(unitBoidGroupID);
	}

	//On crée le nouveau groupe
	unitBoidGroupID = mp_boidGroups.size();
	mp_boidGroups.push_back(new BoidGroup(unitBoidGroupID));
	for(std::hash_set<Unit*>::iterator it(ip_boidList.begin()), endIt(ip_boidList.end()); it != endIt; ++it)
	{
		mp_boidGroups[unitBoidGroupID]->addBoid(*it);
	}

	return unitBoidGroupID;
}

Vector UnitHandler::getBoidGroupCenter(int i_boidGroupID)
{
	return mp_boidGroups[i_boidGroupID]->getCenter();
}

void UnitHandler::deleteBoidGroup(int i_boidGroupID)
{
	assert(mp_boidGroups[i_boidGroupID]->getListSize() == 0);
	int boidGroupsSize(mp_boidGroups.size());

	assert(boidGroupsSize > i_boidGroupID && i_boidGroupID >= 0);

	BoidGroup* boidGroupToDelete(mp_boidGroups[i_boidGroupID]);

	mp_boidGroups[i_boidGroupID] = mp_boidGroups[boidGroupsSize - 1];
	mp_boidGroups[i_boidGroupID]->setGroupID(i_boidGroupID);

	delete boidGroupToDelete;
}

void UnitHandler::applyBoidBehaviors(Uint32 i_timestep)
{
	//for(int i(0), boidGroupsSize(mp_boidGroups.size()); i < boidGroupsSize; ++i)
	//{
	//	mp_boidGroups[i]->applyBoidBehaviors(i_timestep);
	//}

	for(int i(0), teamCount(mp_unitList.size()); i < teamCount; ++i)
	{
		for(int j(0), unitCount(mp_unitList[i].size()); j < unitCount; ++j)
		{
			if(mp_unitList[i][j] != 0)
			{	
				mp_unitList[i][j]->radarScan();
				mp_unitList[i][j]->boidBehaviors(i_timestep);
			}
		}
	}
}

void UnitHandler::moveBoidGroup(int i_boidGroupID, const Vector3I& i_centerCube)
{ 
	std::vector<Vector3I> endPosition;
	rectangleFormation(i_centerCube, mp_boidGroups[i_boidGroupID]->getListSize(), Terrain::cubeToPosition(i_centerCube) - mp_boidGroups[i_boidGroupID]->getCenter(), endPosition);
	mp_boidGroups[i_boidGroupID]->findPath(endPosition);
}

void UnitHandler::rectangleFormation(const Vector3I& i_centerCube, int i_groupSize, const Vector& i_direction, std::vector<Vector3I>& o_endPosition)
{
	const float k_distance(1.f);
	const int maxWidth(5);
	Vector normalizedDirection(i_direction);
	normalizedDirection.normalize();
	Vector leftDirection(Vector(0.f, 1.f, 0.f).crossProduct(normalizedDirection));
	
	for(int i(0); i < i_groupSize; ++i)
	{
		int lineNumber(i % maxWidth);
		Vector currentPosition(Terrain::cubeToPosition(i_centerCube));
		int orientation((lineNumber%2)?1:-1);
		currentPosition = currentPosition + (leftDirection * lineNumber/2.f * orientation * k_distance);

		o_endPosition.push_back(Terrain::positionToCube(currentPosition));
	}
}

void UnitHandler::updateHitpointBar()
{
	int teamCount = mp_unitList.size();
	Unit* p_unit;
	Vector cameraForward(SceneOpenGL::getInstance()->getCamera()->getForward());

	//On itère pour tous les unités
	for(int i(0); i < teamCount; ++i)
	{
		for(int j(0); j < mp_unitList[i].size(); ++j)
		{
			p_unit = mp_unitList[i][j];
			if(p_unit != 0)
			{	
				p_unit->updateHitpointBar(cameraForward);			
			}
		}
	}

}