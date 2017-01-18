#include "Chunk.h"

#include <algorithm>

#include "Terrain.h"
#include "../Math/Matrix.h"
#include "../Camera/Camera.h"
#include "../Shaders/Shader.h"
#include "../Position.h"
#include "MarchingCube.h"
#include "../Debugger/GlDebugger.h"
#include "../Serializer.h"
#include "../Rendering/Texture.h"
#include "../Rendering/WhitePrismeInstanciable.h"
#include "../Building/TemplateManager.h"
#include "../GUI/GUIHandler.h"
#include "../GUI/MapGUI.h"

#include <set>
#include <map>

#include "../Debugger/ProgTools.h"

#include "../Math/Matrix3x3.h"

#define SUP_UV (1.41f * HALFSIDE - HALFSIDE)
#define HALF_THIRD_SIDE (SIDE/6.0f)

enum LayerModifier{DEFINE_TOP = 1, DEFINE_BOTTOM = -1};


//static GLDebugDrawer sDebugDraw;

////////////////////////////////////////////////////////////////////////
///
/// @fn Chunk(int i_x, int i_y, int i_z)
///
/// Constructeur
/// 
/// @param[in] i_x : Numéro de la chunk en x
/// @param[in] i_y : Numéro de la chunk en y (vertical)	
/// @param[in] i_z : Numéro de la chunk en z
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Chunk::Chunk(int i_x, int i_y, int i_z, int i_sizeX, int i_sizeY, int i_sizeZ, const bool ik_isTerrainChunk)
:m_chunkPosition(i_x, i_y, i_z),
m_sizeX(i_sizeX), m_sizeY(i_sizeY), m_sizeZ(i_sizeZ),
m_cubePosition(i_sizeX * i_x, i_sizeY * i_y, i_sizeZ * i_z),
gVertices(0),
trimeshShape(0),
m_staticBody(0),
m_indexVertexArrays(0),
m_defaultContactProcessingThreshold(BT_LARGE_FLOAT),
m_whiteTerrainLine(this), Drawable(GEOM_PASS_SHADER),
mk_isTerrainChunk(ik_isTerrainChunk),
m_outlineRenderInformation(this)
{
	init();
}

void Chunk::init()
{
	if(!mk_isTerrainChunk)
	{
		m_texturesIndex.push_back(Texture::getTextureID("TextureSampler", "Texture/terrain.png"));

		updateOutline();
	}

	//On remplit la map de cube vide
	resizeChunk();

	// Génère les buffers requis
	glGenVertexArrays(1, &m_VAOID);  
	glBindVertexArray(m_VAOID);
	glGenBuffers(1, &m_buffers[INDEX_BUFFER]);
	glGenBuffers(1, &m_buffers[POS_VB]);
	glGenBuffers(1, &m_textureNbID);
	glGenBuffers(1, &m_damageID);
	

	glGenBuffers(1, &m_lineIBOID);
	glGenBuffers(1, &m_lineVBOID);

	glBindVertexArray(0);
}

