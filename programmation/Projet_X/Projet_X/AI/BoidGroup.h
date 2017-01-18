#ifndef BOID_GROUP_H
#define BOID_GROUP_H

#include <vector>
#include <SDL/SDL.h>

#include "../Math/Vector.h"

class Unit;
class Vector3I;

//Classe permettant d'effectuer un A*
class BoidGroup
{
	public:
		//Constructeur
		BoidGroup(int i_groupID);
		~BoidGroup();
		void update();
		void addBoid(Unit* ip_unit);
		bool removeBoid(Unit* ip_unit);
		const Vector& getCenter()const{return m_center;}
		void setGroupID(int i_groupID);

		int getListSize(){return m_boidList.size();}
		void applyBoidBehaviors(Uint32 i_timestep);

		void findPath(const std::vector<Vector3I>& i_endPosition);
	private:
		std::vector<Unit*> m_boidList;
		Vector m_center;
		int m_groupID;

};


#endif //BOID_GROUP_H
