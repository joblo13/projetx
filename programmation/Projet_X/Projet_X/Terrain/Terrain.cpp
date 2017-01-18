#include "Terrain.h"

#include "../Rendering/Texture.h"
#include "../Math/Noise2D.h"
#include "../Camera/Camera.h"
#include "../Math/Matrix.h"
#include "../Math/Matrix3x3.h"
#include "../Shaders/Shader.h"
#include "../AI/AStar.h"
#include "../Ai/Node.h"
#include "../Position.h"
#include "../Input.h"
#include "../Debugger/GlDebugger.h"
#include "../Rendering/DeferredRenderer/DeferredRenderer.h"
#include "../Building/TemplateManager.h" 

#include "../GUI/GUIHandler.h"
#include "../GUI/MapGUI.h"

#include "../Serializer.h"

#include "../Entities/UnitHandler.h"

//Rayon des chunks a afficher
#define PRINTEDCHUNK 20

Terrain* Terrain::mp_instance = 0;

////////////////////////////////////////////////////////////////////////
///
/// @fn Terrain()
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Terrain::Terrain()
:m_initialised(false), m4p_chunkMap(0), m_showWire(false),
//physique
m_broadphase(0),
m_dispatcher(0),
m_solver(0),
m_collisionConfiguration(0),
m_dynamicsWorld(0),
Drawable(NULL_SHADER),
mk_chunkLength(CHUNKLENGTH)
{
	m_texturesIndex.push_back(Texture::getTextureID("TextureSampler", "Texture/terrain.png"));
	glGetError();
	initPhysics();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Terrain()
///
/// Destructeur. on détruit le errorCube et le terrain
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Terrain::~Terrain()
{
	//on supprime les éléments de la physique
	exitPhysics();
	//On supprime tout les chunks
	clearTerrain();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void setDimension(int i_width, int i_height, int i_length)
///
/// Fonction permettant de définir la grandeur du terrain. Si le terrain est déja défini,
/// on le vide
/// 
/// @param[in] i_width : Largeur du terrain (en chunks)
/// @param[in] i_height : Hauteur du terrain (en chunks)	
/// @param[in] i_length : Longueur du terrain (en chunks)
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::setDimension(int i_width, int i_height, int i_length)
{
	//On vide la map si elle est déjà créé
	if(m_initialised == true)
	{
		clearTerrain();
	}

    //Initialisation
    m_width = i_width;
    m_height = i_height;
    m_length = i_length;

    //allocation dynamique de la map
    m4p_chunkMap = new Chunk***[m_width];
    for(int i(0); i < m_width; ++i)
    {
        m4p_chunkMap[i] = new Chunk**[m_height];
        for(int j(0); j < m_height ; ++j)
            m4p_chunkMap[i][j] = new Chunk*[m_length];
    }
    for(int x(0); x < m_width; ++x)
        for(int y(0); y < m_height; ++y)
            for(int z(0); z < m_length; ++z)
                m4p_chunkMap[x][y][z] = new Chunk(x, y, z, mk_chunkLength, mk_chunkLength, mk_chunkLength);

	//On ajoute le terrain à la liste des éléments à afficher
	DeferredRenderer::getInstance()->addDrawable(this);

	m_mapOverview.resize(m_width * m_length * mk_chunkLength * mk_chunkLength * MAP_PIXEL_SIZE);

	((MapGUI*)(GUIHandler::getInstance()->getLayout(MAP_LAYOUT)))->reloadImage();

	//On spécifie que le terrain est initialisé
	m_initialised = true;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void clearTerrain()
///
/// Fonction permettant de vider le terrain
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::clearTerrain()
{
	//Destruction de la map
	for(int x(0); x < m_width; ++x)
		for(int y(0); y < m_height; ++y)
			for(int z(0); z < m_length; ++z)
					delete m4p_chunkMap[x][y][z];

	for(int x(0); x < m_width; ++x)
		for(int y(0); y < m_height; ++y)
			delete m4p_chunkMap[x][y];

	for(int x(0); x < m_width; ++x)
			delete m4p_chunkMap[x];

	delete m4p_chunkMap;
	//On spécifie que le terrain n'est plus initialisé
	m_initialised = false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool saveMap(const std::string& ik_filePath)
///
/// Fonction permettant de charger un terrain contenue dans un fichier
///
/// @param[in] ik_filePath : Nom du fichier
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Terrain::saveMap(const std::string& ik_filePath)
{
	//On ouvre le fichier
	std::ofstream ofstreamFile;
	ofstreamFile.open("Map/" + ik_filePath, std::ios::binary);

	//On créer les variables pour l'écriture
	const unsigned int k_terrainBufferSize(sizeof(short) + 3 * sizeof(int));
	std::vector<char> a_terrainBuffer(k_terrainBufferSize);
	int bufferPosition = 0;

	//On écrit le header général
	short headerGeneral = HEADER_GENERAL;
	memcpy(&a_terrainBuffer[bufferPosition], &headerGeneral, sizeof(headerGeneral));
	bufferPosition += sizeof(headerGeneral);
	memcpy(&a_terrainBuffer[bufferPosition], &m_width, sizeof(m_width));
	bufferPosition += sizeof(m_width);
	memcpy(&a_terrainBuffer[bufferPosition], &m_height, sizeof(m_height));
	bufferPosition += sizeof(m_height);
	memcpy(&a_terrainBuffer[bufferPosition], &m_length, sizeof(m_length));
	bufferPosition = 0;

	//On écrit le header dans un fichier
	ofstreamFile.write(&a_terrainBuffer[0], k_terrainBufferSize);
	
	//On écrit le header pour chaque chunk suivi du numéro de la chunk
	for(int i(0); i < m_width; ++i)
		for(int j(0); j < m_height; ++j)
			for(int k(0); k < m_length; ++k)
			{
				//On écris le contenue de chaques chunks
				m4p_chunkMap[i][j][k]->saveMap(ofstreamFile);
			}

	//On ferme le fichier
	ofstreamFile.close();
	return true;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool loadMap(const std::string& ik_filePath)
///
/// Fonction permettant de charger un terrain contenue dans un fichier
///
/// @param[in] ik_filePath : Nom du fichier
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Terrain::loadMap(const std::string& ik_filePath)
{
	resetPhysics();

	//On ouvre le fichier
	std::ifstream ifstreamFile;
	ifstreamFile.open("Map/" + ik_filePath, std::ios::binary);

	//On définie les variables de position
	int chunkX;
	int chunkY;
	int chunkZ;

	int chunkSizeX;
	int chunkSizeY;
	int chunkSizeZ;

	int width;
	int height;
	int length;

	//On définie les variables de lecture
	short header;
	char a_buffer[4];

	//On vérifie si le fichier à pu être ouvert
	if(ifstreamFile.is_open())
	{
		//On lit le header
		readFromFile(ifstreamFile, header);

		//Tant que l'on ateint pas la fin du fichier
		while(!ifstreamFile.eof())
		{
			//On vérifie a quel genre d'information on a à faire
			switch(header)
			{
				//Si c'est le header général
				case HEADER_GENERAL:
					//On lit les dimensions
					readFromFile(ifstreamFile, width);
					readFromFile(ifstreamFile, height);
					readFromFile(ifstreamFile, length);

					//On vide la map
					clearTerrain();
					//On définie les nouvelles dimensions
					setDimension(width, height, length);
					break;

				//Si c'est un header de chunk
				case HEADER_CHUNK:
					//On enregistre la chunk courante
					readFromFile(ifstreamFile, chunkX);
					readFromFile(ifstreamFile, chunkY);
					readFromFile(ifstreamFile, chunkZ);

					//On envoie le steam à la chunk courrante
					header = m4p_chunkMap[chunkX][chunkY][chunkZ]->loadMap(ifstreamFile);
					continue;
					break;

				//Si c'est un autre header, il y a eu une erreur
				default:
					ifstreamFile.close();
					return false;
			}
			//On lit le header
			readFromFile(ifstreamFile, header);
		}
	}
	else
		return false;
	
	//On ferme le fichier
	ifstreamFile.close();
	for(int i(0); i < m_width; ++i)
		for(int j(0); j < m_length; ++j)
			for(int k(0); k < m_height; ++k)
				m4p_chunkMap[i][k][j]->updateAndSetBtObject(m_dynamicsWorld);
	return true;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void loadMap(int i_seed)
///
/// Fonction permettant de charger les cubes dans le terrain selon la seeds
///
/// @param[in] i_seed : Seeds de la generation aléatoire
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::loadMap(int i_seed)
{
	resetPhysics();
	float a_density[8];

	//On crée le générateur aléatoire
	Noise2D noise2D(mk_chunkLength, mk_chunkLength, i_seed);

	float cornerRelativeHeight[4];
	float cornerTotaleHeight[4];

	//Pour chaque colonne
	for(int i(0), maxWidth(m_width * mk_chunkLength) ; i < maxWidth; ++i)
		for(int j(0), maxLength(m_length * mk_chunkLength); j < maxLength; ++j)
		{
			//On trouve la valeur de hauteur
			cornerTotaleHeight[0] = noise2D.evaluateFractal((float)(i + 1), (float)(j + 1));
			cornerTotaleHeight[1] = noise2D.evaluateFractal((float)i, (float)(j + 1));
			cornerTotaleHeight[2] = noise2D.evaluateFractal((float)i, (float)j);
			cornerTotaleHeight[3] = noise2D.evaluateFractal((float)(i + 1), (float)j);

			//On itère sur tout les cubes solides de la colonne
			for(int k(0), 
				hight(max(ceil(cornerTotaleHeight[0]), max(ceil(cornerTotaleHeight[1]), max(ceil(cornerTotaleHeight[2]), ceil(cornerTotaleHeight[3]))))); 
			k < hight; ++k)
			{
				//On trouve la distance manquante entre le dernier cube plein et l'isosurface
				cornerRelativeHeight[0] = (k + 1 - cornerTotaleHeight[0]) * SIDE;
				cornerRelativeHeight[1] = (k + 1 - cornerTotaleHeight[1]) * SIDE;
				cornerRelativeHeight[2] = (k + 1 - cornerTotaleHeight[2]) * SIDE;
				cornerRelativeHeight[3] = (k + 1 - cornerTotaleHeight[3]) * SIDE;

				//On trouve le poids de chaque sommets du voxel
				a_density[0] = cornerRelativeHeight[0] - SIDE - ISOSURFACE_ERROR_CORRECTION;
				a_density[1] = cornerRelativeHeight[1] - SIDE - ISOSURFACE_ERROR_CORRECTION;
				a_density[2] = cornerRelativeHeight[2] - SIDE - ISOSURFACE_ERROR_CORRECTION;
				a_density[3] = cornerRelativeHeight[3] - SIDE - ISOSURFACE_ERROR_CORRECTION;

				a_density[4] = cornerRelativeHeight[0] + ISOSURFACE_ERROR_CORRECTION;
				a_density[5] = cornerRelativeHeight[1] + ISOSURFACE_ERROR_CORRECTION;
				a_density[6] = cornerRelativeHeight[2] + ISOSURFACE_ERROR_CORRECTION;
				a_density[7] = cornerRelativeHeight[3] + ISOSURFACE_ERROR_CORRECTION;

				//On choisie le matériel du cube et on le crée
				if(k == 0)
					createCubePrivate(i, k, j, 1, 0, 0, a_density);
				else if(k >= mk_chunkLength)
					createCubePrivate(i, k, j, 3, 100, 100, a_density);
				else
					createCubePrivate(i, k, j, 2, 100, 100, a_density);
			}
		}
	//Mise à jour des cubes initiale
	for(int i(0); i < m_width; ++i)
		for(int j(0); j < m_length; ++j)
			for(int k(0); k < m_height; ++k)
				m4p_chunkMap[i][k][j]->updateAndSetBtObject(m_dynamicsWorld);

}
bool Terrain::createCube(int i_x, int i_y, int i_z, int i_type, float i_hitpoint, float i_maxHitpoint)
{
	float a_density[8] = {-SIDE - ISOSURFACE_ERROR_CORRECTION, -SIDE - ISOSURFACE_ERROR_CORRECTION, -SIDE - ISOSURFACE_ERROR_CORRECTION, -SIDE - ISOSURFACE_ERROR_CORRECTION, 
							ISOSURFACE_ERROR_CORRECTION, ISOSURFACE_ERROR_CORRECTION, ISOSURFACE_ERROR_CORRECTION, ISOSURFACE_ERROR_CORRECTION};

	return createCube(i_x, i_y, i_z, i_type, i_hitpoint, i_maxHitpoint, a_density);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void createCube(int i_x, int i_y, int i_z, int i_resist)
///
/// Fonction permettant de créer un cube dans le terrain et de mettre à jour ses voisins
/// et lui même
/// 
/// @param[in] i_x : Numéro du cube en x
/// @param[in] i_y : Numéro du cube en y (vertical)	
/// @param[in] i_z : Numéro du cube en z
/// @param[in] i_resist : Résistance du cube
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Terrain::createCube(int i_x, int i_y, int i_z, int i_type, float i_hitpoint, float i_maxHitpoint, float ia_density[8])
{
	//On crée le cube
	if(i_type != 0)
	{
		if(createCubePrivate(i_x, i_y, i_z, i_type, i_hitpoint, i_maxHitpoint, ia_density))
		{
			//On update la chunk du cube
			setChangedFlag(i_x, i_y, i_z, true);
			setChangedFlag(i_x, i_y+1, i_z, true);
			setChangedFlag(i_x, i_y-1, i_z, true);
			setChangedFlag(i_x+1, i_y, i_z, true);
			setChangedFlag(i_x-1, i_y, i_z, true);
			setChangedFlag(i_x, i_y, i_z+1, true);
			setChangedFlag(i_x, i_y, i_z-1, true);

			if(isArtificial(i_x+1, i_y, i_z+1))
				setChangedFlag(i_x+1, i_y, i_z+1, true);
			if(isArtificial(i_x-1, i_y, i_z+1))
				setChangedFlag(i_x-1, i_y, i_z+1, true);
			if(isArtificial(i_x+1, i_y, i_z-1))
				setChangedFlag(i_x+1, i_y, i_z-1, true);
			if(isArtificial(i_x-1, i_y, i_z-1))
				setChangedFlag(i_x-1, i_y, i_z-1, true);

			if(i_type > 0 && getCubeType(i_x, i_y - 1, i_z) > 0 && isBottomFilled(i_x, i_y, i_z))
			{
				setIsoPosition(i_x, i_y - 1, i_z, CUBE_HIGH_0F, 2.f);
				setIsoPosition(i_x, i_y - 1, i_z, CUBE_HIGH_1F, 2.f);
				setIsoPosition(i_x, i_y - 1, i_z, CUBE_HIGH_2F, 2.f);
				setIsoPosition(i_x, i_y - 1, i_z, CUBE_HIGH_3F, 2.f);
			}

			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool damageCube(int i_x, int i_y, int i_z, int i_hit)
///
/// Fonction permettant d'endomager un cube
/// 
/// @param[in] i_x : Numéro du cube en x
/// @param[in] i_y : Numéro du cube en y (vertical)
/// @param[in] i_z : Numéro du cube en z
/// @param[in] i_hit : Nombre de hitpoint a enlever
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
float Terrain::damageCube(int i_x, int i_y, int i_z, float i_hit)
{
	//On trouve la position du cube par rapport à la chunk
	int cubeX = i_x % mk_chunkLength;
	int cubeY = i_y % mk_chunkLength;
	int cubeZ = i_z % mk_chunkLength;

	//On trouve la chunk du cube
	int chunkX = i_x/mk_chunkLength;
	int chunkY = i_y/mk_chunkLength;
	int chunkZ = i_z/mk_chunkLength;

	//On vérifie si le cube est dans les limites
	if(isInLimit(i_x, i_y, i_z))
	{
		//On endommage le cube
		bool isNeighborReady(true);
		std::vector<bool> removableCubeTops;

		int currentTopY(i_y + 1);
		removableCubeTops.push_back(isNatural(i_x, currentTopY, i_z) && !isFull(i_x, currentTopY, i_z) && isSmoothTopRenderable(i_x, currentTopY, i_z));

		//On trouve tout les voxels qui ne sont pas assez solid pour rester si le cube qu'on endomage est détruit
		while(removableCubeTops[removableCubeTops.size()-1])
		{
			++currentTopY;
			removableCubeTops.push_back(isNatural(i_x, currentTopY, i_z) && !isFull(i_x, currentTopY, i_z) && isSmoothTopRenderable(i_x, currentTopY, i_z));
		}
		//On enleve le dernier cube qui est nécessairement à false
		removableCubeTops.pop_back();

		//On endomage tout les cubes de la colonne
		for(int i(0), removableCubeTopsSize(removableCubeTops.size()); i < removableCubeTopsSize; ++i)
		{
			if(removableCubeTops[i])
				isNeighborReady &= damageCubePrivate(i_x, i_y + 1 + i, i_z, i_hit);
		}

		//On endomage le cube de départ
		float remainingHitpoint(m4p_chunkMap[chunkX][chunkY][chunkZ]->damageCube(cubeX, cubeY, cubeZ, i_hit));
		if(remainingHitpoint <= 0.f)
		{
			//Si tout les cubes non plus de point de vie
			if(isNeighborReady)
			{
				//On enleve tout les cubes
				for(int i(0), removableCubeTopsSize(removableCubeTops.size()); i < removableCubeTopsSize; ++i)
					removeCube(i_x, i_y + 1 + i, i_z);

				removeCube(i_x, i_y, i_z);

				//Si le cube est brisée, on le suprime
				std::hash_set<Position> cubeToRemoveList;

				//On detecte les voisins qui ne sont plus connecté
				removeUnconnectedCube(i_x + 1, i_y, i_z, cubeToRemoveList);
				removeUnconnectedCube(i_x - 1, i_y, i_z, cubeToRemoveList);
				removeUnconnectedCube(i_x, i_y, i_z + 1, cubeToRemoveList);
				removeUnconnectedCube(i_x, i_y, i_z - 1, cubeToRemoveList);

				removeUnconnectedCube(i_x, i_y + removableCubeTops.size(), i_z, cubeToRemoveList);

				removeUnconnectedCube(i_x, i_y - 1, i_z, cubeToRemoveList);

				//On ajoute tout les cubes voisin à ceux qui ne sont plus connecté de facon récursive
				addNeighbor(cubeToRemoveList);

				for(std::hash_set<Position>::iterator it(cubeToRemoveList.begin()); it != cubeToRemoveList.end(); ++it)
				{
					removeCube(it->x, it->y, it->z);
				}
				return remainingHitpoint;
			}
		}
	}
	return m4p_chunkMap[chunkX][chunkY][chunkZ]->getCubeHitpoint(cubeX, cubeY, cubeZ);
}


float Terrain::repairCube(int i_x, int i_y, int i_z, float i_hitpoint)
{
	//On vérifie si le cube est dans la plage
	if(isInLimit(i_x, i_y, i_z))
	{
		//On accède au cube
		return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->repairCube(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_hitpoint);
	}
	//Si le cube est hors de la plage en envoi zéro (cube d'air) 
	return i_hitpoint;
}

bool Terrain::upgradeCube(int i_x, int i_y, int i_z)
{
	//On vérifie si le cube est dans la plage
	if(isInLimit(i_x, i_y, i_z))
	{
		//On accède au cube
		bool isUpgraded(m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->upgradeCube(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength));
		if(isUpgraded)
			setChangedFlag(i_x, i_y, i_z, true);
		return isUpgraded;
	}
	//Si le cube est hors de la plage en envoi zéro (cube d'air) 
	return false;
}

int Terrain::getTopWalkable(int i_x, int i_z)
{
	for(int i(m_height * CHUNKLENGTH); i >= 0; --i)
	{
		if(isWalkable(i_x, i, i_z))
			return i;
	}
	return -1;
}


float Terrain::getCubeHitpoint(int i_x, int i_y, int i_z)
{
	//On vérifie si le cube est dans la plage
	if(isInLimit(i_x, i_y, i_z))
	{
		//On accède au cube
		return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->getCubeHitpoint(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength);
	}
	//Si le cube est hors de la plage en envoi zéro (cube d'air) 
	return 0.f;
}

float Terrain::getCubeMaxHitpoint(int i_x, int i_y, int i_z)
{
	//On vérifie si le cube est dans la plage
	if(isInLimit(i_x, i_y, i_z))
	{
		//On accède au cube
		return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->getCubeMaxHitpoint(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength);
	}
	//Si le cube est hors de la plage en envoi zéro (cube d'air) 
	return 0.f;
}


////////////////////////////////////////////////////////////////////////
///
/// @fn bool addNeighbor(std::hash_set<Position>& i_cubeToRemoveList, int i_x, int i_y, int i_z)
///
/// Fonction permettant permettant d'ajouter un cube a une liste s'il est dans les limites et n'est pas de l'air
/// 
/// @param[in] i_cubeToRemoveList : Liste à remplir
/// @param[in] i_x : Numéro du cube en x
/// @param[in] i_y : Numéro du cube en y (vertical)
/// @param[in] i_z : Numéro du cube en z
///
/// @return Vrai si le cube était valide
///
////////////////////////////////////////////////////////////////////////
bool Terrain::addNeighbor(std::hash_set<Position>& i_cubeToRemoveList, int i_x, int i_y, int i_z)
{
	Position position;
	if(isInLimit(i_x, i_y, i_z) && !isAir(i_x, i_y, i_z))
	{
		position.x = i_x;
		position.y = i_y;
		position.z = i_z;
		return i_cubeToRemoveList.insert(position).second;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void addNeighbor(std::hash_set<Position>& i_cubeToRemoveList, int i_x, int i_y, int i_z)
///
/// Fonction permettant permettant de trouver tous les cubes connecté à une liste de cube.
/// 
/// @param[in] i_cubeToRemoveList : Liste à remplir
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::addNeighbor(std::hash_set<Position>& i_cubeToRemoveList)
{
	bool cubeAdded(false);

	for(std::hash_set<Position>::iterator it(i_cubeToRemoveList.begin()); it != i_cubeToRemoveList.end(); ++it)
	{
		cubeAdded |= addNeighbor(i_cubeToRemoveList, it->x + 1, it->y, it->z);
		cubeAdded |= addNeighbor(i_cubeToRemoveList, it->x - 1, it->y, it->z);
		cubeAdded |= addNeighbor(i_cubeToRemoveList, it->x, it->y + 1, it->z);
		cubeAdded |= addNeighbor(i_cubeToRemoveList, it->x, it->y - 1, it->z);
		cubeAdded |= addNeighbor(i_cubeToRemoveList, it->x, it->y, it->z + 1);
		cubeAdded |= addNeighbor(i_cubeToRemoveList, it->x, it->y, it->z - 1);
	}
	if(cubeAdded)
		addNeighbor(i_cubeToRemoveList);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void removeUnconnectedCube(int i_x, int i_y, int i_z, std::hash_set<Position>& i_cubeToRemoveList)
///
/// Fonction permettant permettant de trouver les cubes qui ne sont plus connecté au sol si l'on retire le cube entré en paramètre
/// 
/// @param[in] i_x : Numéro du cube en x
/// @param[in] i_y : Numéro du cube en y (vertical)
/// @param[in] i_z : Numéro du cube en z
/// @param[in] i_cubeToRemoveList : Liste à contenant les cubes non-connectés de départ
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::removeUnconnectedCube(int i_x, int i_y, int i_z, std::hash_set<Position>& i_cubeToRemoveList)
{
	//On vérifie si le cube est hors limites
	if(!isInLimit(i_x, i_y, i_z))
		return;
	//On vérifie si le cube est de l'air
	if(isAir(i_x, i_y, i_z))
		return;
	//On vérifie si le cube est connecté
	AStar aStar;
	aStar.init(Vector3I(i_x, i_y, i_z), Vector3I(0, 0, 0), INNER_TYPE);

	Node* node;
	while(!aStar.getIsDone())
	{
		node = aStar.executeSearch();
	}

	if(node->getCubePosition().x == -1)
	{
		Position position;
		position.x = i_x;
		position.y = i_y;
		position.z = i_z;
		i_cubeToRemoveList.insert(position);
	}

}

////////////////////////////////////////////////////////////////////////
///
/// @fn void removeCube(Vector& i_cube)
///
/// Fonction permettant de detruire un cube
/// 
/// @param[in] i_cube : cube à détruire
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::removeCube(Vector3I& i_cube)
{
	removeCube( i_cube.x, i_cube.y, i_cube.z );
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void createCube(Vector3I& i_cube, int i_type, int i_resist)
///
/// Fonction permettant d'ajouter un cube 
/// 
/// @param[in] i_cube : position du cube à ajouter
/// @param[in] i_type : position du cube à ajouter
/// @param[in] i_resist : resistance du cube à ajouter
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Terrain::createCube(Vector3I& i_cube, int i_type, float i_hitpoint, float i_maxHitpoint)
{
	return createCube( i_cube.x, i_cube.y, i_cube.z, i_type, i_hitpoint, i_maxHitpoint);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void removeCube(int i_x, int i_y, int i_z)
///
/// Fonction permettant de detruire un cube avec update
/// 
/// @param[in] i_x : Numéro du cube en x
/// @param[in] i_y : Numéro du cube en y (vertical)	
/// @param[in] i_z : Numéro du cube en z
///
/// @return 
///
////////////////////////////////////////////////////////////////////////
bool Terrain::removeCube(int i_x, int i_y, int i_z)
{
	//On vérifie si le cube existe et on trouve sa véritable position dans les chunks
    if(isInLimit(i_x, i_y, i_z))
	{
		lastDestroyedCube.x = i_x;
		lastDestroyedCube.y = i_y;
		lastDestroyedCube.z = i_z;
		setChangedFlag(i_x, i_y, i_z, true);
		setChangedFlag(i_x, i_y+1, i_z, true);
		setChangedFlag(i_x, i_y - 1, i_z, true);
		setChangedFlag(i_x+1, i_y, i_z, true);
		setChangedFlag(i_x-1, i_y, i_z, true);
		setChangedFlag(i_x, i_y, i_z+1, true);
		setChangedFlag(i_x, i_y, i_z-1, true);
		

		if(isArtificial(i_x+1, i_y, i_z+1))
			setChangedFlag(i_x+1, i_y, i_z+1, true);
		if(isArtificial(i_x-1, i_y, i_z+1))
			setChangedFlag(i_x-1, i_y, i_z+1, true);
		if(isArtificial(i_x+1, i_y, i_z-1))
			setChangedFlag(i_x+1, i_y, i_z-1, true);
		if(isArtificial(i_x-1, i_y, i_z-1))
			setChangedFlag(i_x-1, i_y, i_z-1, true);

        m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->removeCube(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength);
	
		return true;
	}
	else
		return false;
}
////////////////////////////////////////////////////////////////////////
///
/// @fn void draw(Matrix& ik_projection, Matrix& i_modelview, Camera& ik_camera)
///
/// Fonction permettant d'afficher les chunks devant être affichées
/// 
/// @param[in] ik_projection : Matrix de projection
/// @param[in] i_modelview : Matrix de modelview	
/// @param[in] ik_camera : Caméra contenant la position du joueur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)const
{	
	//On choisi le shader du terrain
	Shader::chooseShader(GEOM_PASS_SHADER);
	Shader* p_shader(Shader::getShader());
	OpenGLHandler::cullFace(GL_BACK);
	//On active les texture des cubes
	Texture::chooseTexture(m_texturesIndex[0]);
	Texture::linkTexture();
	Texture::bindTexture();

	//On envoie le nombre de texture total
	glUniform1i(p_shader->getUniformVariableSafe("InExecutionType"), TERRAIN);
	glUniform1i(p_shader->getUniformVariable("NbTextureTot"), Texture::getImageCount());

	glUniform1f(p_shader->getUniformVariableSafe("InSpecularPower"), 0.0f);
	glUniform1f(p_shader->getUniformVariableSafe("InMatSpecularIntensity"), 0.0f);

	//On determine dans quel chunk la caméra se situe
	Vector cameraPosition(ipk_camera->getPosition());
	int chunkX = (int)(cameraPosition.x/(SIDE*mk_chunkLength)), 
		chunkY = (int)(cameraPosition.y/(SIDE*mk_chunkLength)), 
		chunkZ = (int)(cameraPosition.z/(SIDE*mk_chunkLength));

	//On affiche les chunks à afficher
    for(int x(chunkX-PRINTEDCHUNK < 0?0:chunkX-PRINTEDCHUNK), maxX(chunkX+PRINTEDCHUNK >= m_width? m_width:chunkX+PRINTEDCHUNK); x < maxX; ++x)
        for(int y(chunkY-PRINTEDCHUNK < 0?0:chunkY-PRINTEDCHUNK), maxY(chunkY+PRINTEDCHUNK >= m_height? m_height:chunkY+PRINTEDCHUNK); y < maxY; ++y)
            for(int z(chunkZ-PRINTEDCHUNK < 0?0:chunkZ-PRINTEDCHUNK), maxZ(chunkZ+PRINTEDCHUNK >= m_length? m_length:chunkZ+PRINTEDCHUNK); z < maxZ; ++z)
                m4p_chunkMap[x][y][z]->draw(ik_projection, i_modelview, ipk_camera);
	OpenGLHandler::cullFace(GL_FRONT);
	getGlError();	
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void createCubePrivate(int i_x, int i_y, int i_z, int i_resist)
///
/// Fonction interne permettant de créer un cube sans effectuer aucun update
/// 
/// @param[in] i_x : Numéro du cube en x
/// @param[in] i_y : Numéro du cube en y (vertical)	
/// @param[in] i_z : Numéro du cube en z
/// @param[in] i_type : Type du cube
/// @param[in] i_resist : Résistance du cube
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Terrain::createCubePrivate(int i_x, int i_y, int i_z, int i_type, float i_hitpoint, float i_maxHitPoint, float ia_density[8])
{

    //On vérifie si l'emplacement peut recevoir un cube et s'il est dans la plage
    if(isInLimit(i_x, i_y, i_z))
	{
		//On crée le cube
        m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->createCube(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_type, i_hitpoint, i_maxHitPoint, ia_density);
		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn int getHighestCube(int i_x, int i_z)const
///
/// Fonction retournant le plus haut cube à une position en x et en z
/// 
/// @param[in] i_x : Numéro du cube en x
/// @param[in] i_z : Numéro du cube en z
///
/// @return Position en y du cube
///
////////////////////////////////////////////////////////////////////////
int Terrain::getHighestCube(int i_x, int i_z)const
{
	//On trouve le premier cube solide à la position donnée
	for(int i(m_height * mk_chunkLength); i >= 0; --i )
		if(!isAir(i_x, i, i_z))
			return i;
	
	//On retourne -1 si il n'y a aucun cube solide
	return -1;
}

bool Terrain::isFull(int i_x, int i_y, int i_z)const
{
	//On vérifie si le cube est dans la plage
	if(isInLimit(i_x, i_y, i_z))
	{
		return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->isFull(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength);
	}
	return false;
}

bool Terrain::isBottomFilled(int i_x, int i_y, int i_z)const
{
	//On vérifie si le cube est dans la plage
	if(isInLimit(i_x, i_y, i_z))
	{
		return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->isBottomFilled(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength);
	}
	return false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool isSolidOrOutside(int i_x, int i_y, int i_z)const
///
/// On test si le cube est solide ou à l'extérieur de la limite
/// 
/// @param[in] i_x : Numéro du cube en x
/// @param[in] i_z : Numéro du cube en y (verticial)
/// @param[in] i_z : Numéro du cube en z
///
/// @return Vrai si le cube est à l'extérieur ou est solide
///
////////////////////////////////////////////////////////////////////////
bool Terrain::isNaturalOrOutside(int i_x, int i_y, int i_z)const
{
	//On vérifie si le cube est dans la plage
	if(isInLimit(i_x, i_y, i_z))
		return getCubeType(i_x, i_y, i_z) != 0;
	else
		//On envoie vrai s'il est à l'extérieur
		return true;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn int getCube(int i_x, int i_y, int i_z)const
///
/// On obtient le type du cube
/// 
/// @param[in] i_x : Numéro du cube en x
/// @param[in] i_z : Numéro du cube en y (verticial)
/// @param[in] i_z : Numéro du cube en z
///
/// @return Le numéro du type du cube
///
////////////////////////////////////////////////////////////////////////
int Terrain::getCubeType(int i_x, int i_y, int i_z)const
{
	//On vérifie si le cube est dans la plage
	if(isInLimit(i_x, i_y, i_z))
	{
		//On accède au cube
		return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->getCube(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength);
	}
	//Si le cube est hors de la plage en envoi zéro (cube d'air) 
	return 0;
}
bool Terrain::isHigherThen(int i_x, int i_y, int i_z,  FaceType i_faceType)const
{
	//On vérifie si le cube est dans la plage
	if(isInLimit(i_x, i_y, i_z))
	{
		//On accède au cube
		return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->isHigherThen(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_faceType);
	}
	//Si le cube est hors de la plage en envoi zéro (cube d'air) 
	return false;
}

bool Terrain::isSmoothTopRenderable(int i_x, int i_y, int i_z)const
{
	//On vérifie si le cube est dans la plage
	if(isInLimit(i_x, i_y, i_z))
	{
		//On accède au cube
		return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->isSmoothTopRenderable(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength);
	}
	//Si le cube est hors de la plage en envoi zéro (cube d'air) 
	return false;
}

bool Terrain::isFlatTopRenderable(int i_x, int i_y, int i_z)const
{
	//On vérifie si le cube est dans la plage
	if(isInLimit(i_x, i_y, i_z) && isNatural(i_x, i_y, i_z))
	{
		//On accède au cube
		return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->isFlatTopRenderable(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength);
	}
	//Si le cube est hors de la plage en envoi zéro (cube d'air) 
	return false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void animate(Uint32 i_timestep)
///
/// Ajout de l'effet du temps sur le terrain (modification du vecteur de lumière)
/// 
/// @param[in] i_timestep : différence de temps depuis la dernière animation
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::animate(Uint32 i_timestep)
{
	Matrix matrix;
	matrix.loadIdentity();
	matrix.rotate(SKYBOX_ROTATION_SPEED * i_timestep, 0.0f, 1.0f, 0.0f);
	Input* p_input(Input::getInstance());
	if(p_input->getKeyPress("wireMode") && p_input->getInputState() != CONSOLE_INPUT)
	{
		for(int x(0), maxX(m_width); x < maxX; ++x)
			for(int y(0), maxY(m_height); y < maxY; ++y)
				for(int z(0), maxZ(m_length); z < maxZ; ++z)
					if(m_showWire)
						DeferredRenderer::getInstance()->removeWhiteLine(m4p_chunkMap[x][y][z]->getWhiteLine());
					else
						DeferredRenderer::getInstance()->addWhiteLine(m4p_chunkMap[x][y][z]->getWhiteLine());
		m_showWire = !m_showWire;
	}

	updateAllChunk();

	if(m_dynamicsWorld != 0)
		if (i_timestep < 100)
			m_dynamicsWorld->stepSimulation(i_timestep/1000.0f);
		else
			m_dynamicsWorld->stepSimulation(100/1000.0f);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn std::vector<Vector3I> getCollision(const Vector& i_sourceVect, const Vector& i_targetVect, bool i_onlyFirst)
///
/// Fonction retournant le premier cube touché par un rayon
/// 
/// @param[in] i_sourceVect : point de départ du rayon
/// @param[in] i_targetVect : point d'arrivé du rayon
/// @param[in] i_onlyFirst : Spécifie si on retourne un seul cube ou plusieurs
///
/// @return List de la position des cubes retournés
///
////////////////////////////////////////////////////////////////////////
std::vector<Vector3I> Terrain::getCollision(const Vector& i_sourceVect, const Vector& i_targetVect, bool i_onlyFirst)
{Vector3I currentCube(Terrain::positionToCube(i_sourceVect));
	Vector3I targetCube(Terrain::positionToCube(i_targetVect));

	//On trouve la direction vers où itérer
	Vector direction(i_targetVect - i_sourceVect);
	direction.normalize();


	//On trouve la distance maximal de deplacement pour ne pas raté de cube.
	float tDeltaX(abs(SIDE/direction.x));
	float tDeltaY(abs(SIDE/direction.y));
	float tDeltaZ(abs(SIDE/direction.z));

	//On trouve la direction 
	int editPositionX;
	int editPositionY;
	int editPositionZ;

	if(i_sourceVect.x < i_targetVect.x)
		editPositionX = 1;
	else
		editPositionX = -1;

	if(i_sourceVect.y < i_targetVect.y)
		editPositionY = 1;
	else
		editPositionY = -1;

	if(i_sourceVect.z < i_targetVect.z)
		editPositionZ = 1;
	else
		editPositionZ = -1;

	//De combien doit ton longer direction pour arriver à la limite du voxel 
	float tMaxX(abs(((currentCube.x * SIDE + HALFSIDE * editPositionX) - i_sourceVect.x)/direction.x));
	float tMaxY(abs(((currentCube.y * SIDE + HALFSIDE * editPositionY) - i_sourceVect.y)/direction.y));
	float tMaxZ(abs(((currentCube.z * SIDE + HALFSIDE * editPositionZ) - i_sourceVect.z)/direction.z));

	std::vector<Vector3I> result;
	HitBox hitBoxCube(HALFSIDE, HALFSIDE, HALFSIDE);
	Vector center;

	do
	{
		//On teste si la voxel actuel est en collision
		if(!isAir(currentCube.x, currentCube.y, currentCube.z))
		{
			center.x = currentCube.x * SIDE;
			center.y = currentCube.y * SIDE;
			center.z = currentCube.z * SIDE;
			hitBoxCube.setPosition(center);
			if(hitBoxCube.rayCollision(i_sourceVect, i_targetVect))
			{
				result.push_back(currentCube);
				if(i_onlyFirst)
					return result;
			}
		}

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
	while(((currentCube.x <= targetCube.x && editPositionX == 1) || (currentCube.x >= targetCube.x && editPositionX == -1)) && 
		  ((currentCube.y <= targetCube.y && editPositionY == 1) || (currentCube.y >= targetCube.y && editPositionY == -1))  && 
		  ((currentCube.z <= targetCube.z && editPositionZ == 1) || (currentCube.z >= targetCube.z && editPositionZ == -1)) );
	return result;
	
}


////////////////////////////////////////////////////////////////////////
///
/// @fn void getMapColor(int i_x, int i_z, unsigned char i_colors[3])
///
/// Fonction retournant la couleur d'une position 2D à afficher dans la map
/// 
/// @param[in] i_x : position absolut de la colonne en x
/// @param[in] i_z : position absolut de la colonne en z
/// @param[in] i_colors : couleur trouvée
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::getMapColor(std::vector<unsigned char>& i_mapOverview)
{
	i_mapOverview.resize(m_height * mk_chunkLength * m_width * mk_chunkLength * MAP_PIXEL_SIZE);
	unsigned char color[3];
	for(int i(0); i < m_width * mk_chunkLength * MAP_PIXEL_SIZE; ++i)
		for(int j(0); j < m_length * mk_chunkLength * MAP_PIXEL_SIZE; ++j)
		{
			getMapOverviewSingleColor(i/MAP_PIXEL_SIZE, j/MAP_PIXEL_SIZE, color);
			i_mapOverview[i + (j*(m_width * mk_chunkLength * MAP_PIXEL_SIZE))] = color[(i + (j*(m_height * mk_chunkLength * MAP_PIXEL_SIZE)))%3];
		}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result)
///
/// Fonction retournant les sommets reliés à un seul voxel
/// 
/// @param[in] i_x : position absolut du cube en x
/// @param[in] i_y : position absolut du cube en y
/// @param[in] i_z : position absolut du cube en z
/// @param[in] i_result : sommets trouvés
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result)
{	
	if(isInLimit(i_x, i_y, i_z))
	{
		m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->getVertex(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_result);
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result, std::vector<VertexPosition>& i_oldVertexList )
///
/// Fonction retournant les sommets reliés à un seul voxel utilisant les anciennes données 
/// 
/// @param[in] i_x : position absolut du cube en x
/// @param[in] i_y : position absolut du cube en y
/// @param[in] i_z : position absolut du cube en z
/// @param[in] i_result : sommets trouvés
/// @param[in] i_oldVertexList : données de l'ancienne itération
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result, std::vector<VertexPosition>& i_oldVertexList)
{	
	if(isInLimit(i_x, i_y, i_z))
	{
		m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->getVertex(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_result, i_oldVertexList);
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool damageCubePrivate(int i_x, int i_y, int i_z, int i_hit)
///
/// On endomage le cube d'une chunk en particulier
/// 
/// @param[in] i_x : position absolut du cube en x
/// @param[in] i_y : position absolut du cube en y
/// @param[in] i_z : position absolut du cube en z
/// @param[in] i_hit : nombre de point de vie à enlever
///
/// @return bool spécifiant si le cube doit être détruit
///
////////////////////////////////////////////////////////////////////////
bool Terrain::damageCubePrivate(int i_x, int i_y, int i_z, float i_hit)
{

    //On vérifie si l'emplacement peut recevoir un cube et s'il est dans la plage
    if(i_x >= 0 && i_x < m_width*mk_chunkLength && i_y >=0 && i_y< m_height*mk_chunkLength && i_z >= 0 && i_z < m_length*mk_chunkLength)
	{
		//On crée le cube
        m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->damageCube(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_hit);
		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void setChangedFlag(int i_x, int i_y, int i_z, bool i_value)
///
/// On modifie le flag de changement d'un cube
/// 
/// @param[in] i_x : position absolut du cube en x
/// @param[in] i_y : position absolut du cube en y
/// @param[in] i_z : position absolut du cube en z
/// @param[in] i_value : la nouvelle valeur du flag
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::setChangedFlag(int i_x, int i_y, int i_z, bool i_value)
{
	//On vérifie si le cube existe et on trouve sa véritable position dans les chunks
    if(isInLimit(i_x, i_y, i_z))
	{
        m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->setChangedFlag(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_value);
		addToUpdateList(i_x/mk_chunkLength, i_y/mk_chunkLength, i_z/mk_chunkLength);
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn float getIsoPosition(int i_x, int i_y, int i_z, unsigned int i_index)
///
/// Fonction permettant de récupérer la position relative de l'isosurface d'un coin par apport au cube
/// 
/// @param[in] i_x : position absolut du cube en x
/// @param[in] i_y : position absolut du cube en y
/// @param[in] i_z : position absolut du cube en z
/// @param[in] i_index : index du sommet en question
///
/// @return Position relative de l'isosurface
///
////////////////////////////////////////////////////////////////////////
float Terrain::getIsoPosition(int i_x, int i_y, int i_z, unsigned int i_index)
{
	if(isInLimit(i_x, i_y, i_z))
	{
		return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->getIsoSurfaceRelativeHeight(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_index);
	}
	return -1.0f;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void modifySingleIsoPosition(int i_x, int i_y, int i_z, unsigned int i_index, float i_isoPosition)
///
/// Fonction permettant de modifier la position relative de l'isosurface d'un coin par apport au cube
/// 
/// @param[in] i_x : position absolut du cube en x
/// @param[in] i_y : position absolut du cube en y
/// @param[in] i_z : position absolut du cube en z
/// @param[in] i_index : index du sommet en question
/// @param[in] i_isoPosition : nouvelle valeur de l'isosuface
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::setIsoPosition(int i_x, int i_y, int i_z, unsigned int i_index, float i_isoPosition)
{
	assert(CUBE_HIGH_0F <= i_index && i_index <= CUBE_HIGH_3F);

	if(isInLimit(i_x, i_y, i_z))
	{
		m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->setIsoSurfaceRelativeHeight(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_index, i_isoPosition);
	}

	setChangedFlag(i_x, i_y, i_z, true);
	setChangedFlag(i_x + 1, i_y, i_z, true);
	setChangedFlag(i_x - 1, i_y, i_z, true);
	setChangedFlag(i_x, i_y, i_z + 1, true);
	setChangedFlag(i_x, i_y, i_z - 1, true);
}
void Terrain::setRenforcement(int i_x, int i_y, int i_z, int i_renforcement)
{
	if(isInLimit(i_x, i_y, i_z))
	{
		m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->setRenforcement(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_renforcement);
		setChangedFlag(i_x, i_y, i_z, true);
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void addToUpdateList(int i_x, int i_y, int i_z)
///
/// Fonction permettant d'ajouter le chunk d'un cube à la liste des chunks à mettre à jour
/// 
/// @param[in] i_x : position absolut du cube en x
/// @param[in] i_y : position absolut du cube en y
/// @param[in] i_z : position absolut du cube en z
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::addToUpdateList(int i_x, int i_y, int i_z)
{
	if(0 <= i_x && i_x < m_width && 0 <= i_y && i_y < m_height && 0 <= i_z && i_z < m_length)
	{
		Vector3I position;
		position.x = i_x;
		position.y = i_y;
		position.z = i_z;
		m_chunksToUpdate.insert(position);
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void updateAllChunk()
///
/// Fonction permettant de mettre à jour tout les chunk devant être mis à jour
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Terrain::updateAllChunk()
{
	if(!m_chunksToUpdate.empty())
	{
		//On update toute la liste
		for(std::hash_set<Vector3I>::iterator it(m_chunksToUpdate.begin()), chunksToUpdateEnd(m_chunksToUpdate.end()); it != chunksToUpdateEnd; ++it)
		{
			m4p_chunkMap[it->x][it->y][it->z]->updateAndSetBtObject(m_dynamicsWorld);
		}

		std::hash_set<Vector3I> emptySet;
		m_chunksToUpdate.swap(emptySet);
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn int switchCubeType(int i_x, int i_y, int i_z, int i_newType)
///
/// Fonction permettant de modifier temporairement le type d'un cube sans modifier l'affichage
/// Utilisé pour le A*
/// 
/// @param[in] i_x : position absolut du cube en x
/// @param[in] i_y : position absolut du cube en y
/// @param[in] i_z : position absolut du cube en z
/// @param[in] i_newType : type temporaire assigé au cube
///
/// @return Ancien type du cube
///
////////////////////////////////////////////////////////////////////////
int Terrain::switchCubeType(int i_x, int i_y, int i_z, int i_newType)
{
	//On vérifie si le cube existe et on trouve sa véritable position dans les chunks
    if(isInLimit(i_x, i_y, i_z))
	{
        return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->switchCubeType(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_newType);
	}
	else
		return 0;
}

//Physique

//initialise le monde
void Terrain::initPhysics()
{
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	m_broadphase = new btAxisSweep3(worldMin,worldMax);
	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
	m_dynamicsWorld->getSolverInfo().m_splitImpulse=true;	
}


void Terrain::exitPhysics()
{
	if(m_dynamicsWorld == 0)
		return;
	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject( obj );
		delete obj;
		obj = 0;
	}

	//delete dynamics world
	delete m_dynamicsWorld;

	//delete solver
	delete m_solver;
	m_solver = 0;

	//delete broadphase
	delete m_broadphase;
	m_broadphase = 0;

	//delete dispatcher
	delete m_dispatcher;
	m_dispatcher = 0;

	//delete configuration
	delete m_collisionConfiguration;
	m_collisionConfiguration = 0;
}

void Terrain::resetPhysics()
{
	if(m_dynamicsWorld == 0)
		return;

	delete m_dynamicsWorld;
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
	m_dynamicsWorld->getSolverInfo().m_splitImpulse=true;

	UnitHandler::getInstance()->reloadPhysic();	
}
bool Terrain::saveChunk(const Chunk& i_chunk, const std::string& ik_filePath)
{
	Vector3I cubePosition(i_chunk.getCubePosition());
	return saveChunk(cubePosition, Vector3I(cubePosition.x + i_chunk.getSizeX(), cubePosition.y + i_chunk.getSizeY(), cubePosition.z + i_chunk.getSizeZ()), ik_filePath);
}
bool Terrain::saveChunk(const Vector3I& i_corner1, const Vector3I& i_corner2, const std::string& ik_filePath)
{
	Vector3I downRightFront;
	Vector3I upLeftBack;

	if(i_corner1.x < i_corner2.x)
	{
		downRightFront.x = i_corner1.x;
		upLeftBack.x = i_corner2.x;
	}
	else
	{
		downRightFront.x = i_corner2.x;
		upLeftBack.x = i_corner1.x;
	}

	if(i_corner1.y < i_corner2.y)
	{
		downRightFront.y = i_corner1.y;
		upLeftBack.y = i_corner2.y;
	}
	else
	{
		downRightFront.y = i_corner2.y;
		upLeftBack.y = i_corner1.y;

	}

	if(i_corner1.z < i_corner2.z)
	{
		downRightFront.z = i_corner1.z;
		upLeftBack.z = i_corner2.z;
	}
	else
	{
		downRightFront.z = i_corner2.z;
		upLeftBack.z = i_corner1.z;
	}

	Vector3I chunKSize(upLeftBack.x - downRightFront.x, 
  					upLeftBack.y - downRightFront.y, 
					upLeftBack.z - downRightFront.z);
		
	Chunk chunkToSave(downRightFront.x, downRightFront.y, downRightFront.z, chunKSize.x, chunKSize.y, chunKSize.z, false);

	chunkToSave.copyTerrain();

	std::ofstream ofstreamFile;
	ofstreamFile.open("Map/" + ik_filePath, std::ios::binary);
	chunkToSave.saveMap(ofstreamFile);
	ofstreamFile.close();

	return true;
}

Chunk* Terrain::loadChunk(const Vector3I& i_position, const std::string& ik_filePath)
{
	std::ifstream ifstreamFile;
	ifstreamFile.open("Map/" + ik_filePath, std::ios::binary);
	
	Chunk* newChunk(new Chunk(-1, -1, -1, 0,0,0, false));
	
	//On lit les données d'entêtes
	short headerChunk;
	readFromFile(ifstreamFile, headerChunk);

	assert(headerChunk == HEADER_CHUNK);

	//Nous n'utilisons pas ces informations dans ce cas précis
	Vector3I chunkPosition;
	readFromFile(ifstreamFile, chunkPosition.x);
	readFromFile(ifstreamFile, chunkPosition.y);
	readFromFile(ifstreamFile, chunkPosition.z);

	newChunk->loadMap(ifstreamFile);
	newChunk->updateBuffer();
	newChunk->moveChunk(i_position);

	return newChunk;
}
const std::vector<int>* Terrain::getFullCube(int i_x, int i_y, int i_z)const
{
	if(isInLimit(i_x, i_y, i_z))
	{
        return m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->getFullCube(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength);
	}

	return 0;
}

void Terrain::setFullCube(int i_x, int i_y, int i_z)
{
	if(isInLimit(i_x, i_y, i_z))
	{
        m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->setFullCube(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength);

		setChangedFlag(i_x, i_y, i_z, true);
		setChangedFlag(i_x + 1, i_y, i_z, true);
		setChangedFlag(i_x - 1, i_y, i_z, true);
		setChangedFlag(i_x, i_y + 1, i_z, true);
		setChangedFlag(i_x, i_y - 1, i_z, true);
		setChangedFlag(i_x, i_y, i_z + 1, true);
		setChangedFlag(i_x, i_y, i_z - 1, true);
	}
}

void Terrain::getDensity(int i_x, int i_y, int i_z, float i_density[8])
{
	if(isInLimit(i_x, i_y, i_z))
	{
        m4p_chunkMap[i_x/mk_chunkLength][i_y/mk_chunkLength][i_z/mk_chunkLength]->getDensity(i_x%mk_chunkLength, i_y%mk_chunkLength, i_z%mk_chunkLength, i_density);
	}
}

void Terrain::setMapOverviewSingleColor(int i_x, int i_z, unsigned char i_color[3])
{
	m_mapOverview[(i_x + i_z * m_width * mk_chunkLength) * 3 + 0] = i_color[0];
	m_mapOverview[(i_x + i_z * m_width * mk_chunkLength) * 3 + 1] = i_color[1];
	m_mapOverview[(i_x + i_z * m_width * mk_chunkLength) * 3 + 2] = i_color[2];
	((MapGUI*)GUIHandler::getInstance()->getLayout(MAP_LAYOUT))->addPixelToUpdate(i_x, i_z);
}

void Terrain::getMapOverviewSingleColor(int i_x, int i_z, unsigned char i_color[3])
{
	i_color[0] = m_mapOverview[(i_x + i_z * m_width * mk_chunkLength) * 3 + 0];
	i_color[1] = m_mapOverview[(i_x + i_z * m_width * mk_chunkLength) * 3 + 1];
	i_color[2] = m_mapOverview[(i_x + i_z * m_width * mk_chunkLength) * 3 + 2];
}


void Terrain::updateMapOverview(int i_chunkX, int i_chunkZ, int i_x, int i_z)
{
	int realX = i_chunkX * mk_chunkLength + i_x;
	int realZ = i_chunkZ * mk_chunkLength + i_z;
	unsigned char color[3];

	Position position;
	position.x = realX;
	position.z = realZ;

		Vector source(Terrain::cubeToPosition(realX), 0, Terrain::cubeToPosition(realZ));
	Vector target(source);
	source.y = Terrain::cubeToPosition(m_height * mk_chunkLength);
	if(TemplateManager::getInstance()->templateInCollision(source, target) > 0)
	{
		color[0] = 255;
		color[1] = 0;
		color[2] = 0;
	}
	else
	{
		for(int i(m_height - 1); i >= 0 ; --i)
		{
			m4p_chunkMap[i_chunkX][i][i_chunkZ]->getMapColor(i_x, i_z, color);
			if(color[0] != 0 || color[1] != 0 || color[2] != 0)
				break;
		}
	}

	setMapOverviewSingleColor(realX, realZ, color);
}

void Terrain::reloadMapOverviewSingleColor(int i_realX, int i_realZ)
{
	unsigned char color[3];

	Position position;
	position.x = i_realX;
	position.z = i_realZ;

	Vector source(Terrain::cubeToPosition(i_realX), 0, Terrain::cubeToPosition(i_realZ));
	Vector target(source);
	source.y = Terrain::cubeToPosition(m_height * mk_chunkLength);
	if(TemplateManager::getInstance()->templateInCollision(source, target) > 0)
	{
		color[0] = 255;
		color[1] = 0;
		color[2] = 0;
	}
	else
	{
		for(int i(m_height - 1); i >= 0 ; --i)
		{
			m4p_chunkMap[Terrain::cubeToChunk(i_realX)][i][Terrain::cubeToChunk(i_realZ)]->getMapColor(Terrain::cubeToRelativeCube(i_realX), Terrain::cubeToRelativeCube(i_realZ), color);
			if(color[0] != 0 || color[1] != 0 || color[2] != 0)
				break;
		}
	}

	setMapOverviewSingleColor(i_realX, i_realZ, color);
}