void Chunk::updateOutline()
{
	if(!mk_isTerrainChunk)
	{
		Matrix matrix;
		matrix.loadIdentity();

		Vector translation(Terrain::cubeToPosition(m_cubePosition));
	
		matrix.translate(translation.x - HALFSIDE, translation.y - HALFSIDE, translation.z - HALFSIDE);
		matrix.scale(m_sizeX * SIDE, m_sizeY * SIDE, m_sizeZ * SIDE);

		m_outlineRenderInformation.setBaseTransform(matrix);
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Chunk()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Chunk::~Chunk()
{
	if(mk_isTerrainChunk)
		exitPhysics();
	else
		TemplateManager::getInstance()->getWhitePrismeInstanciable()->removeFromRenderInfoList(m_outlineRenderInformation);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn int generateColor(unsigned char i_red, unsigned char i_green, unsigned char i_blue, unsigned char i_alpha)
///
/// Focntion permettant de stocker 4 char dans un entier
/// 
/// @param[in] i_red : Canaux rouge
/// @param[in] i_green : Canaux vert
/// @param[in] i_blue : Canaux bleu
/// @param[in] i_alpha : Canaux alpha
///
/// @return Tout les canaux stockers dans un entier
///
////////////////////////////////////////////////////////////////////////
int generateColor(unsigned char i_red, unsigned char i_green, unsigned char i_blue, unsigned char i_alpha)
{
	int red(i_red);
	int green(i_green);
	int blue(i_blue);
	int alpha(i_alpha);

	red = red << 24;
	green = green << 16;
	blue = blue << 8;
	return red | green | blue | alpha;
}

void createEmptyCube(std::vector<int>& o_cubeValue)
{
	for(int l(0) ; l < NB_CUBE_PARAM ; ++l)
	{
		switch(l)
		{
			case CUBE_DRAWING_INDEX_EXTRA_TOP:
			case CUBE_DRAWING_INDEX:
				o_cubeValue[l] = -1;
			break;
			case CUBE_CHANGED:
				o_cubeValue[l] = true;
			break;
			case CUBE_COLOR:
				o_cubeValue[l] = generateColor(255, 255, 255, 4);
				break;
			default:
				o_cubeValue[l] = 0;
		}
	}
}
void Chunk::moveChunkBottomCenter(Vector3I i_position)
{
	i_position.x -= m_sizeX/2;
	i_position.z -= m_sizeZ/2;
	moveChunk(i_position);
}

void Chunk::moveChunk(int i_x, int i_y, int i_z)
{
	m_cubePosition.x = i_x;
	m_cubePosition.y = i_y;
	m_cubePosition.z = i_z;
	updateOutline();
}

void Chunk::moveChunk(const Vector3I& i_position)
{
	m_cubePosition = i_position;
	updateOutline();
}

void Chunk::resizeChunk(int i_sizeX, int i_sizeY, int i_sizeZ)
{
	m_sizeX = i_sizeX;
	m_sizeY = i_sizeY;
	m_sizeZ = i_sizeZ;

	resizeChunk();
}
void Chunk::resizeChunk()
{
	//On crée la map
	m_map.resize(m_sizeX);
	for(int i(0); i < m_sizeX; ++i)
	{
		m_map[i].resize(m_sizeY);
		for(int j(0); j < m_sizeY; ++j)
		{
			m_map[i][j].resize(m_sizeZ);
			for(int k(0); k < m_sizeZ; ++k)
				m_map[i][j][k].resize(NB_CUBE_PARAM);
		}
	}
	//On peuple la map
	for(int i(0); i < m_sizeX ; ++i)
        for(int j(0); j < m_sizeY ; ++j)
            for(int k(0); k < m_sizeZ ; ++k)
				createEmptyCube(m_map[i][j][k]);

	//Un crée le chunk overview
	if(mk_isTerrainChunk)
	{
		m_chunkOverview.resize(m_sizeX);
		for(int i(0); i < m_sizeX; ++i)
		{
			m_chunkOverview[i].resize(m_sizeZ);
			for(int j(0); j < m_sizeZ; ++j)
				m_chunkOverview[i][j].resize(3);
		}
		m_cubePosition.x = m_sizeX * m_chunkPosition.x;
		m_cubePosition.y = m_sizeY * m_chunkPosition.y;
		m_cubePosition.z = m_sizeZ * m_chunkPosition.z;
	}


	updateOutline();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void createCube(int i_x, int i_y, int i_z, int i_resist)
///
/// Fonction permettant de créer un cube à l'emplacement désigné
///
/// @param[in] i_x : Numéro de la cube en x dans la chunk
/// @param[in] i_y : Numéro de la cube en y dans la chunk (vertical)	
/// @param[in] i_z : Numéro de la cube en n_z dans la chunk
/// @param[in] i_resist : Résistance du cube construit
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::createCube(int i_x, int i_y, int i_z, int i_type, float i_hitpoint, float i_maxHitpoint, float ia_density[8])
{	
	//Les paramètres devrait être dans cet intervale
	assert (i_x >= 0 && i_x < m_sizeX && i_y >= 0 && i_y < m_sizeY && i_z >= 0 && i_z < m_sizeZ);

	//On ajoute le cube. Il doit être updaté pour être affiché
	if(m_map[i_x][i_y][i_z][CUBE_TYPE] == 0)
	{
		m_map[i_x][i_y][i_z][CUBE_TYPE] = i_type;
		setFloatToMap(i_x, i_y, i_z, CUBE_HITPOINT, i_hitpoint);
		setFloatToMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT, i_maxHitpoint);
		m_map[i_x][i_y][i_z][CUBE_COLOR] = generateCubeColor(i_x, i_y, i_z);

		for(int i(0); i < 8; ++i)
			setFloatToMap(i_x, i_y, i_z, CUBE_HIGH_0F + i, ia_density[i]);

		if(mk_isTerrainChunk)
			updateChunkOverview(i_x, i_z);
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void removeCube(int i_x, int i_y, int i_z)
///
/// Fonction permettant de retirer un cube à l'emplacement désigné.
///
/// @param[in] i_x : Numéro de la cube en x dans la chunk
/// @param[in] i_y : Numéro de la cube en y dans la chunk (vertical)	
/// @param[in] i_z : Numéro de la cube en z dans la chunk
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::removeCube(int i_x, int i_y, int i_z)
{
	if(m_map[i_x][i_y][i_z][CUBE_TYPE] != 0)
	{
		for(int i(0); i < NB_CUBE_PARAM; ++i)
			m_map[i_x][i_y][i_z][i] = 0;

		if(mk_isTerrainChunk)
			updateChunkOverview(i_x, i_z);
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void removeCube(int i_x, int i_y, int i_z)
///
/// Fonction permettant de retirer un cube à l'emplacement désigné.
///
/// @param[in] i_x : Numéro de la cube en x dans la chunk
/// @param[in] i_y : Numéro de la cube en y dans la chunk (vertical)	
/// @param[in] i_z : Numéro de la cube en z dans la chunk
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
float Chunk::damageCube(int i_x, int i_y, int i_z, float i_hit)
{
	float maxHitpoint(getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT));
	float hitpoint(getFloatFromMap(i_x, i_y, i_z, CUBE_HITPOINT));

	if(maxHitpoint == 0.f)
		return -i_hit;

	hitpoint -= i_hit;
	if(hitpoint <= 0 && m_map[i_x][i_y][i_z][CUBE_RENFORCEMENT] > 0)
	{
		--m_map[i_x][i_y][i_z][CUBE_RENFORCEMENT];
		hitpoint = maxHitpoint - hitpoint;
	}

	setFloatToMap(i_x, i_y, i_z, CUBE_HITPOINT, hitpoint);
	if(maxHitpoint > 0.f)
		updateDamage(i_x, i_y, i_z);

	return hitpoint;
}

float Chunk::repairCube(int i_x, int i_y, int i_z, float i_hit)
{
	if(getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT) == 0.f)
		return i_hit;

	setFloatToMap(i_x, i_y, i_z, CUBE_HITPOINT, getFloatFromMap(i_x, i_y, i_z, CUBE_HITPOINT) + i_hit);

	int result(0);
	if(getFloatFromMap(i_x, i_y, i_z, CUBE_HITPOINT) > getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT))
	{
		result = getFloatFromMap(i_x, i_y, i_z, CUBE_HITPOINT) - getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT);
		setFloatToMap(i_x, i_y, i_z, CUBE_HITPOINT, getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT));
	}
	updateDamage(i_x, i_y, i_z);

	return result;
}

bool Chunk::upgradeCube(int i_x, int i_y, int i_z)
{
	if(m_map[i_x][i_y][i_z][CUBE_RENFORCEMENT] >= 1 || 
		getFloatFromMap(i_x, i_y, i_z, CUBE_HITPOINT) != getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT) ||
		m_map[i_x][i_y][i_z][CUBE_TYPE] >= 0)
		return false;

	++m_map[i_x][i_y][i_z][CUBE_RENFORCEMENT];
	setFloatToMap(i_x, i_y, i_z, CUBE_HITPOINT, 1.f);
	updateDamage(i_x, i_y, i_z);
	return true;
}

void Chunk::preTreatementDraw()const
{
	//On choisi le shader du terrain
	Shader::chooseShader(GEOM_PASS_SHADER);
	Shader* p_currentShader(Shader::getShader());

	if(!mk_isTerrainChunk)
	{
		//On active les texture des cubes
		Texture::chooseTexture(m_texturesIndex[0]);
		Texture::linkTexture();
		Texture::bindTexture();

		//On envoie le nombre de texture total
		glUniform1i(p_currentShader->getUniformVariableSafe("InExecutionType"), TERRAIN);
		glUniform1i(p_currentShader->getUniformVariable("NbTextureTot"), Texture::getImageCount());

		glUniform1f(p_currentShader->getUniformVariableSafe("InSpecularPower"), 0.0f);
		glUniform1f(p_currentShader->getUniformVariableSafe("InMatSpecularIntensity"), 0.0f);
	}

	//On change la cullface de côté
	OpenGLHandler::cullFace(GL_BACK);
	
	Matrix identity;
	identity.loadIdentity();
	glUniformMatrix4fv(p_currentShader->getUniformVariableSafe("InWorld"), 1, GL_TRUE, identity.getValues());
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void addVertex(std::vector<VertexPosition>& i_vertexList, VertexPosition ia_vertex[4])
///
/// Fonction permettant d'ajouter un quad à la liste des sommets
/// 0_1
/// |\|
/// 2_3
/// 
/// @param[in] i_vertexList : liste des sommets
/// @param[in] ia_vertex : sommets
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void addVertex(std::vector<VertexPosition>& i_vertexList, VertexPosition ia_vertex[4])
{
	Vector vect1(ia_vertex[3].x - ia_vertex[0].x, ia_vertex[3].y - ia_vertex[0].y, ia_vertex[3].z - ia_vertex[0].z);
	Vector vect2(ia_vertex[1].x - ia_vertex[0].x, ia_vertex[1].y - ia_vertex[0].y, ia_vertex[1].z - ia_vertex[0].z);

	Vector normal(vect1.crossProduct(vect2).normalize());
	for(int i(0); i < 4; ++i)
	{
		ia_vertex[i].normal[0] = normal.x;
		ia_vertex[i].normal[1] = normal.y;
		ia_vertex[i].normal[2] = normal.z;
	}
	i_vertexList.push_back(ia_vertex[0]);
	i_vertexList.push_back(ia_vertex[3]);
	i_vertexList.push_back(ia_vertex[1]);

	i_vertexList.push_back(ia_vertex[0]);
	i_vertexList.push_back(ia_vertex[2]);
	i_vertexList.push_back(ia_vertex[3]);
}
////////////////////////////////////////////////////////////////////////
///
/// @fn void addUVHorizontal(float i_xCenter, float i_zCenter, std::vector<VertexPosition>& i_vertexList, VertexPosition ia_vertex[4])
///
/// Fonction permettant d'ajouter les coordonnées de texture d'un quad à la liste des UV pour les faces horizontales
/// 0_1
/// |\|
/// 2_3
/// 
/// @param[in] i_xCenter : centre du quad en x
/// @param[in] i_zCenter : centre du quad en y
/// @param[in] i_UVList : liste des sommets
/// @param[in] ia_vertex : sommets
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void addUVHorizontal(float i_xCenter, float i_zCenter, std::vector<VertexPosition>& i_vertexList, VertexPosition ia_vertex[4], int& i_index)
{
	i_vertexList[i_index].u = (ia_vertex[0].x - (i_xCenter - HALFSIDE))/SIDE;
	i_vertexList[i_index].v = (ia_vertex[0].z - (i_zCenter - HALFSIDE))/SIDE;
	++i_index;

	i_vertexList[i_index].u = (ia_vertex[3].x - (i_xCenter - HALFSIDE))/SIDE;
	i_vertexList[i_index].v = (ia_vertex[3].z - (i_zCenter - HALFSIDE))/SIDE;
	++i_index;

	i_vertexList[i_index].u = (ia_vertex[1].x - (i_xCenter - HALFSIDE))/SIDE;
	i_vertexList[i_index].v = (ia_vertex[1].z - (i_zCenter - HALFSIDE))/SIDE;
	++i_index;

	i_vertexList[i_index].u = (ia_vertex[0].x - (i_xCenter - HALFSIDE))/SIDE;
	i_vertexList[i_index].v = (ia_vertex[0].z - (i_zCenter - HALFSIDE))/SIDE;
	++i_index;

	i_vertexList[i_index].u = (ia_vertex[2].x - (i_xCenter - HALFSIDE))/SIDE;
	i_vertexList[i_index].v = (ia_vertex[2].z - (i_zCenter - HALFSIDE))/SIDE;
	++i_index;

	i_vertexList[i_index].u = (ia_vertex[3].x - (i_xCenter - HALFSIDE))/SIDE;
	i_vertexList[i_index].v = (ia_vertex[3].z - (i_zCenter - HALFSIDE))/SIDE;
	++i_index;
	
}
////////////////////////////////////////////////////////////////////////
///
/// @fn void addUVVertical(float i_xCenter, float i_yCenter, float i_zCenter, std::vector<VertexUV>& i_UVList, VertexPosition ia_vertex[4])
///
/// Fonction permettant d'ajouter les coordonnées de texture d'un quad à la liste des UV pour les faces verticales
/// 0_1
/// |\|
/// 2_3
/// 
/// @param[in] i_xCenter : centre du quad en x
/// @param[in] i_zCenter : centre du quad en y
/// @param[in] i_UVList : liste des sommets
/// @param[in] ia_vertex : sommets
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void addUVVertical(float i_xCenter, float i_yCenter, float i_zCenter, std::vector<VertexPosition>& i_vertexList, VertexPosition ia_vertex[4], int& i_index)
{
	int uCenter;
	bool useX(ia_vertex[0].z - ia_vertex[1].z == 0.0f);

	if(useX)
		i_vertexList[i_index].u = (ia_vertex[0].x - (i_xCenter - HALFSIDE))/SIDE;
	else
		i_vertexList[i_index].u = (ia_vertex[0].z - (i_zCenter - HALFSIDE))/SIDE;

	i_vertexList[i_index].v = (ia_vertex[0].y - (i_yCenter - HALFSIDE))/SIDE;

	++i_index;

	if(useX)
		i_vertexList[i_index].u = (ia_vertex[3].x - (i_xCenter - HALFSIDE))/SIDE;
	else
		i_vertexList[i_index].u = (ia_vertex[3].z - (i_zCenter - HALFSIDE))/SIDE;

	i_vertexList[i_index].v = (ia_vertex[3].y - (i_yCenter - HALFSIDE))/SIDE;
	++i_index;

	if(useX)
		i_vertexList[i_index].u = (ia_vertex[1].x - (i_xCenter - HALFSIDE))/SIDE;
	else
		i_vertexList[i_index].u = (ia_vertex[1].z - (i_zCenter - HALFSIDE))/SIDE;

	i_vertexList[i_index].v = (ia_vertex[1].y - (i_yCenter - HALFSIDE))/SIDE;
	++i_index;

	if(useX)
		i_vertexList[i_index].u = (ia_vertex[0].x - (i_xCenter - HALFSIDE))/SIDE;
	else
		i_vertexList[i_index].u = (ia_vertex[0].z - (i_zCenter - HALFSIDE))/SIDE;

	i_vertexList[i_index].v = (ia_vertex[0].y - (i_yCenter - HALFSIDE))/SIDE;
	++i_index;

	if(useX)
		i_vertexList[i_index].u = (ia_vertex[2].x - (i_xCenter - HALFSIDE))/SIDE;
	else
		i_vertexList[i_index].u = (ia_vertex[2].z - (i_zCenter - HALFSIDE))/SIDE;

	i_vertexList[i_index].v = (ia_vertex[2].y - (i_yCenter - HALFSIDE))/SIDE;
	++i_index;

	if(useX)
		i_vertexList[i_index].u = (ia_vertex[3].x - (i_xCenter - HALFSIDE))/SIDE;
	else
		i_vertexList[i_index].u = (ia_vertex[3].z - (i_zCenter - HALFSIDE))/SIDE;

	i_vertexList[i_index].v = (ia_vertex[3].y - (i_yCenter - HALFSIDE))/SIDE;
	++i_index;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void generateUsedVertex(int i_x, int i_y, int i_z, bool ia_usedVertex[16])const
///
/// Fonction permettant de générer la forme des cubes artificiels
/// 
/// @param[in] i_x : position du cube en x
/// @param[in] i_y : position du cube en y
/// @param[in] i_z : position du cube en z
/// @param[in] ia_usedVertex : tableau des sommets utilisées
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::generateUsedVertex(int i_x, int i_y, int i_z, bool ia_usedVertex[16])const
{
	bool isAirLeft = isAirIntern(i_x + 1, i_y, i_z);
	bool isAirRight = isAirIntern(i_x - 1, i_y, i_z);
	bool isAirFront = isAirIntern(i_x, i_y, i_z - 1);
	bool isAirBack = isAirIntern(i_x, i_y, i_z + 1);
	bool isAirTop = isAirIntern(i_x, i_y + 1, i_z);
	bool isAirBottom = isAirIntern(i_x, i_y - 1, i_z);

	bool isAirBackLeft = isAirIntern(i_x + 1, i_y, i_z + 1);
	bool isAirBackRight = isAirIntern(i_x - 1, i_y, i_z + 1);
	bool isAirFrontLeft = isAirIntern(i_x + 1, i_y, i_z - 1);
	bool isAirFrontRight = isAirIntern(i_x - 1, i_y, i_z - 1);


	//On ajoute les faces
	//top

	// 0  1  2  3
	// 4  5  6  7
	// 8  9  10 11
	// 12 13 14 15
		
	for(int i(0); i < 16 ; ++i)
		ia_usedVertex[i] = false;

	//On trouve quel sont les sommets nécessaires
	if(isAirLeft)
	{
		ia_usedVertex[5] = true;
		ia_usedVertex[9] = true;
	}
	else
	{
		ia_usedVertex[4] = true;
		ia_usedVertex[8] = true;
	}

	if(isAirRight)
	{
		ia_usedVertex[6] = true;
		ia_usedVertex[10] = true;
	}
	else
	{
		ia_usedVertex[7] = true;
		ia_usedVertex[11] = true;
	}

	if(isAirFront)
	{
		ia_usedVertex[9] = true;
		ia_usedVertex[10] = true;
	}
	else
	{
		ia_usedVertex[13] = true;
		ia_usedVertex[14] = true;
	}

	if(isAirBack)
	{
		ia_usedVertex[5] = true;
		ia_usedVertex[6] = true;
	}
	else
	{
		ia_usedVertex[1] = true;
		ia_usedVertex[2] = true;
	}

	if(!isAirBackLeft && !isAirBack && !isAirLeft)
	{
		ia_usedVertex[0] = true;
		ia_usedVertex[5] = false;
	}
	else
	{
		ia_usedVertex[5] = true;
	}

	if(!isAirBackRight && !isAirBack && !isAirRight)
	{
		ia_usedVertex[3] = true;
		ia_usedVertex[6] = false;
			
	}
	else
	{
		ia_usedVertex[6] = true;
	}

	if(!isAirFrontLeft && !isAirFront && !isAirLeft)
	{
		ia_usedVertex[12] = true;
		ia_usedVertex[9] = false;
			
	}
	else
	{
		ia_usedVertex[9] = true;
	}

	if(!isAirFrontRight && !isAirFront && !isAirRight)
	{
		ia_usedVertex[15] = true;
		ia_usedVertex[10] = false;
			
	}
	else
	{
		ia_usedVertex[10] = true;
	}

	//Simplification des points intérieur
	if(!ia_usedVertex[4] && ia_usedVertex[1] && ia_usedVertex[5])
		ia_usedVertex[5] = false;
	if(!ia_usedVertex[1] && ia_usedVertex[4] && ia_usedVertex[5])
		ia_usedVertex[5] = false;

	if(!ia_usedVertex[2] && ia_usedVertex[7] && ia_usedVertex[6])
		ia_usedVertex[6] = false;
	if(!ia_usedVertex[7] && ia_usedVertex[2] && ia_usedVertex[6])
		ia_usedVertex[6] = false;

	if(!ia_usedVertex[11] && ia_usedVertex[14] && ia_usedVertex[10])
		ia_usedVertex[10] = false;
	if(!ia_usedVertex[14] && ia_usedVertex[11] && ia_usedVertex[10])
		ia_usedVertex[10] = false;

	if(!ia_usedVertex[8] && ia_usedVertex[13] && ia_usedVertex[9])
		ia_usedVertex[9] = false;
	if(!ia_usedVertex[13] && ia_usedVertex[8] && ia_usedVertex[9])
		ia_usedVertex[9] = false;

	//Simplification des points extérieur
	if(ia_usedVertex[3] && ia_usedVertex[7])
		ia_usedVertex[7] = false;

	if(ia_usedVertex[15] && ia_usedVertex[11])
		ia_usedVertex[11] = false;

	if(ia_usedVertex[15] && ia_usedVertex[14])
		ia_usedVertex[14] = false;

	if(ia_usedVertex[12] && ia_usedVertex[13])
		ia_usedVertex[13] = false;

	if(ia_usedVertex[12] && ia_usedVertex[8])
		ia_usedVertex[8] = false;

	if(ia_usedVertex[0] && ia_usedVertex[4])
		ia_usedVertex[4] = false;

	if(ia_usedVertex[0] && ia_usedVertex[1])
		ia_usedVertex[1] = false;

	if(ia_usedVertex[3] && ia_usedVertex[2])
		ia_usedVertex[2] = false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void Chunk::createArtificialCubeMesh(int i_x, int i_y, int i_z, 
///			std::vector<TVertex_VC>& i_vertexList, std::vector<GLuint>& i_indexList, std::vector<VertexUV>& i_UVList, std::vector<GLfloat>& i_textureNbList)
///
/// Fonction permetant d'ajouter le cube au tableau à envoyer à la carte graphique
/// 
/// @param[in] i_x : Coordonée en x du cube (valeur relative)
/// @param[in] i_y : Coordonée en z du cube (valeur relative)
/// @param[in] i_z : Coordonée en x du cube (valeur relative)
/// @param[in] i_vertexList : Valeurs de retour des sommets
/// @param[in] i_indexList : Valeurs de retour des index
/// @param[in] i_UVList : Valeurs de retour des coordonnées de sommets
/// @param[in] i_textureNbList : Valeurs de retour des numéros de texture utilisés
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::createArtificialCubeMesh(int i_x, int i_y, int i_z, std::vector<GLint>& i_textureNbList, std::vector<GLint>& i_renforcementList, std::vector<GLfloat>& i_damageList)
{
	int indexStart(m_vertexList.size());
	int index(indexStart);

	Terrain* p_terrain(Terrain::getInstance());

	int vertexOldSize(m_vertexList.size());

	float xCenter(i_x * SIDE);
	float yCenter(i_y * SIDE);
	float zCenter(i_z * SIDE);

	bool isTopNeedRendering(isAirIntern(i_x, i_y + 1, i_z) || 
	(isArtificialIntern(i_x, i_y + 1, i_z) &&(	
	isArtificialIntern(i_x + 1, i_y, i_z) != isArtificialIntern(i_x + 1, i_y + 1, i_z) &&
	isArtificialIntern(i_x - 1, i_y, i_z) != isArtificialIntern(i_x - 1, i_y + 1, i_z) &&
	isArtificialIntern(i_x, i_y, i_z - 1) != isArtificialIntern(i_x, i_y + 1, i_z - 1) &&
	isArtificialIntern(i_x, i_y, i_z + 1) != isArtificialIntern(i_x, i_y + 1, i_z + 1)))
	|| isNaturalIntern(i_x, i_y + 1, i_z) && !isBottomFilledIntern(i_x, i_y + 1, i_z));

	bool isAirBottom = isAirIntern(i_x, i_y - 1, i_z);

	bool a_usedVertex[16];
	generateUsedVertex(i_x, i_y, i_z, a_usedVertex);

	bool a_usedVertexSide[16];
	for(int i(0); i < 16; ++i)
		a_usedVertexSide[i] = a_usedVertex[i];

	//Correction de la simplification pour le dessus et le dessous
	if(a_usedVertex[5] && a_usedVertex[10] && !a_usedVertex[6] && !a_usedVertex[9])
		a_usedVertex[6] = true;
	if(!a_usedVertex[5] && !a_usedVertex[10] && a_usedVertex[6] && a_usedVertex[9])
		a_usedVertex[5] = true;

	if((!a_usedVertex[5] && a_usedVertex[6] && !a_usedVertex[9] && !a_usedVertex[10] && a_usedVertex[1] && a_usedVertex[2]) || 
		(!a_usedVertex[5] && !a_usedVertex[6] && a_usedVertex[9] && !a_usedVertex[10] && a_usedVertex[4] && a_usedVertex[8]))
		a_usedVertex[5] = true;
	if((!a_usedVertex[5] && !a_usedVertex[6] && !a_usedVertex[9] && a_usedVertex[10] && a_usedVertex[7] && a_usedVertex[11]) || 
		(a_usedVertex[5] && !a_usedVertex[6] && !a_usedVertex[9] && !a_usedVertex[10] && a_usedVertex[1] && a_usedVertex[2]))
		a_usedVertex[6] = true;
	if((!a_usedVertex[5] && !a_usedVertex[6] && !a_usedVertex[9] && a_usedVertex[10] && a_usedVertex[13] && a_usedVertex[14]) ||
		(a_usedVertex[5] && !a_usedVertex[6] && !a_usedVertex[9] && !a_usedVertex[10] && a_usedVertex[4] && a_usedVertex[8]))
		a_usedVertex[9] = true;
	if((!a_usedVertex[5] && a_usedVertex[6] && !a_usedVertex[9] && !a_usedVertex[10] && a_usedVertex[7] && a_usedVertex[11]) || 
		(!a_usedVertex[5] && !a_usedVertex[6] && a_usedVertex[9] && !a_usedVertex[10] && a_usedVertex[13] && a_usedVertex[14]))
		a_usedVertex[10] = true;

	if(!a_usedVertex[0] && a_usedVertex[3] && a_usedVertex[12] && a_usedVertex[15] && a_usedVertex[5])
		a_usedVertex[7] = true;
	if(a_usedVertex[0] && !a_usedVertex[3] && a_usedVertex[12] && a_usedVertex[15] && a_usedVertex[6])
		a_usedVertex[4] = true;
	if(a_usedVertex[0] && a_usedVertex[3] && !a_usedVertex[12] && a_usedVertex[15] && a_usedVertex[9])
		a_usedVertex[1] = true;
	if(a_usedVertex[0] && a_usedVertex[3] && a_usedVertex[12] && !a_usedVertex[15] && a_usedVertex[10])
		a_usedVertex[8] = true;

	//On trouve les coordonnées absolues
	int realX = i_x + m_cubePosition.x;
	int realY = i_y + m_cubePosition.y;
	int realZ = i_z + m_cubePosition.z;

	int verticalStrech(0);

	while(p_terrain->isNatural(realX, realY - verticalStrech - 1, realZ) && 
	   (p_terrain->getIsoPosition(realX, realY - verticalStrech - 1, realZ, CUBE_HIGH_0F) <= 1.f ||
		p_terrain->getIsoPosition(realX, realY - verticalStrech - 1, realZ, CUBE_HIGH_1F) <= 1.f ||
		p_terrain->getIsoPosition(realX, realY - verticalStrech - 1, realZ, CUBE_HIGH_2F) <= 1.f ||
		p_terrain->getIsoPosition(realX, realY - verticalStrech - 1, realZ, CUBE_HIGH_3F) <= 1.f ))
	{
		++verticalStrech;
	}

	// 0  1  2  3
	// 4  5  6  7
	// 8  9  10 11
	// 12 13 14 15

	VertexPosition potentialVertices[32] = 
	{
		VertexPosition(i_x * SIDE + HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE + HALFSIDE),//0
		VertexPosition(i_x * SIDE + SIDE*1/6, i_y * SIDE + HALFSIDE, i_z * SIDE + HALFSIDE),//1
		VertexPosition(i_x * SIDE - SIDE*1/6, i_y * SIDE + HALFSIDE, i_z * SIDE + HALFSIDE),//2
		VertexPosition(i_x * SIDE - HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE + HALFSIDE),//3
		VertexPosition(i_x * SIDE + HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE + SIDE*1/6),//4
		VertexPosition(i_x * SIDE + SIDE*1/6, i_y * SIDE + HALFSIDE, i_z * SIDE + SIDE*1/6),//5
		VertexPosition(i_x * SIDE - SIDE*1/6, i_y * SIDE + HALFSIDE, i_z * SIDE + SIDE*1/6),//6
		VertexPosition(i_x * SIDE - HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE + SIDE*1/6),//7
		VertexPosition(i_x * SIDE + HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE - SIDE*1/6),//8
		VertexPosition(i_x * SIDE + SIDE*1/6, i_y * SIDE + HALFSIDE, i_z * SIDE - SIDE*1/6),//9
		VertexPosition(i_x * SIDE - SIDE*1/6, i_y * SIDE + HALFSIDE, i_z * SIDE - SIDE*1/6),//10
		VertexPosition(i_x * SIDE - HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE - SIDE*1/6),//11
		VertexPosition(i_x * SIDE + HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE - HALFSIDE),//12
		VertexPosition(i_x * SIDE + SIDE*1/6, i_y * SIDE + HALFSIDE, i_z * SIDE - HALFSIDE),//13
		VertexPosition(i_x * SIDE - SIDE*1/6, i_y * SIDE + HALFSIDE, i_z * SIDE - HALFSIDE),//14
		VertexPosition(i_x * SIDE - HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE - HALFSIDE),//15

		VertexPosition(i_x * SIDE + HALFSIDE, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE + HALFSIDE),//16
		VertexPosition(i_x * SIDE + SIDE*1/6, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE + HALFSIDE),//17
		VertexPosition(i_x * SIDE - SIDE*1/6, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE + HALFSIDE),//18
		VertexPosition(i_x * SIDE - HALFSIDE, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE + HALFSIDE),//19
		VertexPosition(i_x * SIDE + HALFSIDE, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE + SIDE*1/6),//20
		VertexPosition(i_x * SIDE + SIDE*1/6, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE + SIDE*1/6),//21
		VertexPosition(i_x * SIDE - SIDE*1/6, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE + SIDE*1/6),//22
		VertexPosition(i_x * SIDE - HALFSIDE, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE + SIDE*1/6),//23
		VertexPosition(i_x * SIDE + HALFSIDE, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE - SIDE*1/6),//24
		VertexPosition(i_x * SIDE + SIDE*1/6, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE - SIDE*1/6),//25
		VertexPosition(i_x * SIDE - SIDE*1/6, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE - SIDE*1/6),//26
		VertexPosition(i_x * SIDE - HALFSIDE, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE - SIDE*1/6),//27
		VertexPosition(i_x * SIDE + HALFSIDE, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE - HALFSIDE),//28
		VertexPosition(i_x * SIDE + SIDE*1/6, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE - HALFSIDE),//29
		VertexPosition(i_x * SIDE - SIDE*1/6, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE - HALFSIDE),//30
		VertexPosition(i_x * SIDE - HALFSIDE, (i_y - verticalStrech) * SIDE - HALFSIDE, i_z * SIDE - HALFSIDE) //31
	};

	VertexPosition a_vertexPlane[4];
	//top
	//0
	if(isTopNeedRendering)
	{
		if(a_usedVertex[0])
		{
			if(a_usedVertex[1] && a_usedVertex[8] && a_usedVertex[9])
			{
				a_vertexPlane[0] = potentialVertices[0];
				a_vertexPlane[1] = potentialVertices[1];
				a_vertexPlane[2] = potentialVertices[8];
				a_vertexPlane[3] = potentialVertices[9];
				addVertex(m_vertexList, a_vertexPlane);
				addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
			else if(a_usedVertex[2] && ((a_usedVertex[8] && a_usedVertex[10]) || (a_usedVertex[12] && a_usedVertex[14]) || (a_usedVertex[4] && a_usedVertex[6])))
			{
				if(a_usedVertex[4] && a_usedVertex[6])
				{
					a_vertexPlane[0] = potentialVertices[0];
					a_vertexPlane[1] = potentialVertices[2];
					a_vertexPlane[2] = potentialVertices[4];
					a_vertexPlane[3] = potentialVertices[6];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				if(a_usedVertex[8] && a_usedVertex[10])
				{
					a_vertexPlane[0] = potentialVertices[0];
					a_vertexPlane[1] = potentialVertices[2];
					a_vertexPlane[2] = potentialVertices[8];
					a_vertexPlane[3] = potentialVertices[10];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[12] && a_usedVertex[14])
				{
					a_vertexPlane[0] = potentialVertices[0];
					a_vertexPlane[1] = potentialVertices[2];
					a_vertexPlane[2] = potentialVertices[12];
					a_vertexPlane[3] = potentialVertices[14];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
			else if(a_usedVertex[3])
			{
				if(a_usedVertex[8] && a_usedVertex[11])
				{
					a_vertexPlane[0] = potentialVertices[0];
					a_vertexPlane[1] = potentialVertices[3];
					a_vertexPlane[2] = potentialVertices[8];
					a_vertexPlane[3] = potentialVertices[11];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[12] && a_usedVertex[15])
				{
					a_vertexPlane[0] = potentialVertices[0];
					a_vertexPlane[1] = potentialVertices[3];
					a_vertexPlane[2] = potentialVertices[12];
					a_vertexPlane[3] = potentialVertices[15];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
		}
		//1
		if(a_usedVertex[1])
		{
			if(a_usedVertex[2])
			{
				if(a_usedVertex[5] && a_usedVertex[6])
				{
					a_vertexPlane[0] = potentialVertices[1];
					a_vertexPlane[1] = potentialVertices[2];
					a_vertexPlane[2] = potentialVertices[5];
					a_vertexPlane[3] = potentialVertices[6];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[9] && a_usedVertex[10])
				{
					a_vertexPlane[0] = potentialVertices[1];
					a_vertexPlane[1] = potentialVertices[2];
					a_vertexPlane[2] = potentialVertices[9];
					a_vertexPlane[3] = potentialVertices[10];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[13] && a_usedVertex[14])
				{
					a_vertexPlane[0] = potentialVertices[1];
					a_vertexPlane[1] = potentialVertices[2];
					a_vertexPlane[2] = potentialVertices[13];
					a_vertexPlane[3] = potentialVertices[14];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
			else if(a_usedVertex[3])
			{
				if(a_usedVertex[5] && a_usedVertex[7])
				{
					a_vertexPlane[0] = potentialVertices[1];
					a_vertexPlane[1] = potentialVertices[3];
					a_vertexPlane[2] = potentialVertices[5];
					a_vertexPlane[3] = potentialVertices[7];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[9] && a_usedVertex[11])
				{
					a_vertexPlane[0] = potentialVertices[1];
					a_vertexPlane[1] = potentialVertices[3];
					a_vertexPlane[2] = potentialVertices[9];
					a_vertexPlane[3] = potentialVertices[11];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[13] && a_usedVertex[15])
				{
					a_vertexPlane[0] = potentialVertices[1];
					a_vertexPlane[1] = potentialVertices[3];
					a_vertexPlane[2] = potentialVertices[13];
					a_vertexPlane[3] = potentialVertices[15];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
		}
		//4
		if(a_usedVertex[4])
		{
			if(a_usedVertex[5] && a_usedVertex[8] && a_usedVertex[9])
			{
				a_vertexPlane[0] = potentialVertices[4];
				a_vertexPlane[1] = potentialVertices[5];
				a_vertexPlane[2] = potentialVertices[8];
				a_vertexPlane[3] = potentialVertices[9];
				addVertex(m_vertexList, a_vertexPlane);
				addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
			else if(a_usedVertex[6] && ((a_usedVertex[8] && a_usedVertex[10])||(a_usedVertex[12] && a_usedVertex[14])))
			{
				if(a_usedVertex[8] && a_usedVertex[10])
				{
					a_vertexPlane[0] = potentialVertices[4];
					a_vertexPlane[1] = potentialVertices[6];
					a_vertexPlane[2] = potentialVertices[8];
					a_vertexPlane[3] = potentialVertices[10];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[12] && a_usedVertex[14])
				{
					a_vertexPlane[0] = potentialVertices[4];
					a_vertexPlane[1] = potentialVertices[6];
					a_vertexPlane[2] = potentialVertices[12];
					a_vertexPlane[3] = potentialVertices[14];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
			else if(a_usedVertex[7])
			{
				if(a_usedVertex[8] && a_usedVertex[11])
				{
					a_vertexPlane[0] = potentialVertices[4];
					a_vertexPlane[1] = potentialVertices[7];
					a_vertexPlane[2] = potentialVertices[8];
					a_vertexPlane[3] = potentialVertices[11];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[12] && a_usedVertex[15])
				{
					a_vertexPlane[0] = potentialVertices[4];
					a_vertexPlane[1] = potentialVertices[7];
					a_vertexPlane[2] = potentialVertices[12];
					a_vertexPlane[3] = potentialVertices[15];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
		}
		//5
		if(a_usedVertex[5])
		{
			if(a_usedVertex[6] &&((a_usedVertex[9] && a_usedVertex[10]) || (a_usedVertex[13] && a_usedVertex[14])))
			{
				if(a_usedVertex[9] && a_usedVertex[10])
				{
					a_vertexPlane[0] = potentialVertices[5];
					a_vertexPlane[1] = potentialVertices[6];
					a_vertexPlane[2] = potentialVertices[9];
					a_vertexPlane[3] = potentialVertices[10];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[13] && a_usedVertex[14])
				{
					a_vertexPlane[0] = potentialVertices[5];
					a_vertexPlane[1] = potentialVertices[6];
					a_vertexPlane[2] = potentialVertices[13];
					a_vertexPlane[3] = potentialVertices[14];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
			else if(a_usedVertex[7])
			{
				if(a_usedVertex[9] && a_usedVertex[11])
				{
					a_vertexPlane[0] = potentialVertices[5];
					a_vertexPlane[1] = potentialVertices[7];
					a_vertexPlane[2] = potentialVertices[9];
					a_vertexPlane[3] = potentialVertices[11];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[13] && a_usedVertex[15])
				{
					a_vertexPlane[0] = potentialVertices[5];
					a_vertexPlane[1] = potentialVertices[7];
					a_vertexPlane[2] = potentialVertices[13];
					a_vertexPlane[3] = potentialVertices[15];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
		}
		//6
		if(a_usedVertex[6])
		{
			if(a_usedVertex[6] && a_usedVertex[7] && a_usedVertex[10] && a_usedVertex[11])
			{
				a_vertexPlane[0] = potentialVertices[6];
				a_vertexPlane[1] = potentialVertices[7];
				a_vertexPlane[2] = potentialVertices[10];
				a_vertexPlane[3] = potentialVertices[11];
				addVertex(m_vertexList, a_vertexPlane);
				addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
		}
		//8
		if(a_usedVertex[8] && a_usedVertex[10] && a_usedVertex[12] && a_usedVertex[14])
		{
			a_vertexPlane[0] = potentialVertices[8];
			a_vertexPlane[1] = potentialVertices[10];
			a_vertexPlane[2] = potentialVertices[12];
			a_vertexPlane[3] = potentialVertices[14];
			addVertex(m_vertexList, a_vertexPlane);
			addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		//9
		if(a_usedVertex[9] && a_usedVertex[10] && a_usedVertex[13] && a_usedVertex[14])
		{
			a_vertexPlane[0] = potentialVertices[9];
			a_vertexPlane[1] = potentialVertices[10];
			a_vertexPlane[2] = potentialVertices[13];
			a_vertexPlane[3] = potentialVertices[14];
			addVertex(m_vertexList, a_vertexPlane);
			addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
	}
	//bottom
	//0
	if(isAirBottom)
	{
		if(a_usedVertex[0])
		{
			if(a_usedVertex[1] && a_usedVertex[8] && a_usedVertex[9])
			{
				a_vertexPlane[0] = potentialVertices[16];
				a_vertexPlane[1] = potentialVertices[24];
				a_vertexPlane[2] = potentialVertices[17];
				a_vertexPlane[3] = potentialVertices[25];
				addVertex(m_vertexList, a_vertexPlane);
				addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
			else if(a_usedVertex[2] && ((a_usedVertex[8] && a_usedVertex[10]) || (a_usedVertex[12] && a_usedVertex[14]) || (a_usedVertex[4] && a_usedVertex[6])))
			{
				if(a_usedVertex[4] && a_usedVertex[6])
				{
					a_vertexPlane[0] = potentialVertices[16];
					a_vertexPlane[1] = potentialVertices[20];
					a_vertexPlane[2] = potentialVertices[18];
					a_vertexPlane[3] = potentialVertices[22];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				if(a_usedVertex[8] && a_usedVertex[10])
				{
					a_vertexPlane[0] = potentialVertices[16];
					a_vertexPlane[1] = potentialVertices[24];
					a_vertexPlane[2] = potentialVertices[18];
					a_vertexPlane[3] = potentialVertices[26];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[12] && a_usedVertex[14])
				{
					a_vertexPlane[0] = potentialVertices[16];
					a_vertexPlane[1] = potentialVertices[28];
					a_vertexPlane[2] = potentialVertices[18];
					a_vertexPlane[3] = potentialVertices[30];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
			else if(a_usedVertex[3])
			{
				if(a_usedVertex[8] && a_usedVertex[11])
				{
					a_vertexPlane[0] = potentialVertices[16];
					a_vertexPlane[1] = potentialVertices[24];
					a_vertexPlane[2] = potentialVertices[19];
					a_vertexPlane[3] = potentialVertices[27];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[12] && a_usedVertex[15])
				{
					a_vertexPlane[0] = potentialVertices[16];
					a_vertexPlane[1] = potentialVertices[28];
					a_vertexPlane[2] = potentialVertices[19];
					a_vertexPlane[3] = potentialVertices[31];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
		}
		//1
		if(a_usedVertex[1])
		{
			if(a_usedVertex[2])
			{
				if(a_usedVertex[5] && a_usedVertex[6])
				{
					a_vertexPlane[0] = potentialVertices[17];
					a_vertexPlane[1] = potentialVertices[21];
					a_vertexPlane[2] = potentialVertices[18];
					a_vertexPlane[3] = potentialVertices[22];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[9] && a_usedVertex[10])
				{
					a_vertexPlane[0] = potentialVertices[17];
					a_vertexPlane[1] = potentialVertices[25];
					a_vertexPlane[2] = potentialVertices[18];
					a_vertexPlane[3] = potentialVertices[26];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[13] && a_usedVertex[14])
				{
					a_vertexPlane[0] = potentialVertices[17];
					a_vertexPlane[1] = potentialVertices[29];
					a_vertexPlane[2] = potentialVertices[18];
					a_vertexPlane[3] = potentialVertices[30];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
			else if(a_usedVertex[3])
			{
				if(a_usedVertex[5] && a_usedVertex[7])
				{
					a_vertexPlane[0] = potentialVertices[17];
					a_vertexPlane[1] = potentialVertices[21];
					a_vertexPlane[2] = potentialVertices[19];
					a_vertexPlane[3] = potentialVertices[23];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[9] && a_usedVertex[11])
				{
					a_vertexPlane[0] = potentialVertices[17];
					a_vertexPlane[1] = potentialVertices[25];
					a_vertexPlane[2] = potentialVertices[19];
					a_vertexPlane[3] = potentialVertices[27];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[13] && a_usedVertex[15])
				{
					a_vertexPlane[0] = potentialVertices[17];
					a_vertexPlane[1] = potentialVertices[29];
					a_vertexPlane[2] = potentialVertices[19];
					a_vertexPlane[3] = potentialVertices[31];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
		}
		//4
		if(a_usedVertex[4])
		{
			if(a_usedVertex[5] && a_usedVertex[8] && a_usedVertex[9])
			{
				a_vertexPlane[0] = potentialVertices[20];
				a_vertexPlane[1] = potentialVertices[24];
				a_vertexPlane[2] = potentialVertices[21];
				a_vertexPlane[3] = potentialVertices[25];
				addVertex(m_vertexList, a_vertexPlane);
				addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
			else if(a_usedVertex[6] && ((a_usedVertex[8] && a_usedVertex[10])||(a_usedVertex[12] && a_usedVertex[14])))
			{
				if(a_usedVertex[8] && a_usedVertex[10])
				{
					a_vertexPlane[0] = potentialVertices[20];
					a_vertexPlane[1] = potentialVertices[24];
					a_vertexPlane[2] = potentialVertices[22];
					a_vertexPlane[3] = potentialVertices[26];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[12] && a_usedVertex[14])
				{
					a_vertexPlane[0] = potentialVertices[20];
					a_vertexPlane[1] = potentialVertices[28];
					a_vertexPlane[2] = potentialVertices[22];
					a_vertexPlane[3] = potentialVertices[30];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
			else if(a_usedVertex[7])
			{
				if(a_usedVertex[8] && a_usedVertex[11])
				{
					a_vertexPlane[0] = potentialVertices[20];
					a_vertexPlane[1] = potentialVertices[24];
					a_vertexPlane[2] = potentialVertices[23];
					a_vertexPlane[3] = potentialVertices[27];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[12] && a_usedVertex[15])
				{
					a_vertexPlane[0] = potentialVertices[20];
					a_vertexPlane[1] = potentialVertices[28];
					a_vertexPlane[2] = potentialVertices[23];
					a_vertexPlane[3] = potentialVertices[31];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
		}
		//5
		if(a_usedVertex[5])
		{
			if(a_usedVertex[6] &&((a_usedVertex[9] && a_usedVertex[10]) || (a_usedVertex[13] && a_usedVertex[14])))
			{
				if(a_usedVertex[9] && a_usedVertex[10])
				{
					a_vertexPlane[0] = potentialVertices[21];
					a_vertexPlane[1] = potentialVertices[25];
					a_vertexPlane[2] = potentialVertices[22];
					a_vertexPlane[3] = potentialVertices[26];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[13] && a_usedVertex[14])
				{
					a_vertexPlane[0] = potentialVertices[21];
					a_vertexPlane[1] = potentialVertices[29];
					a_vertexPlane[2] = potentialVertices[22];
					a_vertexPlane[3] = potentialVertices[30];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
			else if(a_usedVertex[7])
			{
				if(a_usedVertex[9] && a_usedVertex[11])
				{
					a_vertexPlane[0] = potentialVertices[21];
					a_vertexPlane[1] = potentialVertices[25];
					a_vertexPlane[2] = potentialVertices[23];
					a_vertexPlane[3] = potentialVertices[27];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
				else if(a_usedVertex[13] && a_usedVertex[15])
				{
					a_vertexPlane[0] = potentialVertices[21];
					a_vertexPlane[1] = potentialVertices[29];
					a_vertexPlane[2] = potentialVertices[23];
					a_vertexPlane[3] = potentialVertices[31];
					addVertex(m_vertexList, a_vertexPlane);
					addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
				}
			}
		}
		//6
		if(a_usedVertex[6])
		{
			if(a_usedVertex[6] && a_usedVertex[7] && a_usedVertex[10] && a_usedVertex[11])
			{
				a_vertexPlane[0] = potentialVertices[22];
				a_vertexPlane[1] = potentialVertices[26];
				a_vertexPlane[2] = potentialVertices[23];
				a_vertexPlane[3] = potentialVertices[27];
				addVertex(m_vertexList, a_vertexPlane);
				addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
		}
		//8
		if(a_usedVertex[8] && a_usedVertex[10] && a_usedVertex[12] && a_usedVertex[14])
		{
			a_vertexPlane[0] = potentialVertices[24];
			a_vertexPlane[1] = potentialVertices[28];
			a_vertexPlane[2] = potentialVertices[26];
			a_vertexPlane[3] = potentialVertices[30];
			addVertex(m_vertexList, a_vertexPlane);
			addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		//9
		if(a_usedVertex[9] && a_usedVertex[10] && a_usedVertex[13] && a_usedVertex[14])
		{
			a_vertexPlane[0] = potentialVertices[25];
			a_vertexPlane[1] = potentialVertices[29];
			a_vertexPlane[2] = potentialVertices[26];
			a_vertexPlane[3] = potentialVertices[30];
			addVertex(m_vertexList, a_vertexPlane);
			addUVHorizontal(xCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
	}
			
	//side
	index = indexStart;
	bool useLeftSide = !isArtificialIntern(i_x + 1, i_y, i_z);
	bool useRightSide = !isArtificialIntern(i_x - 1, i_y, i_z);
	bool useFrontSide = !isArtificialIntern(i_x, i_y, i_z - 1);
	bool useBackSide = !isArtificialIntern(i_x, i_y, i_z + 1);
	//0
	if(a_usedVertexSide[0])
	{
		if(useBackSide)
		{
			if(a_usedVertexSide[2])
			{
				a_vertexPlane[0] = potentialVertices[2];
				a_vertexPlane[1] = potentialVertices[0];
				a_vertexPlane[2] = potentialVertices[18];
				a_vertexPlane[3] = potentialVertices[16];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
			else if(a_usedVertexSide[3])
			{
				a_vertexPlane[0] = potentialVertices[3];
				a_vertexPlane[1] = potentialVertices[0];
				a_vertexPlane[2] = potentialVertices[19];
				a_vertexPlane[3] = potentialVertices[16];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
		}

		if(useLeftSide)
		{
			if(a_usedVertexSide[8])
			{
				a_vertexPlane[0] = potentialVertices[0];
				a_vertexPlane[1] = potentialVertices[8];
				a_vertexPlane[2] = potentialVertices[16];
				a_vertexPlane[3] = potentialVertices[24];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
			else if(a_usedVertexSide[12])
			{
				a_vertexPlane[0] = potentialVertices[0];
				a_vertexPlane[1] = potentialVertices[12];
				a_vertexPlane[2] = potentialVertices[16];
				a_vertexPlane[3] = potentialVertices[28];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
		}
	}
	//1
	if(a_usedVertexSide[1])
	{
		if(useBackSide)
		{
			if(a_usedVertexSide[2])
			{
				a_vertexPlane[0] = potentialVertices[2];
				a_vertexPlane[1] = potentialVertices[1];
				a_vertexPlane[2] = potentialVertices[18];
				a_vertexPlane[3] = potentialVertices[17];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
			else if(a_usedVertexSide[3])
			{
				a_vertexPlane[0] = potentialVertices[3];
				a_vertexPlane[1] = potentialVertices[1];
				a_vertexPlane[2] = potentialVertices[19];
				a_vertexPlane[3] = potentialVertices[17];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
		}
		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[5])
		{
			a_vertexPlane[0] = potentialVertices[1];
			a_vertexPlane[1] = potentialVertices[5];
			a_vertexPlane[2] = potentialVertices[17];
			a_vertexPlane[3] = potentialVertices[21];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[9])
		{
			a_vertexPlane[0] = potentialVertices[1];
			a_vertexPlane[1] = potentialVertices[9];
			a_vertexPlane[2] = potentialVertices[17];
			a_vertexPlane[3] = potentialVertices[25];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[13])
		{
			a_vertexPlane[0] = potentialVertices[1];
			a_vertexPlane[1] = potentialVertices[13];
			a_vertexPlane[2] = potentialVertices[17];
			a_vertexPlane[3] = potentialVertices[29];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
	}
	//2
	if(a_usedVertexSide[2])
	{
		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[6])
		{
			a_vertexPlane[0] = potentialVertices[6];
			a_vertexPlane[1] = potentialVertices[2];
			a_vertexPlane[2] = potentialVertices[22];
			a_vertexPlane[3] = potentialVertices[18];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[10])
		{
			a_vertexPlane[0] = potentialVertices[10];
			a_vertexPlane[1] = potentialVertices[2];
			a_vertexPlane[2] = potentialVertices[26];
			a_vertexPlane[3] = potentialVertices[18];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[14])
		{
			a_vertexPlane[0] = potentialVertices[14];
			a_vertexPlane[1] = potentialVertices[2];
			a_vertexPlane[2] = potentialVertices[30];
			a_vertexPlane[3] = potentialVertices[18];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
	}
	//3
	if(a_usedVertexSide[3])
	{
		if(useRightSide)
		{
			if(a_usedVertexSide[11])
			{
				a_vertexPlane[0] = potentialVertices[11];
				a_vertexPlane[1] = potentialVertices[3];
				a_vertexPlane[2] = potentialVertices[27];
				a_vertexPlane[3] = potentialVertices[19];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
			else if(a_usedVertexSide[15])
			{
				a_vertexPlane[0] = potentialVertices[15];
				a_vertexPlane[1] = potentialVertices[3];
				a_vertexPlane[2] = potentialVertices[31];
				a_vertexPlane[3] = potentialVertices[19];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
		}
	}
	//4
	if(a_usedVertexSide[4])
	{
		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[5])
		{
			a_vertexPlane[0] = potentialVertices[5];
			a_vertexPlane[1] = potentialVertices[4];
			a_vertexPlane[2] = potentialVertices[21];
			a_vertexPlane[3] = potentialVertices[20];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[6])
		{
			a_vertexPlane[0] = potentialVertices[6];
			a_vertexPlane[1] = potentialVertices[4];
			a_vertexPlane[2] = potentialVertices[22];
			a_vertexPlane[3] = potentialVertices[20];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[7])
		{
			a_vertexPlane[0] = potentialVertices[7];
			a_vertexPlane[1] = potentialVertices[4];
			a_vertexPlane[2] = potentialVertices[23];
			a_vertexPlane[3] = potentialVertices[20];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		if(useLeftSide)
		{
			if(a_usedVertexSide[8])
			{
				a_vertexPlane[0] = potentialVertices[4];
				a_vertexPlane[1] = potentialVertices[8];
				a_vertexPlane[2] = potentialVertices[20];
				a_vertexPlane[3] = potentialVertices[24];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
			else if(a_usedVertexSide[12])
			{
				a_vertexPlane[0] = potentialVertices[4];
				a_vertexPlane[1] = potentialVertices[12];
				a_vertexPlane[2] = potentialVertices[20];
				a_vertexPlane[3] = potentialVertices[28];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
		}
	}
	//5
	if(a_usedVertexSide[5])
	{
		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[6])
		{
			a_vertexPlane[0] = potentialVertices[6];
			a_vertexPlane[1] = potentialVertices[5];
			a_vertexPlane[2] = potentialVertices[22];
			a_vertexPlane[3] = potentialVertices[21];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[7])
		{
			a_vertexPlane[0] = potentialVertices[7];
			a_vertexPlane[1] = potentialVertices[5];
			a_vertexPlane[2] = potentialVertices[23];
			a_vertexPlane[3] = potentialVertices[21];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[9])
		{
			a_vertexPlane[0] = potentialVertices[5];
			a_vertexPlane[1] = potentialVertices[9];
			a_vertexPlane[2] = potentialVertices[21];
			a_vertexPlane[3] = potentialVertices[25];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[13])
		{
			a_vertexPlane[0] = potentialVertices[5];
			a_vertexPlane[1] = potentialVertices[13];
			a_vertexPlane[2] = potentialVertices[21];
			a_vertexPlane[3] = potentialVertices[29];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
	}
	//6
	if(a_usedVertexSide[6])
	{
		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[7])
		{
			a_vertexPlane[0] = potentialVertices[7];
			a_vertexPlane[1] = potentialVertices[6];
			a_vertexPlane[2] = potentialVertices[23];
			a_vertexPlane[3] = potentialVertices[22];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}

		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[10])
		{
			a_vertexPlane[0] = potentialVertices[10];
			a_vertexPlane[1] = potentialVertices[6];
			a_vertexPlane[2] = potentialVertices[26];
			a_vertexPlane[3] = potentialVertices[22];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[14])
		{
			a_vertexPlane[0] = potentialVertices[14];
			a_vertexPlane[1] = potentialVertices[6];
			a_vertexPlane[2] = potentialVertices[30];
			a_vertexPlane[3] = potentialVertices[22];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
	}
	//7
	if(a_usedVertexSide[7])
	{
		if(useRightSide)
		{
			if(a_usedVertexSide[11])
			{
				a_vertexPlane[0] = potentialVertices[11];
				a_vertexPlane[1] = potentialVertices[7];
				a_vertexPlane[2] = potentialVertices[27];
				a_vertexPlane[3] = potentialVertices[23];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}

			if(a_usedVertexSide[15])
			{
				a_vertexPlane[0] = potentialVertices[15];
				a_vertexPlane[1] = potentialVertices[7];
				a_vertexPlane[2] = potentialVertices[31];
				a_vertexPlane[3] = potentialVertices[23];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
		}
	}
	//8
	if(a_usedVertexSide[8])
	{
		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[9])
		{
			a_vertexPlane[0] = potentialVertices[8];
			a_vertexPlane[1] = potentialVertices[9];
			a_vertexPlane[2] = potentialVertices[24];
			a_vertexPlane[3] = potentialVertices[25];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[10])
		{
			a_vertexPlane[0] = potentialVertices[8];
			a_vertexPlane[1] = potentialVertices[10];
			a_vertexPlane[2] = potentialVertices[24];
			a_vertexPlane[3] = potentialVertices[26];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[11])
		{
			a_vertexPlane[0] = potentialVertices[8];
			a_vertexPlane[1] = potentialVertices[11];
			a_vertexPlane[2] = potentialVertices[24];
			a_vertexPlane[3] = potentialVertices[27];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
	}
	//9
	if(a_usedVertexSide[9])
	{
		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[10])
		{
			a_vertexPlane[0] = potentialVertices[9];
			a_vertexPlane[1] = potentialVertices[10];
			a_vertexPlane[2] = potentialVertices[25];
			a_vertexPlane[3] = potentialVertices[26];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		else if(a_usedVertexSide[11])
		{
			a_vertexPlane[0] = potentialVertices[9];
			a_vertexPlane[1] = potentialVertices[11];
			a_vertexPlane[2] = potentialVertices[25];
			a_vertexPlane[3] = potentialVertices[27];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}

		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[13])
		{
			a_vertexPlane[0] = potentialVertices[9];
			a_vertexPlane[1] = potentialVertices[13];
			a_vertexPlane[2] = potentialVertices[25];
			a_vertexPlane[3] = potentialVertices[29];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}

	}
	//10
	if(a_usedVertexSide[10])
	{
		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[11])
		{
			a_vertexPlane[0] = potentialVertices[10];
			a_vertexPlane[1] = potentialVertices[11];
			a_vertexPlane[2] = potentialVertices[26];
			a_vertexPlane[3] = potentialVertices[27];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
		
		//On est décollé du bord donc pas besoin de connaitre les voisins
		if(a_usedVertexSide[14])
		{
			a_vertexPlane[0] = potentialVertices[14];
			a_vertexPlane[1] = potentialVertices[10];
			a_vertexPlane[2] = potentialVertices[30];
			a_vertexPlane[3] = potentialVertices[26];
			addVertex(m_vertexList, a_vertexPlane);
			addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
		}
	}
	//12
	if(a_usedVertexSide[12])
	{
		if(useFrontSide)
		{
			if(a_usedVertexSide[14])
			{
				a_vertexPlane[0] = potentialVertices[12];
				a_vertexPlane[1] = potentialVertices[14];
				a_vertexPlane[2] = potentialVertices[28];
				a_vertexPlane[3] = potentialVertices[30];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
			else if(a_usedVertexSide[15])
			{
				a_vertexPlane[0] = potentialVertices[12];
				a_vertexPlane[1] = potentialVertices[15];
				a_vertexPlane[2] = potentialVertices[28];
				a_vertexPlane[3] = potentialVertices[31];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
		}
	}
	//13
	if(a_usedVertexSide[13])
	{
		if(useFrontSide)
		{
			if(a_usedVertexSide[14])
			{
				a_vertexPlane[0] = potentialVertices[13];
				a_vertexPlane[1] = potentialVertices[14];
				a_vertexPlane[2] = potentialVertices[29];
				a_vertexPlane[3] = potentialVertices[30];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
			else if(a_usedVertexSide[15])
			{
				a_vertexPlane[0] = potentialVertices[13];
				a_vertexPlane[1] = potentialVertices[15];
				a_vertexPlane[2] = potentialVertices[29];
				a_vertexPlane[3] = potentialVertices[31];
				addVertex(m_vertexList, a_vertexPlane);
				addUVVertical(xCenter, yCenter, zCenter, m_vertexList, a_vertexPlane, index);
			}
		}
	}

	//On ajoute les dommages déjà subis
	float damage((getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT) - getFloatFromMap(i_x, i_y, i_z, CUBE_HITPOINT))/getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT));
	for(int i(vertexOldSize), newVertexSize(m_vertexList.size()); i < newVertexSize; ++i)
		i_damageList.push_back(damage);

	//On ajoute l'index
	for(int i(vertexOldSize), newSize(m_vertexList.size()); i < newSize; ++i)
	{
		m_indexList.push_back(i);
		i_textureNbList.push_back(abs(m_map[i_x][i_y][i_z][CUBE_TYPE]));
		i_renforcementList.push_back(abs(m_map[i_x][i_y][i_z][CUBE_RENFORCEMENT]));
	}
	//On modifie le lien vers les sommets pour le cube 
	m_map[i_x][i_y][i_z][CUBE_NB_VERTEX] = m_vertexList.size() - vertexOldSize;
	if(m_map[i_x][i_y][i_z][CUBE_NB_VERTEX] != 0)
		m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX] = vertexOldSize;
	else
		m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX] = -1;
}
////////////////////////////////////////////////////////////////////////
///
/// @fn bool sameSign(float i_value1, float i_value2)
///
/// Fonction determinant si deux valeur on le même signe
/// 
/// @param[in] i_value1 : valeur 1
/// @param[in] i_value2 : valeur 2
///
/// @return vrai si du même signe
///
////////////////////////////////////////////////////////////////////////
bool sameSign(float i_value1, float i_value2)
{
	return (i_value1 > 0.0f && i_value2 > 0.0f) || (i_value1 < 0.0f && i_value2 < 0.0f);
}

void Chunk::createFlatTop(int i_x, int i_y, int i_z)
{
	VertexPosition vertexPosition;
	vertexPosition.normal[0] = 0.0f;
	vertexPosition.normal[1] = 0.0f;
	vertexPosition.normal[2] = 0.0f;
	vertexPosition.y = i_y * SIDE + HALFSIDE;

	vertexPosition.x = i_x * SIDE - HALFSIDE;
	vertexPosition.z = i_z * SIDE - HALFSIDE;
	m_vertexList.push_back(vertexPosition);

	vertexPosition.x = i_x * SIDE - HALFSIDE;
	vertexPosition.z = i_z * SIDE + HALFSIDE;
	m_vertexList.push_back(vertexPosition);

	vertexPosition.x = i_x * SIDE + HALFSIDE;
	vertexPosition.z = i_z * SIDE - HALFSIDE;
	m_vertexList.push_back(vertexPosition);

	vertexPosition.x = i_x * SIDE + HALFSIDE;
	vertexPosition.z = i_z * SIDE - HALFSIDE;
	m_vertexList.push_back(vertexPosition);
	
	vertexPosition.x = i_x * SIDE - HALFSIDE;
	vertexPosition.z = i_z * SIDE + HALFSIDE;
	m_vertexList.push_back(vertexPosition);

	vertexPosition.x = i_x * SIDE + HALFSIDE;
	vertexPosition.z = i_z * SIDE + HALFSIDE;
	m_vertexList.push_back(vertexPosition);


}

////////////////////////////////////////////////////////////////////////
///
/// @fn void Chunk::createCubeMesh(int i_x, int i_y, int i_z, 
///			std::vector<TVertex_VC>& i_vertexList, std::vector<GLuint>& i_indexList, std::vector<VertexUV>& i_UVList, std::vector<GLfloat>& i_textureNbList)
///
/// Fonction permetant d'ajouter le cube au tableau à envoyer à la carte graphique
/// 
/// @param[in] i_x : Coordonée en x du cube (valeur relative)
/// @param[in] i_y : Coordonée en z du cube (valeur relative)
/// @param[in] i_z : Coordonée en x du cube (valeur relative)
/// @param[in] i_vertexList : Valeurs de retour des sommets
/// @param[in] i_indexList : Valeurs de retour des index
/// @param[in] i_UVList : Valeurs de retour des coordonnées de sommets
/// @param[in] i_textureNbList : Valeurs de retour des numéros de texture utilisés
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::createNaturalCubeMesh(int i_x, int i_y, int i_z, std::vector<GLint>& i_textureNbList, std::vector<GLint>& i_renforcementList, std::vector<GLfloat>& i_damageList)
{
	int index(m_vertexList.size());
	Terrain* p_terrain(Terrain::getInstance());

	std::vector<VertexPosition*> vertexList;

	//On trouve les sommets qui sont possiblement au même endroit que le cube à lisser
	std::hash_map<Position, std::pair<Vector, std::vector<VertexPosition*>>> samePositionList;
	Position tempPosition;

	GridCell grid;
	grid.p[0] = Vector(i_x * SIDE + HALFSIDE, i_y * SIDE - HALFSIDE, i_z * SIDE + HALFSIDE);
	grid.p[1] = Vector(i_x * SIDE - HALFSIDE, i_y * SIDE - HALFSIDE, i_z * SIDE + HALFSIDE);
	grid.p[2] = Vector(i_x * SIDE - HALFSIDE, i_y * SIDE - HALFSIDE, i_z * SIDE - HALFSIDE);
	grid.p[3] = Vector(i_x * SIDE + HALFSIDE, i_y * SIDE - HALFSIDE, i_z * SIDE - HALFSIDE);

	grid.p[4] = Vector(i_x * SIDE + HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE + HALFSIDE);
	grid.p[5] = Vector(i_x * SIDE - HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE + HALFSIDE);
	grid.p[6] = Vector(i_x * SIDE - HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE - HALFSIDE);
	grid.p[7] = Vector(i_x * SIDE + HALFSIDE, i_y * SIDE + HALFSIDE, i_z * SIDE - HALFSIDE);

	for(int i(0); i < 8; ++i)
		grid.val[i] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_0F + i);

	double isolevel(0.0f);

	int vertexOldSize(m_vertexList.size());
	if(isSmoothTopRenderable(i_x, i_y, i_z))
		polygonise(grid, isolevel, m_vertexList);
	else if(!mk_isTerrainChunk && i_y == m_sizeY - 1)
	{
		createFlatTop(i_x, i_y, i_z);
	}

	int currentVertex(vertexOldSize);
	int currentLastVertex(m_vertexList.size());

	while(currentVertex < currentLastVertex)
	{
		if((m_vertexList[currentVertex].x == m_vertexList[currentVertex + 1].x 
		&&  m_vertexList[currentVertex].y == m_vertexList[currentVertex + 1].y 
		&&  m_vertexList[currentVertex].z == m_vertexList[currentVertex + 1].z)

		|| (m_vertexList[currentVertex].x == m_vertexList[currentVertex + 2].x 
		&&  m_vertexList[currentVertex].y == m_vertexList[currentVertex + 2].y 
		&&  m_vertexList[currentVertex].z == m_vertexList[currentVertex + 2].z)

		|| (m_vertexList[currentVertex + 1].x == m_vertexList[currentVertex + 2].x 
		&&  m_vertexList[currentVertex + 1].y == m_vertexList[currentVertex + 2].y 
		&&  m_vertexList[currentVertex + 1].z == m_vertexList[currentVertex + 2].z))
		{
			m_vertexList[currentVertex] = m_vertexList[currentLastVertex - 3];

			m_vertexList[currentVertex + 1] = m_vertexList[currentLastVertex - 2];

			m_vertexList[currentVertex + 2] = m_vertexList[currentLastVertex - 1];

			m_vertexList.pop_back();
			m_vertexList.pop_back();
			m_vertexList.pop_back();
			currentLastVertex -= 3;
		}
		else
			currentVertex += 3;
	}

	//On calcule les uvs
	for(int i(vertexOldSize), newVertexSize(m_vertexList.size()); i < newVertexSize; ++i)
	{

		m_vertexList[index].color = generateCubeColor(i_x, i_y, i_z);

		m_vertexList[index].u = (m_vertexList[index].x - i_x * SIDE + HALFSIDE)/SIDE;
		assert(m_vertexList[index].u <= 1.0f && m_vertexList[index].u >= 0.0f);

		m_vertexList[index].v = (m_vertexList[index].z - i_z * SIDE + HALFSIDE)/SIDE;
		assert(m_vertexList[index].v <= 1.0f && m_vertexList[index].v >= 0.0f);

		//m_vertexList[i].y;

		++index;
	}

	int vertexOldEnd(m_vertexList.size());

	addVerticalPolygon(i_x, i_y, i_z, vertexOldSize, vertexOldEnd, LEFT_FACE);
	addVerticalPolygon(i_x, i_y, i_z, vertexOldSize, vertexOldEnd, RIGHT_FACE);
	addVerticalPolygon(i_x, i_y, i_z, vertexOldSize, vertexOldEnd, FRONT_FACE);
	addVerticalPolygon(i_x, i_y, i_z, vertexOldSize, vertexOldEnd, BACK_FACE);
	addBottomPolygon(i_x, i_y, i_z, vertexOldSize, vertexOldEnd);

	//On ajoute les dommages déjà subis
	float damage((getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT) - getFloatFromMap(i_x, i_y, i_z, CUBE_HITPOINT))/getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT));
	for(int i(vertexOldSize), newVertexSize(m_vertexList.size()); i < newVertexSize; ++i)
		i_damageList.push_back(damage);

	//On ajoute l'index
	for(int i(vertexOldSize), newVertexSize(m_vertexList.size()); i < newVertexSize; ++i)
		m_indexList.push_back(i);
	//On ajoute le numérau de texture
	for(int i(vertexOldSize), newVertexSize(m_vertexList.size()); i < newVertexSize; ++i)
		i_textureNbList.push_back(m_map[i_x][i_y][i_z][CUBE_TYPE]);
	//On ajoute le niveau de renforcement
	for(int i(vertexOldSize), newVertexSize(m_vertexList.size()); i < newVertexSize; ++i)
		i_renforcementList.push_back(m_map[i_x][i_y][i_z][CUBE_RENFORCEMENT]);

	//On modifie le lien entre le cube et ses sommets
	m_map[i_x][i_y][i_z][CUBE_NB_VERTEX] = m_vertexList.size() - vertexOldSize;
	if(m_map[i_x][i_y][i_z][CUBE_NB_VERTEX] != 0)
		m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX] = vertexOldSize;
	else
		m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX] = -1;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void createLineMesh(int i_x, int i_y, int i_z, std::vector<VertexPosition>& i_vertex, std::vector<GLuint>& i_indexList)
///
/// Fonction permetant de  créer la liste de sommets pour le mode wire d'affichage
/// 
/// @param[in] i_x : Coordonée en x du cube (valeur relative)
/// @param[in] i_y : Coordonée en z du cube (valeur relative)
/// @param[in] i_z : Coordonée en x du cube (valeur relative)
/// @param[in] i_vertex : liste de sommets
/// @param[in] i_indexList : liste d'index
/// o
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::createLineMesh(int i_x, int i_y, int i_z, std::vector<Vector>& i_vertex, std::vector<GLuint>& i_indexList)
{
	if(i_x >= m_map.size())
		return;
	if(i_y >= m_map[i_x].size())
		return;
	if(i_z >= m_map[i_x][i_y].size())
		return;

	if(m_map[i_x][i_y][i_z][CUBE_NB_VERTEX] == 0)
		return;

	int vertexListOldSize(i_vertex.size());
	Vector vertexPosition;
	const float minimalShift(0.0005f);
	//0
	vertexPosition.x = i_x * SIDE + (HALFSIDE + minimalShift);
	vertexPosition.y = i_y * SIDE - (HALFSIDE + minimalShift);
	vertexPosition.z = i_z * SIDE + (HALFSIDE + minimalShift);
	i_vertex.push_back(vertexPosition);

	//1
	vertexPosition.x = i_x * SIDE - (HALFSIDE + minimalShift);
	vertexPosition.y = i_y * SIDE - (HALFSIDE + minimalShift);
	vertexPosition.z = i_z * SIDE + (HALFSIDE + minimalShift);
	i_vertex.push_back(vertexPosition);

	//2
	vertexPosition.x = i_x * SIDE - (HALFSIDE + minimalShift);
	vertexPosition.y = i_y * SIDE - (HALFSIDE + minimalShift);
	vertexPosition.z = i_z * SIDE - (HALFSIDE + minimalShift);
	i_vertex.push_back(vertexPosition);

	//3
	vertexPosition.x = i_x * SIDE + (HALFSIDE + minimalShift);
	vertexPosition.y = i_y * SIDE - (HALFSIDE + minimalShift);
	vertexPosition.z = i_z * SIDE - (HALFSIDE + minimalShift);
	i_vertex.push_back(vertexPosition);

	//4
	vertexPosition.x = i_x * SIDE + (HALFSIDE + minimalShift);
	vertexPosition.y = i_y * SIDE + (HALFSIDE + minimalShift);
	vertexPosition.z = i_z * SIDE + (HALFSIDE + minimalShift);
	i_vertex.push_back(vertexPosition);

	//5
	vertexPosition.x = i_x * SIDE - (HALFSIDE + minimalShift);
	vertexPosition.y = i_y * SIDE + (HALFSIDE + minimalShift);
	vertexPosition.z = i_z * SIDE + (HALFSIDE + minimalShift);
	i_vertex.push_back(vertexPosition);

	//6
	vertexPosition.x = i_x * SIDE - (HALFSIDE + minimalShift);
	vertexPosition.y = i_y * SIDE + (HALFSIDE + minimalShift);
	vertexPosition.z = i_z * SIDE - (HALFSIDE + minimalShift);
	i_vertex.push_back(vertexPosition);

	//7
	vertexPosition.x = i_x * SIDE + (HALFSIDE + minimalShift);
	vertexPosition.y = i_y * SIDE + (HALFSIDE + minimalShift);
	vertexPosition.z = i_z * SIDE - (HALFSIDE + minimalShift);
	i_vertex.push_back(vertexPosition);

	//bottom
	i_indexList.push_back(vertexListOldSize);
	i_indexList.push_back(vertexListOldSize + 1);

	i_indexList.push_back(vertexListOldSize + 1);
	i_indexList.push_back(vertexListOldSize + 2);

	i_indexList.push_back(vertexListOldSize + 2);
	i_indexList.push_back(vertexListOldSize + 3);

	i_indexList.push_back(vertexListOldSize + 3);
	i_indexList.push_back(vertexListOldSize);

	//top
	i_indexList.push_back(vertexListOldSize + 4);
	i_indexList.push_back(vertexListOldSize + 5);

	i_indexList.push_back(vertexListOldSize + 5);
	i_indexList.push_back(vertexListOldSize + 6);

	i_indexList.push_back(vertexListOldSize + 6);
	i_indexList.push_back(vertexListOldSize + 7);

	i_indexList.push_back(vertexListOldSize + 7);
	i_indexList.push_back(vertexListOldSize + 4);

	//côté
	i_indexList.push_back(vertexListOldSize);
	i_indexList.push_back(vertexListOldSize + 4);

	i_indexList.push_back(vertexListOldSize + 1);
	i_indexList.push_back(vertexListOldSize + 5);

	i_indexList.push_back(vertexListOldSize + 2);
	i_indexList.push_back(vertexListOldSize + 6);

	i_indexList.push_back(vertexListOldSize + 3);
	i_indexList.push_back(vertexListOldSize + 7);
}
////////////////////////////////////////////////////////////////////////
///
/// @fn void saveMap(std::ofstream& i_ofstreamFile)
///
/// Fonction permetant d'enregistrer une chunk
/// 
/// @param[in] i_ofstreamFile : fichier utilisé pour enregistrer la chunk
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::saveMap(std::ofstream& i_ofstreamFile)
{
	//On crée une variable pour le header des chunks

	const short k_headerChunk(HEADER_CHUNK);
	const short k_headerCube(HEADER_CUBE);

	writeToFile(i_ofstreamFile, k_headerChunk);

	writeToFile(i_ofstreamFile, m_chunkPosition.x);
	writeToFile(i_ofstreamFile, m_chunkPosition.y);
	writeToFile(i_ofstreamFile, m_chunkPosition.z);

	writeToFile(i_ofstreamFile, m_sizeX);
	writeToFile(i_ofstreamFile, m_sizeY);
	writeToFile(i_ofstreamFile, m_sizeZ);

	std::vector<char> vectBuffer(sizeof(short) + 3 * sizeof(int) + (NB_CUBE_PARAM - NB_UNSAVABLE_CUBE_ATTRIB) * sizeof(int));

	//pour tout les cubes
	for(int i(0); i < m_sizeX; ++i)
		for(int j(0); j < m_sizeY; ++j)
			for(int k(0); k < m_sizeZ; ++k)
			{
				//si le cube existe
				if(m_map[i][j][k][CUBE_TYPE] != 0)
				{
					//On écrit le header du cube
					
					writeToFile(i_ofstreamFile, k_headerCube);
				
					writeToFile(i_ofstreamFile, i);					
					writeToFile(i_ofstreamFile, j);
					writeToFile(i_ofstreamFile, k);

					//On écrit les paramètres
					for(int param(0); param < NB_CUBE_PARAM - NB_UNSAVABLE_CUBE_ATTRIB; ++param)
					{
						writeToFile(i_ofstreamFile, m_map[i][j][k][param]);
					}
				}
			}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void loadMap(std::ifstream& i_ifstreamFile)
///
/// Fonction permetant de charger une chunk
/// 
/// @param[in] i_ifstreamFile : fichier utilisé pour charger la chunk
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
short Chunk::loadMap(std::ifstream& i_ifstreamFile)
{
	readFromFile(i_ifstreamFile, m_sizeX);
	readFromFile(i_ifstreamFile, m_sizeY);
	readFromFile(i_ifstreamFile, m_sizeZ);

	resizeChunk();

	char a_bufferShort[2];
	int x;
	int y;
	int z;
	int param;
	short header;
	//On lit le header
	readFromFile(i_ifstreamFile, header);
	while(header == HEADER_CUBE)
	{
		readFromFile(i_ifstreamFile, x);
		readFromFile(i_ifstreamFile, y);
		readFromFile(i_ifstreamFile, z);

		//On lit tout les paramètres
		for(int n_i(0); n_i < NB_CUBE_PARAM - NB_UNSAVABLE_CUBE_ATTRIB; ++n_i)
		{
			readFromFile(i_ifstreamFile, m_map[x][y][z][n_i]);
		}
		readFromFile(i_ifstreamFile, header);
	}

	return header;
}

void Chunk::updateBuffer()
{
	Shader::chooseShader(GEOM_PASS_SHADER);
	glBindVertexArray(m_VAOID);

	std::vector<VertexPosition> oldVertexList;
	m_vertexList.swap(oldVertexList);

	std::vector<GLfloat> oldDamageList;
	m_damageList.swap(oldDamageList);

	std::vector<GLint> renforcementList;

	std::vector<GLuint> oldIndexList;
	m_indexList.swap(oldIndexList);

	std::vector<GLint> textureNbList;

	std::vector<GLuint> lineIndexList;
	std::vector<VertexPosition> lineVertexList;

	std::set<Vector3I> cubesChanged;

	//On ajoute chaque cube au mesh de la chunk
	for(int i(0); i < m_sizeX ; ++i)
		for(int j(0) ; j < m_sizeY ; ++j)
			for(int k(0); k < m_sizeZ ; ++k)
			{
				int realX(i + m_cubePosition.x);
				int realY(j + m_cubePosition.y);
				int realZ(k + m_cubePosition.z);
				//Si le cube a changer on recalcule les sommets sinon, on reprend les anciens
				if(m_map[i][j][k][CUBE_CHANGED])
				{
					int vertexListSize(m_vertexList.size());
					//Le traitement est différent si c'est un cube artificiel ou naturel
					if(m_map[i][j][k][CUBE_TYPE] > 0)
					{
						createNaturalCubeMesh(i, j, k, textureNbList, renforcementList, m_damageList);
						smoothNormal(i,j,k, oldVertexList);
					}
					else if(m_map[i][j][k][CUBE_TYPE] < 0)
					{
						createArtificialCubeMesh(i, j, k, textureNbList, renforcementList, m_damageList);
					}
					m_map[i][j][k][CUBE_CHANGED] = false;
					cubesChanged.insert(Vector3I(i, j, k));
				}
				else
				{
					if(m_map[i][j][k][CUBE_DRAWING_INDEX] != -1)
					{
						int vertexListSize(m_vertexList.size());
						for(int l(m_map[i][j][k][CUBE_DRAWING_INDEX]), nbVertex(m_map[i][j][k][CUBE_DRAWING_INDEX] + m_map[i][j][k][CUBE_NB_VERTEX]); l < nbVertex; ++l)
						{
							m_vertexList.push_back(oldVertexList[l]);
							m_damageList.push_back(oldDamageList[l]);
							renforcementList.push_back(m_map[i][j][k][CUBE_RENFORCEMENT]);
							m_indexList.push_back(m_vertexList.size() - 1);
							textureNbList.push_back(abs(m_map[i][j][k][CUBE_TYPE]));
						}
						m_map[i][j][k][CUBE_DRAWING_INDEX] = vertexListSize;
					}
				}
			}

	//on ferme les trous
	Terrain* p_terrain(Terrain::getInstance());
	for(int i(0); i < m_sizeX ; ++i)
		for(int j(0) ; j < m_sizeY ; ++j)
			for(int k(0); k < m_sizeZ ; ++k)
			{
				int realX(i + m_cubePosition.x);
				int realY(j + m_cubePosition.y);
				int realZ(k + m_cubePosition.z);
				//Si le cube a changer on recalcule les sommets sinon, on reprend les anciens
				if(cubesChanged.find(Vector3I(i, j, k)) != cubesChanged.end())
				{
					int vertexListSize(m_vertexList.size());
					//Le traitement est différent si c'est un cube artificiel ou naturel
					if(m_map[i][j][k][CUBE_TYPE] > 0)
					{
						if(p_terrain->isNatural(realX, realY, realZ))
						{
							generateExtraTop(i, j, k, textureNbList, renforcementList);
						}
						else
						{
							m_map[i][j][k][CUBE_DRAWING_INDEX_EXTRA_TOP] = -1;
							m_map[i][j][k][CUBE_NB_EXTRA_TOP] = 0;
						}
					}
				}
				else
				{
					if(m_map[i][j][k][CUBE_DRAWING_INDEX_EXTRA_TOP] != -1)
					{

						if(m_map[i][j][k][CUBE_DRAWING_INDEX_EXTRA_TOP] != -1 || m_map[i][j][k][CUBE_NB_EXTRA_TOP] != 0)
						{
							int vertexListSize(m_vertexList.size());
							for(int l(m_map[i][j][k][CUBE_DRAWING_INDEX_EXTRA_TOP]), nbVertex(m_map[i][j][k][CUBE_DRAWING_INDEX_EXTRA_TOP] + m_map[i][j][k][CUBE_NB_EXTRA_TOP]); l < nbVertex; ++l)
							{
								m_vertexList.push_back(oldVertexList[l]);
								m_damageList.push_back(oldDamageList[l]);
								renforcementList.push_back(m_map[i][j][k][CUBE_RENFORCEMENT]);
								textureNbList.push_back(abs(m_map[i][j][k][CUBE_TYPE]));
							}
							m_map[i][j][k][CUBE_DRAWING_INDEX_EXTRA_TOP] = vertexListSize;
							generateExtraTopIndex(i, j, k);
						}
					}
				}
			}

	Shader* p_currentShader(Shader::getShader());

	//Création de l'IBO
	//On définie le buffer comme étant le buffer courant
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[INDEX_BUFFER]);
	m_indexSize = m_indexList.size();
	if(m_indexList.size() != 0)
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexList.size() * sizeof(m_indexList[0]), &m_indexList[0], GL_DYNAMIC_DRAW);
	

	//Création du VBO
	//On définie le buffer comme étant le buffer courant
	int attribVariable(p_currentShader->getAttribVariableSafe("InVertex"));

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[POS_VB]);
	if(m_vertexList.size() != 0)
		glBufferData(GL_ARRAY_BUFFER, m_vertexList.size() * sizeof(m_vertexList[0]), &m_vertexList[0], GL_DYNAMIC_DRAW);	
	glEnableVertexAttribArray(attribVariable);
	glVertexAttribPointer(attribVariable, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosition), BUFFER_OFFSET(0));
	//On envoie les normales
	attribVariable = p_currentShader->getAttribVariableSafe("InNormal");
	glEnableVertexAttribArray(attribVariable);
	glVertexAttribPointer(attribVariable, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosition), BUFFER_OFFSET(3*sizeof(float)));
	//On envoie la couleur
	attribVariable = p_currentShader->getAttribVariableSafe("InColor");
	glEnableVertexAttribArray(attribVariable);
	glVertexAttribIPointer(attribVariable, 1, GL_INT, sizeof(VertexPosition), BUFFER_OFFSET(6*sizeof(float)));

	//On envoie les coordonnées de sommet
	attribVariable = p_currentShader->getAttribVariableSafe("InUV");
	glEnableVertexAttribArray(attribVariable);
	glVertexAttribPointer(attribVariable, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosition), BUFFER_OFFSET(6*sizeof(float) + sizeof(int)));

	//Création du VBO
	//On définie le buffer comme étant le buffer courant
	attribVariable = p_currentShader->getAttribVariableSafe("InTextureNb");

	glBindBuffer(GL_ARRAY_BUFFER, m_textureNbID);
	if(textureNbList.size() != 0)
		glBufferData(GL_ARRAY_BUFFER, textureNbList.size() * sizeof(textureNbList[0]), &textureNbList[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(attribVariable);
	glVertexAttribIPointer(attribVariable, 1, GL_INT, 0, 0);

	//Création du VBO
	//On définie le buffer comme étant le buffer courant

	attribVariable = p_currentShader->getAttribVariableSafe("InDamage");

	glBindBuffer(GL_ARRAY_BUFFER, m_damageID);
	if(m_damageList.size() != 0)
		glBufferData(GL_ARRAY_BUFFER, m_damageList.size() * sizeof(m_damageList[0]), &m_damageList[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(attribVariable);
	glVertexAttribPointer(attribVariable, 1, GL_FLOAT, GL_FALSE, 0, 0);

	//blindage
	attribVariable = p_currentShader->getAttribVariableSafe("InRenforcement");

	glBindBuffer(GL_ARRAY_BUFFER, m_renforcementID);
	if(renforcementList.size() != 0)
		glBufferData(GL_ARRAY_BUFFER, renforcementList.size() * sizeof(renforcementList[0]), &renforcementList[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(attribVariable);
	glVertexAttribPointer(attribVariable, 1, GL_INT, GL_FALSE, 0, 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_whiteTerrainLine.fillBuffer();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void update()
///
/// Fonction permettant de mettre à jour le mesh de la chunk
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::updateAndSetBtObject( btDynamicsWorld* i_dynamicsWorld )
{
	updateBuffer();
	
	//Physique
	if(mk_isTerrainChunk)
	{
		exitPhysics();
		if(i_dynamicsWorld && m_staticBody)
		{
			i_dynamicsWorld->removeRigidBody(m_staticBody);
			delete m_staticBody;
			m_staticBody = NULL;
		}	
		initPhysics();
		if(i_dynamicsWorld && m_staticBody)
			i_dynamicsWorld->addRigidBody(m_staticBody, COL_WALL, COL_UNIT | COL_SHIP | COL_TERRAIN_RAYCAST);	
	}
}
////////////////////////////////////////////////////////////////////////
///
/// @fn void updateChunkOverview(int i_x, int i_z)
///
/// Fonction mettant à jour la couleur d'une colonne du chunk
///
/// @param[in] i_x : numéro de colonne en x
/// @param[in] i_z : numéro de colonne en y
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::updateChunkOverview(int i_x, int i_z)
{
	Terrain* p_terrain(Terrain::getInstance());
	MapGUI* p_map((MapGUI*)GUIHandler::getInstance()->getLayout(MAP_LAYOUT));

	int terrainHeightInCube(p_terrain->getHeight() * m_sizeY);
	int chunkOffset(m_cubePosition.y);

	std::vector<int> colors;

	m_chunkOverview[i_x][i_z][0] = 0;
	m_chunkOverview[i_x][i_z][1] = 0;
	m_chunkOverview[i_x][i_z][2] = 0;
	//en partant du haut, on trouve le premier cube visible et on prend la couleur moyenne de la texture que l'on multiplie avec sa hauteur absolue
	for(int y(m_sizeY - 1); y >= 0; --y)
	{
		if(m_map[i_x][y][i_z][CUBE_TYPE] != 0)
		{
			colors.swap(p_map->getColorAverage(m_map[i_x][y][i_z][CUBE_TYPE]));
			for(int colorIndex(0); colorIndex < 3; ++colorIndex)
				m_chunkOverview[i_x][i_z][colorIndex] = colors[colorIndex] * (((float)y + chunkOffset + 1.0f)/(float)terrainHeightInCube);
			break;
		}
	}

	p_terrain->updateMapOverview(m_chunkPosition.x, m_chunkPosition.z, i_x, i_z);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void updateChunkOverview()
///
/// Fonction mettant à jour les couleurs de chaque colonne du chunk
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::updateChunkOverview()
{
	for(int x(0); x < m_sizeX; ++x)
		for(int z(0); z < m_sizeZ; ++z)
			updateChunkOverview(x, z);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void addFloatToMap(int i_x, int i_y, int i_z, int i_cubeInfo, float i_value)
///
/// Fonction permettant d'ajouter une valeur décimal au tableau d'information d'une voxel
///
/// @param[in] i_x : position du cube en x
/// @param[in] i_x : position du cube en y
/// @param[in] i_z : position du cube en z
/// @param[in] i_cubeInfo : numéro du champs à modifier
/// @param[in] i_value : valeur à ajouter 
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::setFloatToMap(int i_x, int i_y, int i_z, int i_cubeInfo, float i_value)
{
	assert(sizeof(m_map[i_x][i_y][i_z][i_cubeInfo]) == sizeof(i_value));


	int realX(i_x + m_cubePosition.x);
	int realY(i_y + m_cubePosition.y);
	int realZ(i_z + m_cubePosition.z);
	memcpy(&m_map[i_x][i_y][i_z][i_cubeInfo], &i_value, sizeof(i_value));
}

////////////////////////////////////////////////////////////////////////
///
/// @fn float getFloatFromMap(int i_x, int i_y, int i_z, int i_cubeInfo)
///
/// Fonction permettant de récuperer une valeur décimal au tableau d'information d'une voxel
///
/// @param[in] i_x : position du cube en x
/// @param[in] i_x : position du cube en y
/// @param[in] i_z : position du cube en z
/// @param[in] i_cubeInfo : numéro du champs à lire
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
float Chunk::getFloatFromMap(int i_x, int i_y, int i_z, int i_cubeInfo)const
{
	float result;
	assert(sizeof(m_map[i_x][i_y][i_z][i_cubeInfo]) == sizeof(result));
	memcpy(&result, &m_map[i_x][i_y][i_z][i_cubeInfo], sizeof(m_map[i_x][i_y][i_z][i_cubeInfo]));
	return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void modifySingleDensity(int i_x, int i_y, int i_z, unsigned int i_index, float i_density)
///
/// Fonction permettant de modifier le poids d'un sommets d'un cube
///
/// @param[in] i_x : position du cube en x
/// @param[in] i_x : position du cube en y
/// @param[in] i_z : position du cube en z
/// @param[in] i_density : valeurs de poids
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::setSingleDensity(int i_x, int i_y, int i_z, unsigned int i_index, float i_density)
{
	assert(CUBE_HIGH_0F <= i_index && i_index <= CUBE_HIGH_7F);
	setFloatToMap(i_x, i_y, i_z, i_index , i_density);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void smoothNormal(int i_x, int i_y, int i_z, std::vector<VertexPosition>& i_oldVertexList)
///
/// Fonction permettant de lisser les normals pour un cube et ses voisins
///
/// @param[in] i_x : position du cube en x
/// @param[in] i_x : position du cube en y
/// @param[in] i_z : position du cube en z
/// @param[in] i_oldVertexList : ancienne liste de tout les vertex
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::smoothNormal(int i_x, int i_y, int i_z, std::vector<VertexPosition>& i_oldVertexList)
{
	Terrain* p_terrain(Terrain::getInstance());

	std::vector<VertexPosition*> vertexList;

	//On trouve les sommets qui sont possiblement au même endroit que le cube à lisser
	std::hash_map<Position, std::pair<Vector, std::vector<VertexPosition*>>> samePositionList;
	Position tempPosition;

	for(int x(i_x - 1), xSize(i_x + 1); x <= xSize; ++x)
		for(int y(i_y - 1), ySize(i_y + 1); y <= ySize; ++y)
			for(int z(i_z - 1), zSize(i_z + 1); z <= zSize; ++z)
			{
				if(isNaturalIntern(x, y, z))
				{
					//On extrait les sommets du cube
					if((x < i_x || (x == i_x && (y < i_y || (y == i_y && z <= i_z)))) ||  //Le cube a déjà été transféré
						i_oldVertexList.size() == 0 || //C'est la première itération
						x/m_sizeX != i_x/m_sizeX || y/m_sizeY != i_y/m_sizeY || z/m_sizeZ != i_z/m_sizeZ) //On change de chunk
					{
						if(x/m_sizeX != i_x/m_sizeX || y/m_sizeY != i_y/m_sizeY || z/m_sizeZ != i_z/m_sizeZ)
							p_terrain->addToUpdateList(x/m_sizeX, y/m_sizeY, z/m_sizeZ);

						getVertexIntern(x, y, z, vertexList);
					}
					else
						getVertexIntern(x, y, z, vertexList, i_oldVertexList);
				}
			}
	//On regroupe les sommets similaires et on calcule la normale
	{
		Vector tempNormal;
		Vector tempU;
		Vector tempV;

		std::vector<Vector> transformedVertexList(vertexList.size());
		Matrix transformation;
		transformation.loadIdentity();
		transformation.translate(m_cubePosition.x * SIDE, m_cubePosition.y * SIDE, m_cubePosition.z * SIDE);
		for(int i(0), vertexListSize(vertexList.size()); i < vertexListSize; ++i)
		transformedVertexList[i] = transformation * Vector(vertexList[i]->x, vertexList[i]->y, vertexList[i]->z);

		for(int i(0), vertexListSize(vertexList.size()); i < vertexListSize; ++i)
		{
			if(i % 3 == 0)
			{
				tempU.x = transformedVertexList[i + 1].x - transformedVertexList[i].x;
				tempV.x = transformedVertexList[i + 2].x - transformedVertexList[i].x;

				tempU.y = transformedVertexList[i + 1].y - transformedVertexList[i].y;
				tempV.y = transformedVertexList[i + 2].y - transformedVertexList[i].y;

				tempU.z = transformedVertexList[i + 1].z - transformedVertexList[i].z;
				tempV.z = transformedVertexList[i + 2].z - transformedVertexList[i].z;

				tempNormal = tempU.crossProduct(tempV).normalize();
			}

			tempPosition.x = transformedVertexList[i].x;
			tempPosition.y = transformedVertexList[i].y;
			tempPosition.z = transformedVertexList[i].z;
			
			//On exclue les faces verticals
			if(tempNormal.y != 0)
			{
				samePositionList[tempPosition].first += tempNormal;
				samePositionList[tempPosition].second.push_back(vertexList[i]);
			}
			else
			{
				vertexList[i]->normal[0] = tempNormal.x;
				vertexList[i]->normal[1] = tempNormal.y;
				vertexList[i]->normal[2] = tempNormal.z;
			}
		}
	}

	//On trouve les limites du cube à smoother
	int realX(i_x + m_cubePosition.x);
	int realY(i_y + m_cubePosition.y);
	int realZ(i_z + m_cubePosition.z);

	float minX(realX * SIDE - HALFSIDE);
	float maxX(realX * SIDE + HALFSIDE);
	float minY(realY * SIDE - HALFSIDE);
	float maxY(realY * SIDE + HALFSIDE);
	float minZ(realZ * SIDE - HALFSIDE);
	float maxZ(realZ * SIDE + HALFSIDE);

	for(std::hash_map<Position, std::pair<Vector, std::vector<VertexPosition*>>>::iterator it_samePositionList(samePositionList.begin()), samePositionListEnd(samePositionList.end()); 
	it_samePositionList != samePositionListEnd; 
	++it_samePositionList)
	{
		if(minX <= it_samePositionList->first.x && it_samePositionList->first.x <= maxX &&
			minY <= it_samePositionList->first.y && it_samePositionList->first.y <= maxY &&
			minZ <= it_samePositionList->first.z && it_samePositionList->first.z <= maxZ)
		{
			it_samePositionList->second.first = it_samePositionList->second.first.normalize();
			for(int i(0), positionListSize(it_samePositionList->second.second.size()); i < positionListSize; ++i)
			{
				it_samePositionList->second.second[i]->normal[0] = it_samePositionList->second.first.x;
				it_samePositionList->second.second[i]->normal[1] = it_samePositionList->second.first.y;
				it_samePositionList->second.second[i]->normal[2] = it_samePositionList->second.first.z;
			}
		}
	}	
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool isTopRendable(int i_x, int i_y, int i_z)
///
/// Fonction vérifiant si l'isosurface est rendu
///
/// @param[in] i_x : position du cube en x
/// @param[in] i_x : position du cube en y
/// @param[in] i_z : position du cube en z
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Chunk::isSmoothTopRenderable(int i_x, int i_y, int i_z)const
{	
	bool lower(false);
	bool higher(false);

	for(int i(0); i < 4; ++i)
	{
		float isoSurface(getIsoSurfaceRelativeHeight(i_x, i_y, i_z, CUBE_HIGH_0F + i));
		if(0 < isoSurface && isoSurface < 1)
			return true;
		else
		{
			lower |= 0 > isoSurface;
			higher |= isoSurface > 1;
			if(lower && higher)
				return true;
		}
	}

	return false;
}

bool Chunk::isFlatTopRenderable(int i_x, int i_y, int i_z)const
{	
	Terrain* p_terrain(Terrain::getInstance());
	int realX(m_cubePosition.x + i_x);
	int realY(m_cubePosition.y + i_y);
	int realZ(m_cubePosition.z + i_z);

	float isoPosition[4];
	float topIsoPosition[4];

	for(int i(0); i < 4; ++i)
		isoPosition[i] = p_terrain->getIsoPosition(realX, realY, realZ, CUBE_HIGH_0F + i);
	for(int i(0); i < 4; ++i)
		topIsoPosition[i] = p_terrain->getIsoPosition(realX, realY + 1, realZ, CUBE_HIGH_0F + i);

	if(p_terrain->isAir(realX, realY + 1, realZ) || p_terrain->isArtificial(realX, realY + 1, realZ))
		for(int i(0); i < 4; ++i)
			if(isoPosition[i] > 1)
				return true;

	if(p_terrain->isNatural(realX, realY + 1, realZ))
		for(int i(0); i < 4; ++i)
			if(isoPosition[i] > 1 && topIsoPosition[i] < 0)
				return true;
	return false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void setNeighbor(int& i_x, int& i_y, int& i_z, FaceType i_faceType)
///
/// Fonction déterminant les voisins du cube selon le facetype
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_faceType : type de face 
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void setNeighbor(int& i_x, int& i_y, int& i_z, FaceType i_faceType)
{
	switch(i_faceType)
	{
		case LEFT_FACE:
			++i_x;
			break;
		case RIGHT_FACE:
			--i_x;
			break;
		case FRONT_FACE:
			--i_z;
			break;
		case BACK_FACE:
			++i_z;
			break;
		default:
			assert(false);
			break;
	}
}

enum IsosurfaceState{ISOSURFACE_HIGHER, ISOSURFACE_LOWER, ISOSURFACE_EQUAL, ISOSURFACE_ERROR};
////////////////////////////////////////////////////////////////////////
///
/// @fn int faceGotIsosurface(int i_x, int i_y, int i_z, FaceType i_faceType)
///
/// Détermine comment une isosurface intersect une face
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_faceType : type de face 
///
/// @return IsosurfaceState decrivant l'intersection
///
////////////////////////////////////////////////////////////////////////
int Chunk::faceGotIsosurface(int i_x, int i_y, int i_z, FaceType i_faceType)
{
	float cubeHigh[4];
	switch(i_faceType)
	{
		case LEFT_FACE:
			cubeHigh[0] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_0F);
			cubeHigh[1] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_4F);
			cubeHigh[2] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_3F); 
			cubeHigh[3] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_7F);

			break;
		case RIGHT_FACE:
			cubeHigh[0] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_1F); 
			cubeHigh[1] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_5F);
			cubeHigh[2] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_2F);
			cubeHigh[3] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_6F);
			break;
		case FRONT_FACE:
			cubeHigh[0] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_3F);
			cubeHigh[1] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_7F);
			cubeHigh[2] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_2F);
			cubeHigh[3] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_6F);
			break;
		case BACK_FACE:
			cubeHigh[0] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_0F);
			cubeHigh[1] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_4F);
			cubeHigh[2] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_1F);
			cubeHigh[3] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_5F);
			break;
		default:
			assert(false);
			return ISOSURFACE_ERROR;
	}

	if(!sameSign(cubeHigh[0], cubeHigh[1]) || !sameSign(cubeHigh[2], cubeHigh[3]))
		return ISOSURFACE_EQUAL;
	if(!sameSign(cubeHigh[0], cubeHigh[2]) || !sameSign(cubeHigh[1], cubeHigh[3]))
		return ISOSURFACE_EQUAL;
	if(cubeHigh[0] > 0 && cubeHigh[2] > 0)
		return ISOSURFACE_LOWER;
	else
		return ISOSURFACE_HIGHER;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool isInTheFace(int i_x, int i_y, int i_z, const VertexPosition& i_vertex, FaceType i_faceType)
///
/// Détermine si un sommet est dans la face du cube
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_vertex : sommet à tester
/// @param[in] i_faceType : type de face à tester
///
/// @return vrai si le sommet est dans la face
///
////////////////////////////////////////////////////////////////////////
bool isInTheFace(int i_x, int i_y, int i_z, const VertexPosition& i_vertex, FaceType i_faceType)
{
	switch(i_faceType)
	{
		case LEFT_FACE:
			return (i_x * SIDE + HALFSIDE) == i_vertex.x;
		case RIGHT_FACE:
			return (i_x * SIDE - HALFSIDE) == i_vertex.x;
		case FRONT_FACE:
			return (i_z * SIDE - HALFSIDE) == i_vertex.z;
		case BACK_FACE:
			return (i_z * SIDE + HALFSIDE) == i_vertex.z;
		case BOTTOM_FACE:
			return (i_y * SIDE - HALFSIDE) == i_vertex.y;
		default:
			assert(false);
			return false;
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void addTopCorners(int i_x, int i_y, int i_z, std::vector<VertexPosition>& i_vertexPositionList, FaceType i_faceType)
///
/// Ajouter les sommets au coin d'une face pour les polygones verticaux
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_vertexPositionList : liste des sommets
/// @param[in] i_faceType : type de face
///
/// @return aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::addTopCorners(int i_x, int i_y, int i_z, std::vector<VertexPosition>& i_vertexPositionList, FaceType i_faceType)
{
	VertexPosition vertexPosition;
	vertexPosition.color = generateCubeColor(i_x, i_y, i_z);

	//On trouve les limites de la face
	float sideXHigh(i_x * SIDE + HALFSIDE);
	float sideXLow(i_x * SIDE - HALFSIDE);
	float sideYHigh(i_y * SIDE + HALFSIDE);
	float sideZHigh(i_z * SIDE + HALFSIDE);
	float sideZLow(i_z * SIDE - HALFSIDE);

	CubeInfo cubeInfo[2];
	float x[2];
	float z[2];

	//Selon la face, on trouve les coordonnées
	switch(i_faceType)
	{
		case LEFT_FACE:
			cubeInfo[0] = CUBE_HIGH_4F;
			x[0] = sideXHigh;
			z[0] = sideZHigh;

			cubeInfo[1] = CUBE_HIGH_7F;
			x[1] = sideXHigh;
			z[1] = sideZLow;
			break;
		case RIGHT_FACE:
			cubeInfo[0] = CUBE_HIGH_5F;
			x[0] = sideXLow;
			z[0] = sideZHigh;

			cubeInfo[1] = CUBE_HIGH_6F;
			x[1] = sideXLow;
			z[1] = sideZLow;
			break;
		case FRONT_FACE:
			cubeInfo[0] = CUBE_HIGH_6F;
			x[0] = sideXLow;
			z[0] = sideZLow;

			cubeInfo[1] = CUBE_HIGH_7F;
			x[1] = sideXHigh;
			z[1] = sideZLow;
			break;
		case BACK_FACE:
			cubeInfo[0] = CUBE_HIGH_5F;
			x[0] = sideXLow;
			z[0] = sideZHigh;

			cubeInfo[1] = CUBE_HIGH_4F;
			x[1] = sideXHigh;
			z[1] = sideZHigh;
			break;
		default:
			assert(false);
			break;
	}

	//Si l'isosurface est plus haute que le sommets trouvé, on ajoute le sommets
	for(int i(0); i < 2; ++i)
		if(getFloatFromMap(i_x, i_y, i_z, cubeInfo[i]) < 0)
		{				
			vertexPosition.x = x[i];
			vertexPosition.y = sideYHigh;
			vertexPosition.z = z[i];
			i_vertexPositionList.push_back(vertexPosition);
		}				
}

////////////////////////////////////////////////////////////////////////
///
/// @fn float findU(int i_x, int i_y, int i_z, const VertexPosition& i_vertexPosition, FaceType i_faceType)
///
/// Fonction permettant de trouver la composante U de la coordonnée de texture
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_vertexPositionList : liste des sommets
/// @param[in] i_faceType : type de face
///
/// @return valeur U de la coordonné de texture
///
////////////////////////////////////////////////////////////////////////
float findU(int i_x, int i_y, int i_z, const VertexPosition& i_vertexPosition, FaceType i_faceType)
{
	switch(i_faceType)
	{
		case LEFT_FACE:
		case RIGHT_FACE:
			return (i_vertexPosition.z - (i_z * SIDE - HALFSIDE))/SIDE;
		case FRONT_FACE:
		case BACK_FACE:
			return (i_vertexPosition.x - (i_x * SIDE - HALFSIDE))/SIDE;
		default:
			assert(false);
			return 0.0f;
	}	
}
void setNormalFromFaceType(VertexPosition& ip_vertexPosition, FaceType i_faceType)
{
	switch(i_faceType)
	{
		case LEFT_FACE:
			ip_vertexPosition.normal[0] = 1.f;
			ip_vertexPosition.normal[1] = 0.f;
			ip_vertexPosition.normal[2] = 0.f;
			break;
		case RIGHT_FACE:
			ip_vertexPosition.normal[0] = -1.f;
			ip_vertexPosition.normal[1] = 0.f;
			ip_vertexPosition.normal[2] = 0.f;
			break;
		case FRONT_FACE:
			ip_vertexPosition.normal[0] = 0.f;
			ip_vertexPosition.normal[1] = 1.f;
			ip_vertexPosition.normal[2] = 0.f;
			break;
		case BACK_FACE:
			ip_vertexPosition.normal[0] = 0.f;
			ip_vertexPosition.normal[1] = 0.f;
			ip_vertexPosition.normal[2] = -1.f;
			break;
		default:
			assert(false);
			break;
	}
}
////////////////////////////////////////////////////////////////////////
///
/// @fn void addCompleteFace(int i_x, int i_y, int i_z, std::vector<VertexPosition>& i_vertex, std::vector<VertexUV>& i_UVList, FaceType i_faceType)
///
/// Fonction permettant d'ajouter une plaque complète à une face d'un cube
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_vertex : liste des sommets
/// @param[in] i_UVList : liste des coordonnées de texture
/// @param[in] i_faceType : type de face
///
/// @return aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::addCompleteFace(int i_x, int i_y, int i_z, std::vector<VertexPosition>& i_vertex, FaceType i_faceType)
{
	VertexPosition vertexPosition;
	vertexPosition.color = generateCubeColor(i_x, i_y, i_z);

	setNormalFromFaceType(vertexPosition, i_faceType);
	switch(i_faceType)
	{
		case LEFT_FACE:
			//0
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;	
			vertexPosition.u = 0.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);

			//1
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);

			//2
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);

			//3
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);

			//4
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);

			//5
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;	
			vertexPosition.u = 0.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);
			break;
		case RIGHT_FACE:
			//0
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;		
			vertexPosition.u = 1.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);

			//1
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);

			//2
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);

			//3
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);

			//4
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);
			//5
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);
			break;
		case FRONT_FACE:
			//0
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);

			//1
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);

			//2
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);

			//3
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);

			//4
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);

			//5
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);
			break;
		case BACK_FACE:
			//0
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);

			//1
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);

			//2
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);

			//3
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE + HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);

			//4
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 0.0f;
			i_vertex.push_back(vertexPosition);

			//5
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 1.0f;
			i_vertex.push_back(vertexPosition);
			break;
		default:
			assert(false);
			break;
	}	

}
//Comparaison de deux sommets selon leur coordonnées
bool compX(const VertexPosition& i_vertexLeft, const VertexPosition& i_vertexRight){ return i_vertexLeft.x < i_vertexRight.x; }
bool compY(const VertexPosition& i_vertexLeft, const VertexPosition& i_vertexRight){ return i_vertexLeft.y < i_vertexRight.y; }
bool compZ(const VertexPosition& i_vertexLeft, const VertexPosition& i_vertexRight){ return i_vertexLeft.z < i_vertexRight.z; }

//Comparaison inverse de deux sommets selon leur coordonnées
bool compXInv(const VertexPosition& i_vertexLeft, const VertexPosition& i_vertexRight){ return i_vertexLeft.x > i_vertexRight.x; }
bool compYInv(const VertexPosition& i_vertexLeft, const VertexPosition& i_vertexRight){ return i_vertexLeft.y > i_vertexRight.y; }
bool compZInv(const VertexPosition& i_vertexLeft, const VertexPosition& i_vertexRight){ return i_vertexLeft.z > i_vertexRight.z; }

////////////////////////////////////////////////////////////////////////
///
/// @fn void addVerticalPolygon(int i_x, int i_y, int i_z, int i_vertexOldStart, int i_vertexOldEnd, std::vector<VertexUV>& i_UVList, FaceType i_faceType)
///
/// Fonction permettant d'ajouter une face vertical à un cube
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_vertexOldStart : index du début des sommets relié au cube
/// @param[in] i_vertexOldEnd : fin de l'index des sommets relié au cube
/// @param[in] i_UVList : liste des coordonnées de texture
/// @param[in] i_faceType : type de face
///
/// @return aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::addVerticalPolygon(int i_x, int i_y, int i_z, int i_vertexOldStart, int i_vertexOldEnd, FaceType i_faceType)
{
	Terrain* p_terrain(Terrain::getInstance());
	int realX(i_x + m_cubePosition.x);
	int realY(i_y + m_cubePosition.y);
	int realZ(i_z + m_cubePosition.z);

	int realNeighborX(realX);
	int realNeighborY(realY);
	int realNeighborZ(realZ);

	setNeighbor(realNeighborX, realNeighborY, realNeighborZ, i_faceType);

	int neighborX(i_x);
	int neighborY(i_y);
	int neighborZ(i_z);

	setNeighbor(neighborX, neighborY, neighborZ, i_faceType);

	VertexPosition vertexPosition;
	vertexPosition.color = generateCubeColor(i_x, i_y, i_z);
	setNormalFromFaceType(vertexPosition, i_faceType);

	//Le voisin doit être à l'intérieur des limites
	if(!mk_isTerrainChunk || p_terrain->isInLimit(realNeighborX, realNeighborY, realNeighborZ))
		if(!isNaturalIntern(neighborX, neighborY, neighborZ) || 
		(isNaturalIntern(neighborX, neighborY, neighborZ) && isHigherThenIntern(i_x, i_y, i_z, i_faceType)))
		{
			float sideXHigh(i_x * SIDE + HALFSIDE);
			float sideXLow(i_x * SIDE - HALFSIDE);
			float sideYHigh(i_y * SIDE + HALFSIDE);
			float sideYLow(i_y * SIDE - HALFSIDE);
			float sideZHigh(i_z * SIDE + HALFSIDE);
			float sideZLow(i_z * SIDE - HALFSIDE);
			std::vector<VertexPosition> vertexPositionList;

			//On vérifie si l'un des deux côtés contient l'isosurface
			int isoSurfaceState;
			if(i_vertexOldStart == i_vertexOldEnd)
				isoSurfaceState = ISOSURFACE_HIGHER;
			else
				isoSurfaceState = faceGotIsosurface(i_x, i_y, i_z, i_faceType);

			switch(isoSurfaceState)
			{
				case ISOSURFACE_EQUAL:
				
					for(int i(i_vertexOldStart), newVertexSize(i_vertexOldEnd); i < newVertexSize; ++i)
					{
						//On ajoute les sommets de la face
						if(isInTheFace(i_x, i_y, i_z, m_vertexList[i], i_faceType))
						{
							vertexPositionList.push_back(m_vertexList[i]);
							//On enleve les doublons
							for(int j(0), vertexPositionListSize(vertexPositionList.size() - 1); j < vertexPositionListSize; ++j)
								if(m_vertexList[i].x == vertexPositionList[j].x && m_vertexList[i].y == vertexPositionList[j].y && m_vertexList[i].z == vertexPositionList[j].z)
								{
									vertexPositionList.pop_back();
									break;
								}
						}
					}

					//On ajoute les coins si l'isosurface dépasse un des côtés
					addTopCorners(i_x, i_y, i_z, vertexPositionList, i_faceType);

					//On mets les sommets dans le bonne ordre
					switch(i_faceType)
					{
						case LEFT_FACE:
							std::sort(vertexPositionList.begin(), vertexPositionList.end(), compZ);
							break;
						case RIGHT_FACE:
							std::sort(vertexPositionList.begin(), vertexPositionList.end(), compZInv);
							break;
						case FRONT_FACE:
							std::sort(vertexPositionList.begin(), vertexPositionList.end(), compX);
							break;
						case BACK_FACE:
							std::sort(vertexPositionList.begin(), vertexPositionList.end(), compXInv);
							break;
						default:
							assert(false);
							break;
					}

					for(int i(0), vertexPositionListSize(vertexPositionList.size()); i < vertexPositionListSize - 1; ++i)
					{
						float u[2] = {findU(i_x, i_y, i_z, vertexPositionList[i], i_faceType), findU(i_x, i_y, i_z, vertexPositionList[i + 1], i_faceType)};

						//0
						vertexPosition.x = vertexPositionList[i].x;
						vertexPosition.y = sideYLow;
						vertexPosition.z = vertexPositionList[i].z;
						vertexPosition.u = u[0];
						vertexPosition.v = 0.0f;
						m_vertexList.push_back(vertexPosition);

						//1
						vertexPosition.x = vertexPositionList[i].x;
						vertexPosition.y = vertexPositionList[i].y;
						vertexPosition.z = vertexPositionList[i].z;
						vertexPosition.u = u[0];
						vertexPosition.v = (vertexPositionList[i].y - sideYLow)/SIDE;
						m_vertexList.push_back(vertexPosition);

						//2
						vertexPosition.x = vertexPositionList[i + 1].x;
						vertexPosition.y = vertexPositionList[i + 1].y;
						vertexPosition.z = vertexPositionList[i + 1].z;
						vertexPosition.u = u[1];
						vertexPosition.v = (vertexPositionList[i + 1].y - sideYLow)/SIDE;
						m_vertexList.push_back(vertexPosition);

						//3
						vertexPosition.x = vertexPositionList[i].x;
						vertexPosition.y = sideYLow;
						vertexPosition.z = vertexPositionList[i].z;
						vertexPosition.u = u[0];
						vertexPosition.v = 0.0f;
						m_vertexList.push_back(vertexPosition);

						//4
						vertexPosition.x = vertexPositionList[i + 1].x;
						vertexPosition.y = vertexPositionList[i + 1].y;
						vertexPosition.z = vertexPositionList[i + 1].z;
						vertexPosition.u = u[1];
						vertexPosition.v = (vertexPositionList[i + 1].y - sideYLow)/SIDE;
						m_vertexList.push_back(vertexPosition);

						//5
						vertexPosition.x = vertexPositionList[i + 1].x;
						vertexPosition.y = sideYLow;
						vertexPosition.z = vertexPositionList[i + 1].z;
						vertexPosition.u = u[1];
						vertexPosition.v = 0.0f;
						m_vertexList.push_back(vertexPosition);

					}
					break;
				case ISOSURFACE_HIGHER:
					addCompleteFace(i_x, i_y, i_z, m_vertexList, i_faceType);
					break;

				default:
					break;
			}
		}

		//for(int i(i_vertexOldEnd); i < m_vertexList.size(); ++i)
		//{
		//	if(!(m_vertexList[i].u <= 1 && m_vertexList[i].u >= 0 && m_vertexList[i].v <= 1 && m_vertexList[i].v >= 0 ))
		//		assert(false);
		//}

		//for(int i(0); i < m_vertexList.size(); ++i)
		//{
		//	m_vertexList[i].u = 0;
		//	m_vertexList[i].v = 0;
		//}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void addVerticalPolygon(int i_x, int i_y, int i_z, int i_vertexOldStart, int i_vertexOldEnd, std::vector<VertexUV>& i_UVList, FaceType i_faceType)
///
/// Fonction retournant la hauteur relative de l'isosurface d'un sommet en particulier
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_vertexIndex : sommet à vérifier [CUBE_HIGH_0F, CUBE_HIGH_3F]
///
/// @return Hauteur relative de l'isosurface 0 à 1 à l'intérieur du cube
///
////////////////////////////////////////////////////////////////////////
float Chunk::getIsoSurfaceRelativeHeight(int i_x, int i_y, int i_z, int i_vertexIndex)const
{	 
	assert(CUBE_HIGH_0F <= i_vertexIndex && i_vertexIndex <= CUBE_HIGH_3F);
	float bottom(getFloatFromMap(i_x, i_y, i_z, i_vertexIndex) + ISOSURFACE_ERROR_CORRECTION);
	float top(getFloatFromMap(i_x, i_y, i_z, i_vertexIndex + 4) - ISOSURFACE_ERROR_CORRECTION);
	return -bottom/abs(top - bottom);

}

void Chunk::setIsoSurfaceRelativeHeight(int i_x, int i_y, int i_z, int i_vertexIndex, float i_isoPosition)
{	 
	assert(CUBE_HIGH_0F <= i_vertexIndex && i_vertexIndex <= CUBE_HIGH_3F);

	setSingleDensity(i_x, i_y, i_z, i_vertexIndex, (-SIDE * i_isoPosition) - ISOSURFACE_ERROR_CORRECTION);
	setSingleDensity(i_x, i_y, i_z, i_vertexIndex + 4, (-SIDE * i_isoPosition) + SIDE + ISOSURFACE_ERROR_CORRECTION);
}

void Chunk::setRenforcement(int i_x, int i_y, int i_z, int i_renforcement)
{
	if(m_map[i_x][i_y][i_z][CUBE_RENFORCEMENT] < i_renforcement)
		m_map[i_x][i_y][i_z][CUBE_HITPOINT] = 1;
	if(m_map[i_x][i_y][i_z][CUBE_RENFORCEMENT] > i_renforcement)
		m_map[i_x][i_y][i_z][CUBE_HITPOINT] = m_map[i_x][i_y][i_z][CUBE_MAX_HITPOINT];

	m_map[i_x][i_y][i_z][CUBE_RENFORCEMENT] = i_renforcement;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void addBottomPolygon(int i_x, int i_y, int i_z, int i_vertexOldStart, int i_vertexOldEnd, std::vector<VertexUV>& i_UVList)
///
/// Fonction créant les polygones sous la surface
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_vertexOldStart : index du début des sommets relié au cube
/// @param[in] i_vertexOldEnd : fin de l'index des sommets relié au cube
/// @param[in] i_UVList : liste des coordonnées de texture
///
/// @return Hauteur relative de l'isosurface
///
////////////////////////////////////////////////////////////////////////
void Chunk::addBottomPolygon(int i_x, int i_y, int i_z, int i_vertexOldStart, int i_vertexOldEnd)
{
	Terrain* p_terrain(Terrain::getInstance());
	int realX(i_x + m_cubePosition.x);
	int realY(i_y + m_cubePosition.y);
	int realZ(i_z + m_cubePosition.z);

	if(!mk_isTerrainChunk || p_terrain->isInLimit(realX, realY - 1, realZ))
		if(!isNaturalIntern(i_x, i_y - 1, i_z))
		{
			//Face pleine
			VertexPosition vertexPosition;
			vertexPosition.color = generateCubeColor(i_x, i_y, i_z);

			//0
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 0.0f;
			m_vertexList.push_back(vertexPosition);

			//1
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 0.0f;
			m_vertexList.push_back(vertexPosition);

			//2
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 1.0f;
			m_vertexList.push_back(vertexPosition);

			//3
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE - HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 0.0f;
			m_vertexList.push_back(vertexPosition);

			//4
			vertexPosition.x = i_x * SIDE + HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 1.0f;
			vertexPosition.v = 1.0f;
			m_vertexList.push_back(vertexPosition);


			//5
			vertexPosition.x = i_x * SIDE - HALFSIDE;
			vertexPosition.y = i_y * SIDE - HALFSIDE;
			vertexPosition.z = i_z * SIDE + HALFSIDE;
			vertexPosition.u = 0.0f;
			vertexPosition.v = 1.0f;
			m_vertexList.push_back(vertexPosition);
		}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result)
///
/// Fonction retournant les vertex d'un cube
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_result : sommets du cube
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result)
{
	int oldSize(i_result.size());
	i_result.resize(oldSize + m_map[i_x][i_y][i_z][CUBE_NB_VERTEX]);

	for(int i(0), maxIndex(m_map[i_x][i_y][i_z][CUBE_NB_VERTEX]); i < maxIndex; ++i)
	{
		i_result[i + oldSize] = &m_vertexList[m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX] + i];
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result)
///
/// Fonction retournant les vertex d'un cube dans l'ancienne liste de sommet
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_result : sommets du cube
/// @param[in] i_oldVertexList : ancienne liste de sommet
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result, std::vector<VertexPosition>& i_oldVertexList)
{
	int oldSize(i_result.size());
	i_result.resize(oldSize + m_map[i_x][i_y][i_z][CUBE_NB_VERTEX]);

	for(int i(0), maxIndex(m_map[i_x][i_y][i_z][CUBE_NB_VERTEX]); i < maxIndex; ++i)
	{
		i_result[i + oldSize] = &i_oldVertexList[m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX] + i];
	}
}
float Chunk::getDamage(int i_x, int i_y, int i_z)
{
		float damage((getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT) - getFloatFromMap(i_x, i_y, i_z, CUBE_HITPOINT))/getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT));
	if(getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT) == 0.f)
		damage = 0.f;
	return damage;
}
////////////////////////////////////////////////////////////////////////
///
/// @fn void updateDamage(int i_x, int i_y, int i_z)
///
/// Fonction mettant à jour les dommages des cubes sur la carte graphique
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::updateDamage(int i_x, int i_y, int i_z)
{
	float damage(getDamage(i_x, i_y, i_z));

	for(int i(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX]), nbVertex(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX] + m_map[i_x][i_y][i_z][CUBE_NB_VERTEX]); i < nbVertex; ++i)
		m_damageList[i] = damage;

	for(int i(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP]), nbVertex(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP] + m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP]); i < nbVertex; ++i)
		m_damageList[i] = damage;

	//Création du VBO
	//On définie le buffer comme étant le buffer courant
	glBindBuffer(GL_ARRAY_BUFFER, m_damageID);
	//On défini le type de donnée
	glBufferData(GL_ARRAY_BUFFER, m_damageList.size() * sizeof(GLfloat), &m_damageList[0], GL_DYNAMIC_DRAW);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void setChangedFlag(int i_x, int i_y, int i_z, bool i_value)
///
/// Fonction permettant de modifier le flag de modification du cube
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_value : nouvelle valeur du flag
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Chunk::setChangedFlag(int i_x, int i_y, int i_z, bool i_value)
{
	m_map[i_x][i_y][i_z][CUBE_CHANGED] = i_value;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn int switchCubeType(int i_x, int i_y, int i_z, int i_newType)
///
/// Fonction permettant de modifier temporairement le type d'un cube
///
/// @param[in] i_x : position du voisin en x
/// @param[in] i_x : position du voisin en y
/// @param[in] i_z : position du voisin en z
/// @param[in] i_newType : nouveau type du cube
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
int Chunk::switchCubeType(int i_x, int i_y, int i_z, int i_newType)
{
	int oldType(m_map[i_x][i_y][i_z][CUBE_TYPE]);
	m_map[i_x][i_y][i_z][CUBE_TYPE] = i_newType;
	return oldType;
}

bool Chunk::isFull(int i_x, int i_y, int i_z)const
{
	return  getIsoSurfaceRelativeHeight(i_x, i_y, i_z, CUBE_HIGH_0F) >= 1 && 
			getIsoSurfaceRelativeHeight(i_x, i_y, i_z, CUBE_HIGH_1F) >= 1 && 
			getIsoSurfaceRelativeHeight(i_x, i_y, i_z, CUBE_HIGH_2F) >= 1 && 
			getIsoSurfaceRelativeHeight(i_x, i_y, i_z, CUBE_HIGH_3F) >= 1;
}

bool Chunk::isBottomFilled(int i_x, int i_y, int i_z)const
{
	return  !sameSign(getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_0F), getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_4F)) &&
			!sameSign(getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_1F), getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_5F)) &&
			!sameSign(getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_2F), getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_6F)) &&
			!sameSign(getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_3F), getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_7F));
}

btRigidBody*	Chunk::localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		shape->calculateLocalInertia(mass,localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass,myMotionState,shape,localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
	btRigidBody* body = new btRigidBody(mass,0,shape,localInertia);	
	body->setWorldTransform(startTransform);
#endif

	return body;
}

void Chunk::initPhysics()
{
	//Si on voit aucun triangle, il ne peut pas y avoir de collisions
	if (m_indexSize == 0)
	{
		return;
	}

	/*m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);

	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	m_broadphase = new btAxisSweep3(worldMin,worldMax);
	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
	m_dynamicsWorld->getSolverInfo().m_splitImpulse=true;*/
	//m_dynamicsWorld->setDebugDrawer(&sDebugDraw);

	#define TRISIZE 10.f

	int vertStride = sizeof(btVector3);
	int indexStride = 3*sizeof(int);
	int totalVerts(m_vertexList.size());
	gVertices = new btVector3[totalVerts];

	std::vector<Vector> transformedVertexList;
	getTransformedVertexList(transformedVertexList);
	for(int i(0); i < m_vertexList.size(); ++i)
		gVertices[i] = btVector3(transformedVertexList[i].x, transformedVertexList[i].y, transformedVertexList[i].z);


	m_indexVertexArrays = new btTriangleIndexVertexArray(m_indexSize/3,
		(int*)&m_indexList[0],
		indexStride,
		totalVerts,(btScalar*) &gVertices[0].x(),vertStride);

	bool useQuantizedAabbCompression = true;	

	trimeshShape  = new btBvhTriangleMeshShape(m_indexVertexArrays,useQuantizedAabbCompression);
	m_collisionShapes.push_back(trimeshShape);

	btCollisionShape* groundShape = trimeshShape;
	
	float mass = 0.f;
	btTransform	startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(0,0,0));

	/*btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
	m_collisionShapes.push_back(colShape);

	{
		for (int i=0;i<10;i++)
		{
			//btCollisionShape* colShape = new btCapsuleShape(0.5,2.0);//boxShape = new btSphereShape(1.f);
			startTransform.setOrigin(btVector3(2*i,10,1));
			localCreateRigidBody(1, startTransform,colShape);
		}
	}*/

	startTransform.setIdentity();
	m_staticBody = localCreateRigidBody(mass, startTransform,groundShape);	
	m_staticBody->setCollisionFlags(m_staticBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);	
}


void Chunk::exitPhysics()
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

	if (m_indexVertexArrays)
	{
		delete m_indexVertexArrays;
		m_indexVertexArrays = 0;
	}
}

bool Chunk::isAirIntern(int i_x, int i_y, int i_z)const
{
	if(mk_isTerrainChunk)
	{
		int n_realX = m_cubePosition.x + i_x;
		int n_realY = m_cubePosition.y + i_y;
		int n_realZ = m_cubePosition.z + i_z;
		return Terrain::getInstance()->isAir(n_realX, n_realY, n_realZ);
	}
	else
	{
		if(0 > i_x || i_x >= m_sizeX || 0 > i_y || i_y >= m_sizeY || 0 > i_z || i_z >= m_sizeZ)
			return true;
		return m_map[i_x][i_y][i_z][CUBE_TYPE] == 0;
	}
}

bool Chunk::isArtificialIntern(int i_x, int i_y, int i_z)const
{
	if(mk_isTerrainChunk)
	{
		int n_realX = m_cubePosition.x + i_x;
		int n_realY = m_cubePosition.y + i_y;
		int n_realZ = m_cubePosition.z + i_z;
		return Terrain::getInstance()->isArtificial(n_realX, n_realY, n_realZ);
	}
	else
	{
		if(0 > i_x || i_x >= m_sizeX || 0 > i_y || i_y >= m_sizeY || 0 > i_z || i_z >= m_sizeZ)
			return false;
		return m_map[i_x][i_y][i_z][CUBE_TYPE] < 0;
	}
}


bool Chunk::isNaturalIntern(int i_x, int i_y, int i_z)const
{
	if(mk_isTerrainChunk)
	{
		int n_realX = m_cubePosition.x + i_x;
		int n_realY = m_cubePosition.y + i_y;
		int n_realZ = m_cubePosition.z + i_z;
		return Terrain::getInstance()->isNatural(n_realX, n_realY, n_realZ);
	}
	else
	{
		if(0 > i_x || i_x >= m_sizeX || 0 > i_y || i_y >= m_sizeY || 0 > i_z || i_z >= m_sizeZ)
			return false;
		return m_map[i_x][i_y][i_z][CUBE_TYPE] > 0;
	}
}

bool Chunk::isHigherThenIntern(int i_x, int i_y, int i_z,  FaceType i_faceType)const
{
	if(mk_isTerrainChunk)
	{
		int n_realX = m_cubePosition.x + i_x;
		int n_realY = m_cubePosition.y + i_y;
		int n_realZ = m_cubePosition.z + i_z;
		return Terrain::getInstance()->isHigherThen(n_realX, n_realY, n_realZ, i_faceType);
	}
	else
	{
		if(0 > i_x || i_x >= m_sizeX || 0 > i_y || i_y >= m_sizeY || 0 > i_z || i_z >= m_sizeZ)
			return false;
		return isHigherThen(i_x, i_y, i_z, i_faceType);
	}
}

bool Chunk::isTopRenderableIntern(int i_x, int i_y, int i_z)const
{
	if(mk_isTerrainChunk)
	{
		int n_realX = m_cubePosition.x + i_x;
		int n_realY = m_cubePosition.y + i_y;
		int n_realZ = m_cubePosition.z + i_z;
		return Terrain::getInstance()->isSmoothTopRenderable(n_realX, n_realY, n_realZ);
	}
	else
	{
		if(0 > i_x || i_x >= m_sizeX || 0 > i_y || i_y >= m_sizeY || 0 > i_z || i_z >= m_sizeZ)
			return false;
		return isSmoothTopRenderable(i_x, i_y, i_z);
	}
}
bool Chunk::isFullIntern(int i_x, int i_y, int i_z)const
{
	if(mk_isTerrainChunk)
	{
		int n_realX = m_cubePosition.x + i_x;
		int n_realY = m_cubePosition.y + i_y;
		int n_realZ = m_cubePosition.z + i_z;
		return Terrain::getInstance()->isFull(n_realX, n_realY, n_realZ);
	}
	else
	{
		if(0 > i_x || i_x >= m_sizeX || 0 > i_y || i_y >= m_sizeY || 0 > i_z || i_z >= m_sizeZ)
			return false;
		return isFull(i_x, i_y, i_z);
	}
}

bool Chunk::isBottomFilledIntern(int i_x, int i_y, int i_z)const
{
	if(mk_isTerrainChunk)
	{
		int n_realX = m_cubePosition.x + i_x;
		int n_realY = m_cubePosition.y + i_y;
		int n_realZ = m_cubePosition.z + i_z;
		return Terrain::getInstance()->isBottomFilled(n_realX, n_realY, n_realZ);
	}
	else
	{
		if(0 > i_x || i_x >= m_sizeX || 0 > i_y || i_y >= m_sizeY || 0 > i_z || i_z >= m_sizeZ)
			return false;
		return isBottomFilled(i_x, i_y, i_z);
	}
}

void Chunk::getVertexIntern(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result)
{
	if(mk_isTerrainChunk)
	{
		int n_realX = m_cubePosition.x + i_x;
		int n_realY = m_cubePosition.y + i_y;
		int n_realZ = m_cubePosition.z + i_z;
		Terrain::getInstance()->getVertex(n_realX, n_realY, n_realZ, i_result);
	}
	else
	{
		getVertex(i_x, i_y, i_z, i_result);
	}
}
void Chunk::getVertexIntern(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result, std::vector<VertexPosition>& i_oldVertexList)
{
	if(mk_isTerrainChunk)
	{
		int n_realX = m_cubePosition.x + i_x;
		int n_realY = m_cubePosition.y + i_y;
		int n_realZ = m_cubePosition.z + i_z;
		Terrain::getInstance()->getVertex(n_realX, n_realY, n_realZ, i_result, i_oldVertexList);
	}
	else
	{
		getVertex(i_x, i_y, i_z, i_result, i_oldVertexList);
	}
}

void Chunk::copyTerrain()
{
	Terrain* p_terrain(Terrain::getInstance());
	for(int x(0); x < m_sizeX; ++x)
		for(int y(0); y < m_sizeY; ++y)
			for(int z(0); z < m_sizeZ; ++z)
			{
				const std::vector<int>* cubeToCopy(p_terrain->getFullCube(x + m_chunkPosition.x, y + m_chunkPosition.y, z + m_chunkPosition.z));
				if(cubeToCopy == NULL)
					createEmptyCube(m_map[x][y][z]);
				else
					m_map[x][y][z] = *cubeToCopy;
			}
}


const std::vector<int>* Chunk::getFullCube(int i_x, int i_y, int i_z)const
{
	return &m_map[i_x][i_y][i_z];
}


void Chunk::transformModelview(Matrix& i_modelview, const Camera* ipk_camera)const
{
	i_modelview.translate(m_cubePosition.x * SIDE, m_cubePosition.y * SIDE, m_cubePosition.z * SIDE);
}
void Chunk::getTransformedVertexList(std::vector<Vector>& o_transformedVertexList)
{
	Matrix transformation;
	transformation.loadIdentity();
	transformation.translate(m_cubePosition.x * SIDE, m_cubePosition.y * SIDE, m_cubePosition.z * SIDE);
	o_transformedVertexList.resize(m_vertexList.size());
	for(int i(0), sizeVertexList(o_transformedVertexList.size()); i < sizeVertexList; ++i)
	{
		o_transformedVertexList[i] = transformation * Vector(m_vertexList[i].x, m_vertexList[i].y, m_vertexList[i].z);
	}
}

int Chunk::generateCubeColor(int i_x, int i_y, int i_z)
{
	if(mk_isTerrainChunk)
		return generateColor(255, 255, 255, 4);
	else
		return generateColor(0, 255, 0, 4);
}

void Chunk::setFullCube(int i_x, int i_y, int i_z)
{
	setFloatToMap(i_x, i_y, i_z, CUBE_HIGH_0F, -SIDE - ISOSURFACE_ERROR_CORRECTION); 
	setFloatToMap(i_x, i_y, i_z, CUBE_HIGH_1F, -SIDE - ISOSURFACE_ERROR_CORRECTION);
	setFloatToMap(i_x, i_y, i_z, CUBE_HIGH_2F, -SIDE - ISOSURFACE_ERROR_CORRECTION);
	setFloatToMap(i_x, i_y, i_z, CUBE_HIGH_3F, -SIDE - ISOSURFACE_ERROR_CORRECTION);
	setFloatToMap(i_x, i_y, i_z, CUBE_HIGH_4F, ISOSURFACE_ERROR_CORRECTION);
	setFloatToMap(i_x, i_y, i_z, CUBE_HIGH_5F, ISOSURFACE_ERROR_CORRECTION);
	setFloatToMap(i_x, i_y, i_z, CUBE_HIGH_6F, ISOSURFACE_ERROR_CORRECTION);
	setFloatToMap(i_x, i_y, i_z, CUBE_HIGH_7F, ISOSURFACE_ERROR_CORRECTION);
}
bool Chunk::isHigherThen(int i_x, int i_y, int i_z, FaceType i_faceType)const
{
	int realX(i_x + m_cubePosition.x);
	int realY(i_y + m_cubePosition.y);
	int realZ(i_z + m_cubePosition.z);
	int realNeighborX(realX);
	int realNeighborY(realY);
	int realNeighborZ(realZ);

	setNeighbor(realNeighborX, realNeighborY, realNeighborZ, i_faceType);
	Terrain* p_terrain(Terrain::getInstance());
	if(!p_terrain->isInLimit(realNeighborX, realNeighborY, realNeighborZ))
		return false;

	if(!p_terrain->isSmoothTopRenderable(realNeighborX, realNeighborY, realNeighborZ) || p_terrain->isFull(realNeighborX, realNeighborY, realNeighborZ))
		return false;

	float test;
	switch(i_faceType)
	{
		case LEFT_FACE:
			if(p_terrain->getIsoPosition(realX, realY, realZ, CUBE_HIGH_0F) > p_terrain->getIsoPosition(realNeighborX, realNeighborY, realNeighborZ, CUBE_HIGH_1F) ||
			   p_terrain->getIsoPosition(realX, realY, realZ, CUBE_HIGH_3F) > p_terrain->getIsoPosition(realNeighborX, realNeighborY, realNeighborZ, CUBE_HIGH_2F))
				return true;
			else
				return false;
			break;
		case RIGHT_FACE:
			if(p_terrain->getIsoPosition(realX, realY, realZ, CUBE_HIGH_1F) > p_terrain->getIsoPosition(realNeighborX, realNeighborY, realNeighborZ, CUBE_HIGH_0F) ||
			   p_terrain->getIsoPosition(realX, realY, realZ, CUBE_HIGH_2F) > p_terrain->getIsoPosition(realNeighborX, realNeighborY, realNeighborZ, CUBE_HIGH_3F))
				return true;
			else
				return false;
			break;
		case FRONT_FACE:
			if(p_terrain->getIsoPosition(realX, realY, realZ, CUBE_HIGH_3F) > p_terrain->getIsoPosition(realNeighborX, realNeighborY, realNeighborZ, CUBE_HIGH_0F) ||
			   p_terrain->getIsoPosition(realX, realY, realZ, CUBE_HIGH_2F) > p_terrain->getIsoPosition(realNeighborX, realNeighborY, realNeighborZ, CUBE_HIGH_1F))
				return true;
			else
				return false;
			break;
		case BACK_FACE:
			if(p_terrain->getIsoPosition(realX, realY, realZ, CUBE_HIGH_0F) > p_terrain->getIsoPosition(realNeighborX, realNeighborY, realNeighborZ, CUBE_HIGH_3F) ||
			   p_terrain->getIsoPosition(realX, realY, realZ, CUBE_HIGH_1F) > p_terrain->getIsoPosition(realNeighborX, realNeighborY, realNeighborZ, CUBE_HIGH_2F))
				return true;
			else
				return false;
			break;
		default:
			assert(false);
			break;
	}
	return false;
}

void Chunk::addWhitePrismeDrawable()
{
	if(!mk_isTerrainChunk)
	{	
		TemplateManager::getInstance()->getWhitePrismeInstanciable()->addToRenderInfoList(m_outlineRenderInformation);
	}
}

void Chunk::removeWhitePrismeDrawable()
{
	if(!mk_isTerrainChunk)
	{	
		TemplateManager::getInstance()->getWhitePrismeInstanciable()->removeFromRenderInfoList(m_outlineRenderInformation);
	}
}

void Chunk::getDensity(int i_x, int i_y, int i_z, float i_density[8])const
{
	for(int i(0); i < 8; ++i)
		i_density[i] = getFloatFromMap(i_x, i_y, i_z, CUBE_HIGH_0F + i);
}

void Chunk::fill(int i_cubeType, float i_hitpoint)
{
	for(int x(0); x < m_sizeX; ++x)
		for(int y(0); y < m_sizeY; ++y)
			for(int z(0); z < m_sizeZ; ++z)
			{
				m_map[x][y][z][CUBE_TYPE] = i_cubeType;
				setFloatToMap(x, y, z, CUBE_MAX_HITPOINT, i_hitpoint);
				setFloatToMap(x, y, z, CUBE_HITPOINT, i_hitpoint);
				m_map[x][y][z][CUBE_COLOR] = generateColor(255, 255, 255, 4);
			}
}

Chunk::Chunk(const Chunk& i_chunk)
:m_sizeX(i_chunk.m_sizeX),
m_sizeY(i_chunk.m_sizeY),
m_sizeZ(i_chunk.m_sizeZ),
m_chunkPosition(i_chunk.m_chunkPosition),
m_cubePosition(i_chunk.m_cubePosition),
mk_isTerrainChunk(i_chunk.mk_isTerrainChunk),
gVertices(0),
trimeshShape(0),
m_staticBody(0),
m_indexVertexArrays(0),
m_defaultContactProcessingThreshold(BT_LARGE_FLOAT),
m_whiteTerrainLine(this), 
Drawable(GEOM_PASS_SHADER),
m_outlineRenderInformation(this)
{
	resizeChunk();
	for(int i(0); i < m_sizeX; ++i)
		for(int j(0); j < m_sizeY; ++j)
			for(int k(0); k < m_sizeZ; ++k)
				for(int l(0); l < NB_CUBE_PARAM - NB_UNSAVABLE_CUBE_ATTRIB; ++l)
					m_map[i][j][k][l] = i_chunk.m_map[i][j][k][l];
	init();
}

float Chunk::getCubeHitpoint(int i_x, int i_y, int i_z)
{
	return getFloatFromMap(i_x, i_y, i_z, CUBE_HITPOINT);
}

float Chunk::getCubeMaxHitpoint(int i_x, int i_y, int i_z)const
{
	return getFloatFromMap(i_x, i_y, i_z, CUBE_MAX_HITPOINT);
}

void addTopvertex(std::set<VertexPosition>& i_topVertex, VertexPosition i_vertexPosition)
{
	i_vertexPosition.normal[0] = 0.f;
	i_vertexPosition.normal[1] = 1.f;
	i_vertexPosition.normal[2] = 0.f;

	i_topVertex.insert(i_vertexPosition);
}

void Chunk::generateExtraTop(int i_x, int i_y, int i_z, std::vector<GLint>& i_textureNbList, std::vector<GLint>& i_renforcementList)
{
	int realX(m_cubePosition.x + i_x);
	int realY(m_cubePosition.y + i_y);
	int realZ(m_cubePosition.z + i_z);

	//return;
	assert(!Terrain::getInstance()->isAir(m_cubePosition.x + i_x, m_cubePosition.y + i_y, m_cubePosition.z + i_z));

	if(!Terrain::getInstance()->isFlatTopRenderable(m_cubePosition.x + i_x, m_cubePosition.y + i_y, m_cubePosition.z + i_z))
	{
		m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP] = -1;
		m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP] = 0;
		return;
	}
	std::set<VertexPosition> topVertex;
	float topYPosition(i_y * SIDE + HALFSIDE);
	//On trouve les points déjà sur le cube
	for(int i(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX]), iEnd(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX] + m_map[i_x][i_y][i_z][CUBE_NB_VERTEX]); i < iEnd; ++i)
	{
		if(topYPosition == m_vertexList[i].y)
			addTopvertex(topVertex, m_vertexList[i]);
	}
	//On ajoute les coins manquant pour fermer la face
	VertexPosition vertexPosition;
	vertexPosition.y = (i_y) * SIDE + HALFSIDE;

	//  Z
	// 1_0
	// |_| X
	// 2 3

	if(getIsoSurfaceRelativeHeight(i_x, i_y, i_z, CUBE_HIGH_0F) > 1.f)
	{
		vertexPosition.x = (i_x) * SIDE + HALFSIDE;
		vertexPosition.z = (i_z) * SIDE + HALFSIDE;
		vertexPosition.u = (vertexPosition.x - ((i_x + m_cubePosition.x) * SIDE - HALFSIDE))/SIDE;
		vertexPosition.v = (vertexPosition.z - ((i_z + m_cubePosition.z) * SIDE - HALFSIDE))/SIDE;
		addTopvertex(topVertex, vertexPosition);
	}
		
	if(getIsoSurfaceRelativeHeight(i_x, i_y, i_z, CUBE_HIGH_1F) > 1.f)
	{
		vertexPosition.x = (i_x) * SIDE - HALFSIDE;
		vertexPosition.z = (i_z) * SIDE + HALFSIDE;
		vertexPosition.u = (vertexPosition.x - ((i_x + m_cubePosition.x) * SIDE - HALFSIDE))/SIDE;
		vertexPosition.v = (vertexPosition.z - ((i_z + m_cubePosition.z) * SIDE - HALFSIDE))/SIDE;
		addTopvertex(topVertex, vertexPosition);
	}

	if(getIsoSurfaceRelativeHeight(i_x, i_y, i_z, CUBE_HIGH_2F) > 1.f)
	{
		vertexPosition.x = (i_x) * SIDE - HALFSIDE;
		vertexPosition.z = (i_z) * SIDE - HALFSIDE;
		vertexPosition.u = (vertexPosition.x - ((i_x + m_cubePosition.x) * SIDE - HALFSIDE))/SIDE;
		vertexPosition.v = (vertexPosition.z - ((i_z + m_cubePosition.z) * SIDE - HALFSIDE))/SIDE;
		addTopvertex(topVertex, vertexPosition);
	}
		
	if(getIsoSurfaceRelativeHeight(i_x, i_y, i_z, CUBE_HIGH_3F) > 1.f)
	{
		vertexPosition.x = (i_x) * SIDE + HALFSIDE;
		vertexPosition.z = (i_z) * SIDE - HALFSIDE;
		vertexPosition.u = (vertexPosition.x - ((i_x + m_cubePosition.x) * SIDE - HALFSIDE))/SIDE;
		vertexPosition.v = (vertexPosition.z - ((i_z + m_cubePosition.z) * SIDE - HALFSIDE))/SIDE;
		addTopvertex(topVertex, vertexPosition);
	}
		


	//On modifie les caractéristiques des sommets
	if(topVertex.size() < 3)
	{
		m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP] = -1;
		m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP] = 0;
		return;
	}

	//On ajoute le tout à la liste des vertex et on enregistre la position dans le buffer
	m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP] = m_vertexList.size();
	for(std::set<VertexPosition>::iterator it(topVertex.begin()), topVertexEnd(topVertex.end()); it != topVertexEnd; ++it)
		m_vertexList.push_back(*it);
	m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP] = m_vertexList.size() - m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP];

	if(m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP] == 0)
	{
		m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP] = -1;
		m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP] = 0;
		return;
	}

	//On ajoute l'index
	generateExtraTopIndex(i_x, i_y, i_z);
	//On ajoute les dommages

	int vertexStart(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP]);
	for(int i(vertexStart + 1), vertexEnd(m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP] + m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP] + 1); i < vertexEnd; ++i)
		m_damageList.push_back(getDamage(i_x, i_y, i_z));
	//On ajoute le numéro de texture
	for(int i(vertexStart + 1), vertexEnd(m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP] + m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP] + 1); i < vertexEnd; ++i)
		i_textureNbList.push_back(m_map[i_x][i_y][i_z][CUBE_TYPE]);

	for(int i(vertexStart + 1), vertexEnd(m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP] + m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP] + 1); i < vertexEnd; ++i)
		i_renforcementList.push_back(m_map[i_x][i_y][i_z][CUBE_RENFORCEMENT]);
}

