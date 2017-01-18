#ifndef A_STAR_H
#define A_STAR_H

#include <hash_map>
#include <queue>
#include <hash_set>
#include <limits>

class Node;
class Vector;
class Vector3I;
struct Position;

enum AStarType{WALKING_TYPE, INNER_TYPE};

//Classe permettant d'effectuer un A*
class AStar
{
	public:
		//Constructeur
		AStar();
		~AStar();

		//Initialisation d'un A*
		void init(const Vector3I& i_initVector, const Vector3I& i_endVector, AStarType i_aStarType);
		Node* executeSearch();

		//Accesseur
		bool getIsDone(){return m_isDone;}
		Vector getTargetPosition();

		bool getIsInit(){return m_isInit;}

		void setIsInit(bool i_isInit){m_isInit = i_isInit;}

	private:
		//Fonctions outils
		bool appendOpenChild(const std::vector<Node>& i_sourceVect);
		bool removeOpenChild();
		void clear();

		bool nodeFound(const Node& i_node);
		std::vector<Node> findNodeChilds(Node* ip_node);

		void findNodeChildsWalking(Node* ip_node, std::vector<Node>& i_nodeList);
		void developChildWalking(Node* ip_node,  int i_x, int i_y, int i_z, std::vector<Node>& i_nodeList);

		void findNodeChildsInner(Node* ip_node, std::vector<Node>& i_nodeList);
		void developChildInner(Node* ip_node,  int i_x, int i_y, int i_z, std::vector<Node>& i_nodeList);

		//List des noeuds explorés
		std::hash_map<Position, Node*> ma_exploredNodes;
		//List des noeuds à explorer avec le plus petit comme prochain noeud
		std::priority_queue<Node, std::vector<Node>, std::greater<Node>> ma_openChilds;
		//List des noeuds pouvant être exploré
		std::hash_set<Position> ma_openChildsHash;

		Node* m_initNode;
		Node* m_endNode;
		Node* m_resultNode;

		bool m_isDone;
		bool m_isInit;

		AStarType m_aStarType;
};


#endif //A_STAR_H
