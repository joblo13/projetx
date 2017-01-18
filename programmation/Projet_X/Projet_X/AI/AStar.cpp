#include "AStar.h"

#include "../Math/Vector.h"
#include "../Position.h"
#include "Node.h"
#include "../Terrain/Terrain.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn AStar()
///
/// Constructeur par défaut
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
AStar::AStar()
:m_initNode(0), m_endNode(0), m_isDone(false), m_aStarType(WALKING_TYPE), m_isInit(false)
{

}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~AStar()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
AStar::~AStar()
{
	delete m_initNode;
	delete m_endNode;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void init(const Vector& i_initVector, const Vector& i_endVector)
///
/// Constructeur
/// 
/// @param[in] ix_initVector : position de départ
/// @param[in] ix_endVector : position visée
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void AStar::init(const Vector3I& i_initVector, const Vector3I& i_endVector, AStarType i_aStarType)
{
	m_isInit = true;
	m_aStarType = i_aStarType;
	m_initNode = new Node(i_initVector, i_endVector);
	m_endNode = new Node(i_endVector, i_endVector);
	clear();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void clear()
///
/// Fonction permettant de remettre le A* à neuf
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void AStar::clear()
{
	m_isDone = false;

	std::hash_map<Position, Node*> ax_exploredNodes;
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> ax_emptyOpenChilds;
	std::hash_set<Position> astr_emptyOpenChildsHash;

	ma_exploredNodes.swap(ax_exploredNodes);
	ma_openChilds.swap(ax_emptyOpenChilds);
	ma_openChildsHash.swap(astr_emptyOpenChildsHash);
	m_resultNode = new Node();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Node executeSearch()
///
/// Effectue en partie la recherche a*
///
/// @return le noeud final ou un noeud vide si l'execution n'est pas terminé
///
////////////////////////////////////////////////////////////////////////
Node* AStar::executeSearch()
{
	//On initialise le compteur pour limiter la quantité de traitement par frame
	int n_nodeCount(100);

	//Durant la première itération, on cherche les enfants du noeud initial
	if(ma_exploredNodes.empty())
	{
		ma_exploredNodes[m_initNode->generateHash()] = m_initNode;
		if(appendOpenChild(findNodeChilds(m_initNode)))
		{
			//Si on se rends ici, on a trouvé le meilleur chemin
			m_isDone = true;
			return m_resultNode;
		}

	}

	//On initialise un pointeur vers la valeur actuelle du meileur noeud
	Node* x_bestNode;

	while(ma_openChilds.size() != 0)
	{
		//On vérifie si l'on a fait le nombre d'itération demandé pour garder un bon fps
		if(n_nodeCount > 0)
			--n_nodeCount;
		else
			return new Node();

		//On selectionne le noeud le moins couteux
		x_bestNode = new Node(ma_openChilds.top());
		removeOpenChild();

		//On ajoute le meilleur noeud aux noeuds visités
		ma_exploredNodes[x_bestNode->generateHash()] = x_bestNode;

		//On ajoute les enfants du noeuds ouverts, on sort de la boucle si on trouve le bon noueud
		if(appendOpenChild(findNodeChilds(x_bestNode)))
		{
			//Si on se rends ici, on a trouvé le meilleur chemin
			m_isDone = true;
			return m_resultNode;
		}
	}
	m_isDone = true;
	return new Node();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool appendOpenChild(const std::vector<Node>& i_sourceVect)
///
/// Fonction permettant d'ajouter a openchild les noeuds d'un vecteur
/// 
/// @param[in] i_sourceVect : vecteur a ajouter
///
/// @return Vrai si on a réussi la concatenation
///
////////////////////////////////////////////////////////////////////////
bool AStar::appendOpenChild(const std::vector<Node>& i_sourceVect)
{
	Position hash;
	//Pour chaques éléments envoyés
	for(int i(0), n_initNodeChildSize(i_sourceVect.size()); i < n_initNodeChildSize; ++i)
	{
		//On génère le hash
		hash = i_sourceVect[i].generateHash();
		//On vérifie s'il existe déjà dans la liste des noeuds à visiter ou ceux visité
		if(ma_openChildsHash.insert(hash).second && ma_exploredNodes.find(hash) == ma_exploredNodes.end())
		{
			//S'il n'existe pas on l'ajoute dans la liste
			ma_openChilds.push(i_sourceVect[i]);
			//On vérifie si le noeud est le noeud finale
			if(nodeFound(i_sourceVect[i]))
			{
				m_resultNode = new Node(i_sourceVect[i]);
				return true;
			}
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool removeOpenChild()
///
/// Fonction permettant d'enlever un enfant pouvant être visité
///
/// @return Vrai si l'enfant a pu être enlevé
///
////////////////////////////////////////////////////////////////////////
bool AStar::removeOpenChild()
{
	if(ma_openChilds.size() != 0)
	{
		ma_openChilds.pop();
		return true;
	}
	return false;
}

Vector AStar::getTargetPosition()
{
	return m_endNode->getPosition();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void developChild(int i_x, int i_y, int i_z)
///
/// Fonction permettant de developper 
/// 
/// @param[in] i_x : position relative du cube en x
/// @param[in] i_y : position relative du cube en y
/// @param[in] i_z : position relative du cube en z
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void AStar::developChildWalking(Node* ip_node,  int i_x, int i_y, int i_z, std::vector<Node>& i_nodeList)
{
	Terrain* p_terrain(Terrain::getInstance());
	Vector3I cubePosition(ip_node->getCubePosition());

	//Si le cube est hors limite, on ne le developpe pas
	if(!p_terrain->isInLimit(cubePosition.x + i_x, cubePosition.y + i_y - 1, cubePosition.z + i_z))
		return;

	bool isWalkable = false;
	//On génère la forme du plancher cible 
	float a_modifierTarget[9];
	//p_terrain->generateModifierEmptyTop(cubePosition.x + i_x, cubePosition.y + i_y - 1, cubePosition.z + i_z, a_modifierTarget);

	bool isClimbing = false;
	bool isDigging = false;

	//On vérifie si le déplacement en montant est légale
	/*if( i_y == 1 ||
		(i_x == 1 && i_z == 0 && a_modifierTarget[1] <= 1 && a_modifierTarget[8] <= 1 && a_modifierTarget[7] <= 1) ||
		(i_x == 0 && i_z == 1 && a_modifierTarget[1] <= 1 && a_modifierTarget[2] <= 1 && a_modifierTarget[3] <= 1) ||
		(i_x == -1 && i_z == 0 && a_modifierTarget[3] <= 1 && a_modifierTarget[4] <= 1 && a_modifierTarget[5] <= 1) ||
		(i_x == 0 && i_z == -1 && a_modifierTarget[5] <= 1 && a_modifierTarget[6] <= 1 && a_modifierTarget[7] <= 1))
	{*/
		isClimbing = true;		
	//}
	//On vérifie si le deplacement en dessendant est légale
	if(i_y == -1 || 
		(i_x == 1 && i_z == 0) ||
		(i_x == 0 && i_z == 1) ||
		(i_x == -1 && i_z == 0) ||
		(i_x == 0 && i_z == -1))
	{
		isDigging = true;
	}

	//On vérifie si le déplacement peut être effectué
	if(i_y == 0 || isClimbing || isDigging)
	{
		//On développe le noeud si il est possible de le faire
		i_nodeList.push_back(Node(Vector3I(cubePosition.x + i_x, cubePosition.y + i_y, cubePosition.z + i_z),
							ip_node->getCubeTarget(), 
							ip_node->getCost() + (cubePosition - Vector3I(cubePosition.x + i_x, cubePosition.y + i_y, cubePosition.z + i_z)).length(), 
							!p_terrain->isNaturalOrOutside(cubePosition.x + i_x, cubePosition.y + i_y - 2, cubePosition.z + i_z) && isClimbing,
							!p_terrain->isNaturalOrOutside(cubePosition.x + i_x, cubePosition.y + i_y - 1, cubePosition.z + i_z),
							p_terrain->isNatural(cubePosition.x + i_x, cubePosition.y + i_y, cubePosition.z + i_z),
							p_terrain->isNatural(cubePosition.x + i_x, cubePosition.y + i_y + 1, cubePosition.z + i_z),
							p_terrain->isNatural(cubePosition.x + i_x, cubePosition.y + i_y + 2, cubePosition.z + i_z) && isDigging,
							ip_node));
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void developChildInner(int i_x, int i_y, int i_z)
///
/// Fonction permettant de developper 
/// 
/// @param[in] i_x : position relative du cube en x
/// @param[in] i_y : position relative du cube en y
/// @param[in] i_z : position relative du cube en z
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void AStar::developChildInner(Node* ip_node,  int i_x, int i_y, int i_z, std::vector<Node>& i_nodeList)
{
	Terrain* p_terrain(Terrain::getInstance());
	Vector3I cubePosition(ip_node->getCubePosition());

	//Si le cube est hors limite, on ne le developpe pas
	if(!p_terrain->isInLimit(cubePosition.x + i_x, cubePosition.y + i_y, cubePosition.z + i_z))
		return;

	if(p_terrain->isAir(cubePosition.x + i_x, cubePosition.y + i_y, cubePosition.z + i_z))
		return;

	//On développe le noeud s'il est possible de le faire
	i_nodeList.push_back(Node(Vector3I(cubePosition.x + i_x, cubePosition.y + i_y, cubePosition.z + i_z),
						ip_node->getCubeTarget(), 
						ip_node->getCost() + (cubePosition - Vector3I(cubePosition.x + i_x, cubePosition.y + i_y, cubePosition.z + i_z)).length(), 
						ip_node));
}

bool AStar::nodeFound(const Node& i_node)
{
	if(m_aStarType == WALKING_TYPE)
		return i_node == *m_endNode;
	else if(m_aStarType = INNER_TYPE)
	{
		return m_endNode->getCubePosition().y == i_node.getCubePosition().y;
	}

	return true;
}

std::vector<Node> AStar::findNodeChilds(Node* ip_node)
{
	std::vector<Node> result;

	if(m_aStarType == WALKING_TYPE)
		findNodeChildsWalking(ip_node, result);
	else if(m_aStarType = INNER_TYPE)
		findNodeChildsInner(ip_node, result);

	return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn std::vector<Node> findChilds()
///
/// Fonction trouvant les enfants du noeud
///
/// @return Noeuds enfants trouvés
///
////////////////////////////////////////////////////////////////////////
void AStar::findNodeChildsWalking(Node* ip_node, std::vector<Node>& i_nodeList)
{
	Terrain* p_terrain(Terrain::getInstance());
	Vector3I cubePosition(ip_node->getCubePosition());

	//Colonne 1
	developChildWalking(ip_node, 1, 1, 0, i_nodeList);
	developChildWalking(ip_node, 1, 0, 0, i_nodeList);
	developChildWalking(ip_node, 1, -1, 0, i_nodeList);

	//Colonne 2
	//Il est possible d'aller dans cette colonne seulement si les deux côtés sont libre
	if(!p_terrain->isNatural(cubePosition.x + 1, cubePosition.y + 1, cubePosition.z) && 
		!p_terrain->isNatural(cubePosition.x, cubePosition.y + 1, cubePosition.z + 1))
	{
		developChildWalking(ip_node, 1, 1, 1, i_nodeList);
		developChildWalking(ip_node, 1, 0, 1, i_nodeList);
		developChildWalking(ip_node, 1, -1, 1, i_nodeList);
	}

	//Colonne 3
	developChildWalking(ip_node, 0, 1, 1, i_nodeList);
	developChildWalking(ip_node, 0, 0, 1, i_nodeList);
	developChildWalking(ip_node, 0, -1, 1, i_nodeList);

	//Colonne 4
	//Il est possible d'aller dans cette colonne seulement si les deux côtés sont libre
	if(!p_terrain->isNatural(cubePosition.x - 1, cubePosition.y + 1, cubePosition.z) && 
		!p_terrain->isNatural(cubePosition.x, cubePosition.y + 1, cubePosition.z + 1))
	{
		developChildWalking(ip_node, -1, 1, 1, i_nodeList);
		developChildWalking(ip_node, -1, 0, 1, i_nodeList);
		developChildWalking(ip_node, -1, -1, 1, i_nodeList);
	}

	//Colonne 5
	developChildWalking(ip_node, -1, 1, 0, i_nodeList);
	developChildWalking(ip_node, -1, 0, 0, i_nodeList);
	developChildWalking(ip_node, -1, -1, 0, i_nodeList);

	//Colonne 6
	//Il est possible d'aller dans cette colonne seulement si les deux côtés sont libre
	if(!p_terrain->isNatural(cubePosition.x - 1, cubePosition.y + 1, cubePosition.z) && 
		!p_terrain->isNatural(cubePosition.x, cubePosition.y + 1, cubePosition.z - 1))
	{
		developChildWalking(ip_node, -1, 1, -1, i_nodeList);
		developChildWalking(ip_node, -1, 0, -1, i_nodeList);
		developChildWalking(ip_node, -1, -1, -1, i_nodeList);
	}

	//Colonne 7
	developChildWalking(ip_node, 0, 1, -1, i_nodeList);
	developChildWalking(ip_node, 0, 0, -1, i_nodeList);
	developChildWalking(ip_node, 0, -1, -1, i_nodeList);

	//Colonne 8
	//Il est possible d'aller dans cette colonne seulement si les deux côtés sont libre
	if(!p_terrain->isNatural(cubePosition.x + 1, cubePosition.y + 1, cubePosition.z) && 
		!p_terrain->isNatural(cubePosition.x, cubePosition.y + 1, cubePosition.z - 1))
	{
		developChildWalking(ip_node, 1, 1, -1, i_nodeList);
		developChildWalking(ip_node, 1, 0, -1, i_nodeList);
		developChildWalking(ip_node, 1, -1, -1, i_nodeList);
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn std::vector<Node> findChilds()
///
/// Fonction trouvant les enfants du noeud
///
/// @return Noeuds enfants trouvés
///
////////////////////////////////////////////////////////////////////////
 void AStar::findNodeChildsInner(Node* ip_node, std::vector<Node>& i_nodeList)
{
	Terrain* p_terrain(Terrain::getInstance());
	Vector3I cubePosition(ip_node->getCubePosition());


	developChildInner(ip_node, 1, 0, 0, i_nodeList);
	developChildInner(ip_node, -1, 0, 0, i_nodeList);

	developChildInner(ip_node, 0, 1, 0, i_nodeList);
	developChildInner(ip_node, 0, -1, 0, i_nodeList);

	developChildInner(ip_node, 0, 0, 1, i_nodeList);
	developChildInner(ip_node, 0, 0, -1, i_nodeList);
}