void Chunk::generateExtraTopIndex(int i_x, int i_y, int i_z)
{
	if(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP] == -1 || m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP] == 0)
		return;
	std::multimap<float, int> angleIndex;
	float angle;
	
	std::pair<float, int> indexToAdd;

	Vector center(0.f, 0.f, 0.f);

	for(int i(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP]), vertexEnd(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP] + m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP]); i < vertexEnd; ++i)
	{
		center.x += m_vertexList[i].x;
		center.y += m_vertexList[i].y;
		center.z += m_vertexList[i].z;
	}
	center /= m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP];


	std::vector<VertexPosition> test;

	for(int i(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP]), vertexEnd(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP] + m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP]); i < vertexEnd; ++i)
	{
		test.push_back(m_vertexList[i]);
		angle = (Vector(m_vertexList[i].x, m_vertexList[i].y, m_vertexList[i].z) - center).getPhi();
		indexToAdd.first = -angle;
		indexToAdd.second = i;
		angleIndex.insert(indexToAdd);
	}

	for(std::multimap<float, int>::iterator it(++angleIndex.begin()), itEnd(--angleIndex.end()); it != itEnd;)
	{
		m_indexList.push_back(angleIndex.begin()->second);
		m_indexList.push_back(it->second);
		m_indexList.push_back((++it)->second);
	}
	//m_indexList.pop_back();
	//m_indexList.pop_back();
	//m_indexList.pop_back();

	//int vertexStart(m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP]);
	//for(int i(vertexStart + 1), vertexEnd(m_map[i_x][i_y][i_z][CUBE_NB_EXTRA_TOP] + m_map[i_x][i_y][i_z][CUBE_DRAWING_INDEX_EXTRA_TOP]); i < vertexEnd - 1; ++i)
	//{
	//	m_indexList.push_back(vertexStart);
	//	m_indexList.push_back(i);
	//	m_indexList.push_back(i + 1);
	//}
}