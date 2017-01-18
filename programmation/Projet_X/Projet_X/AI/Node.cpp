#include "Node.h"

#include "../Terrain/Terrain.h"
#include "../Math/Vector.h"
#include "../Position.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn Node()
///
/// Constructeur par défaut
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Node::Node()
{
	//On mets des valeurs par défaut qui sont invalide
	m_position = Vector3I(-1, -1, -1);
	m_target = Vector3I(-1, -1, -1);
	m_cost = 0;
	m_heuristic = 0;
	mp_parent = 0;
	for(int i(0); i < 5; ++i)
		ma_isChanging[i] = false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Node(const Vector3I& i_position, const Vector3I& i_target)
///
/// Constructeur
/// 
/// @param[in] ik_position : position du noeud
/// @param[in] ik_target : position du but
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Node::Node(const Vector3I& i_position, const Vector3I& i_target)
{
	m_position = i_position;
	m_target = i_target;
	m_cost = 0;
	m_heuristic = (i_position - i_target).length();
	mp_parent = 0;

	for(int i(0); i < 5; ++i)
		ma_isChanging[i] = false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Node(const Vector3I& ik_position, const Vector3I& ik_target, float i_cost, 
///			bool i_isChangingUnderFloor, bool i_isChangingFloor, bool i_isChangingBottom, bool i_isChangingTop, bool i_isChangingOverTop, 
///			Node* ip_parent)
///
/// Constructeur
/// 
/// @param[in] ik_position : position du noeud
/// @param[in] ik_target : position du but
/// @param[in] i_cost : le coût accumuler jusqu'à présent
/// @param[in] i_isChangingUnderFloor : si on doit changer le cube sous le plancher
/// @param[in] i_isChangingFloor : si on doit changer le cube au niveau du plancher
/// @param[in] i_isChangingBottom : si on doit changer le cube au niveau des pieds
/// @param[in] i_isChangingTop : si on doit changer le cube au niveau de la tête
/// @param[in] i_isChangingOverTop : si on doit changer le noeud au dessus du niveau de la tête
/// @param[in] ip_parent : parent du noeud
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Node::Node(const Vector3I& ik_position, const Vector3I& ik_target, float i_cost, 
			bool i_isChangingUnderFloor, bool i_isChangingFloor, bool i_isChangingBottom, bool i_isChangingTop, bool i_isChangingOverTop, 
			Node* ip_parent)
{
	ma_isChanging[0] = i_isChangingUnderFloor;
	ma_isChanging[1] = i_isChangingFloor;
	ma_isChanging[2] = i_isChangingBottom;
	ma_isChanging[3] = i_isChangingTop;
	ma_isChanging[4] = i_isChangingOverTop;

	m_position = ik_position;
	m_target = ik_target;

	//Le coût augmente avec le nombre de cube à détruire ou à créer pour atteindre le noeud
	m_cost = i_cost + 
		CREATING_COST * ma_isChanging[0] + 
		CREATING_COST * ma_isChanging[1] + 
		DESTROYING_COST * ma_isChanging[2] + 
		DESTROYING_COST * ma_isChanging[3] +
		DESTROYING_COST * ma_isChanging[4];

	//On utilise un heuristique à vole d'oiseau
	m_heuristic = (ik_position - ik_target).length();
	mp_parent = ip_parent;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Node(const Vector3I& ik_position, const Vector3I& ik_target, float i_cost, Node* ip_parent)
///
/// Constructeur
/// 
/// @param[in] ik_position : position du noeud
/// @param[in] ik_target : position du but
/// @param[in] i_cost : le coût accumuler jusqu'à présent
/// @param[in] ip_parent : parent du noeud
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Node::Node(const Vector3I& ik_position, const Vector3I& ik_target, float i_cost, Node* ip_parent)
:m_position(ik_position), m_target(ik_target), m_cost(i_cost), mp_parent(ip_parent), m_heuristic((ik_position.y - ik_target.y))
{
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Node(const Node& i_node)
///
/// Constructeur par copie
/// 
/// @param[in] i_node : noeud à copier
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Node::Node(const Node& i_node)
{
	m_position = i_node.m_position;
	m_target = i_node.m_target;
	m_cost = i_node.m_cost;
	m_heuristic = i_node.m_heuristic;
	mp_parent = i_node.mp_parent;

	for(int i(0); i < 5; ++i)
		ma_isChanging[i] = i_node.ma_isChanging[i];
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Node()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Node::~Node()
{
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool operator==(const Node& i_node)const
///
/// Operateur d'égaliter
/// 
/// @param[in] i_node : noeud à comparer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Node::operator==(const Node& i_node)const
{
	return m_position.x == i_node.m_position.x && 
		   m_position.y == i_node.m_position.y && 
		   m_position.z == i_node.m_position.z;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool operator>(const Node& i_node) const
///
/// Operateur plus quand que
/// 
/// @param[in] i_node : noeud à comparer
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool Node::operator>(const Node& i_node) const
{
	return getEvaluation() > i_node.getEvaluation();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Position generateHash()const
///
/// Fonction permettant de générer un hash du noeud
///
/// @return hash généré
///
////////////////////////////////////////////////////////////////////////
Position Node::generateHash()const
{
	Position result;
	result.x = m_position.x;
	result.y = m_position.y;
	result.z = m_position.z;
	return result;
}

Vector Node::getPosition()const
{
	return Terrain::cubeToPosition(m_position);
}

Vector Node::getTarget()const
{
	return Terrain::cubeToPosition(m_target);
}

Vector3I Node::getCubePosition()const
{
	return m_position;
}

Vector3I Node::getCubeTarget()const
{
	return m_target;
}