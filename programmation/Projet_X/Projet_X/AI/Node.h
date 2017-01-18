#ifndef NODE_H
#define NODE_H

#define CREATING_COST 300
#define DESTROYING_COST 300

#include <vector>

#include "../Math/Vector3I.h"

class Vector;
struct Position;

class Node
{
	public:
		//Constructeurs
		Node();
		Node(const Vector3I& ik_position, const Vector3I& ik_target);
		Node(const Vector3I& ik_position, const Vector3I& ik_target, float i_cost, bool i_isChangingUnderFloor, 
			bool i_isChangingFloor, bool i_isChangingBottom, bool i_isChangingTop, bool i_isChangingOverTop, Node* ip_parent);
		Node(const Vector3I& ik_position, const Vector3I& ik_target, float i_cost, Node* ip_parent);

		Node(const Node& ik_node);
		~Node();

		//On génère une struct représentant le noeud
		Position generateHash()const;

		//Accesseur
		Node* getParent(){return mp_parent;}

		Vector getPosition()const;
		Vector3I getCubePosition()const;

		Vector getTarget()const;
		Vector3I getCubeTarget()const;

		float getCost()const{return m_cost;}
		bool getIsChanging(int i_index){return ma_isChanging[i_index];}
		float getEvaluation()const{return m_cost + m_heuristic;}

		//Operateur
		bool operator==(const Node& i_node)const;
		bool operator>(const Node& i_node) const;

	protected:
		Node* mp_parent;
		Vector3I m_position;
		Vector3I m_target;

		bool ma_isChanging[5];

		float m_cost;
		float m_heuristic;
};


#endif //NODE_H
