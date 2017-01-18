#ifndef HITBOX_H
#define HITBOX_H

#include "Math/Vector.h"
#include "Math/Vector3I.h"

enum CubeLocation{BOTTOM = 1, TOP, LEFT, RIGHT, FRONT, BACK};
class HitBox
{
	public:
		HitBox(float i_width = 0, float i_height = 0,  float i_lenght = 0, const Vector& ik_center = Vector());
		HitBox(HitBox* ip_hitBox, const Vector& ik_center);
		~HitBox();

		//Détection de la collision
		bool collision(const HitBox& ik_hitBox)const;
		void rayCollisionPosition(const Vector& i_sourcePosition, const Vector& i_targetPosition, std::vector<Vector>& i_hitsPositions);
		int rayCollisionSide(const Vector& i_sourcePosition, const Vector& i_targetPosition);
		bool rayCollision(const Vector& i_sourcePosition, const Vector& i_targetPosition);

		//Modifieur
		void setPosition(const Vector& ik_position){m_position = ik_position;}
		void setDimension(const float i_width, const float i_height, const float i_lenght)
		{
			ma_dimensions[0] = i_width;
			ma_dimensions[1] = i_height;
			ma_dimensions[2] = i_lenght;
		}

		//Accesseur
		const Vector& getPosition()const{return m_position;}
		float getDimension(int i_index)const{return ma_dimensions[i_index];}

	protected:		
		bool inline getIntersection(float i_distance1, float i_distance2, const Vector& ik_point1, const Vector& ik_point2, Vector& i_hit);
		bool inline inBox(const Vector& ik_hit, const Vector& ik_box1, const Vector& ik_box2, const int ik_axis);

		float ma_dimensions[3];
		Vector m_position;
};

#endif //HITBOX_H
