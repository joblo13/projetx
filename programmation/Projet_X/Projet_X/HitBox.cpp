//Include interne
#include "HitBox.h"
#include "Terrain\Terrain.h"
#include <vector>

////////////////////////////////////////////////////////////////////////
///
/// @fn HitBox(float i_width, float i_height,  float i_lenght, Vector ik_center)
///
/// Constructeur
/// 
/// @param[in] i_width : Moitier de la largeur de la hitBox
/// @param[in] i_height : Hauteur de la hitBox
/// @param[in] i_lenght : Moitier de la longueur de la hitBox
/// @param[in] ik_center : Position actuelle de la hitBox
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
HitBox::HitBox(float i_width, float i_height,  float if_lenght, const Vector& ik_center)
{
	ma_dimensions[0] = i_width;
	ma_dimensions[1] = i_height;
	ma_dimensions[2] = if_lenght;
	m_position = ik_center;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn HitBox(HitBox* ip_hitBox, Vector ik_center)
///
/// Constructeur
/// 
/// @param[in] ip_hitBox : hitBox à copier
/// @param[in] ik_center : Nouvelle position de la copie
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
HitBox::HitBox(HitBox* ip_hitBox, const Vector& ik_center)
{
	ma_dimensions[0] = ip_hitBox->getDimension(0);
	ma_dimensions[1] = ip_hitBox->getDimension(1);
	ma_dimensions[2] = ip_hitBox->getDimension(2);
	m_position = ik_center;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn ~HitBox()
///
/// Destructeur
/// 
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
HitBox::~HitBox()
{

}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool collision(HitBox ikx_hitBox)
///
/// Fonction déterminant si une collision survient entre deux hitBox
/// 
/// @param[in] ik_hitBox : hitBox à tester
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
bool HitBox::collision(const HitBox& ik_hitBox)const
{
	//Si une de ces conditions est respecté, il est impossible que les deux hitbox soit en collision
	if((ik_hitBox.getPosition().x - ik_hitBox.getDimension(0) >= m_position.x + ma_dimensions[0])      // trop à droite
	|| (ik_hitBox.getPosition().x + ik_hitBox.getDimension(0) <= m_position.x - ma_dimensions[0]) // trop à gauche
	|| (ik_hitBox.getPosition().y - ik_hitBox.getDimension(1)/2 >= m_position.y + ma_dimensions[1]/2) // trop en bas
	|| (ik_hitBox.getPosition().y + ik_hitBox.getDimension(1)/2 <= m_position.y - ma_dimensions[1]/2)  // trop en haut	
    || (ik_hitBox.getPosition().z - ik_hitBox.getDimension(2) >= m_position.z + ma_dimensions[2])   // trop derrière
	|| (ik_hitBox.getPosition().z + ik_hitBox.getDimension(2) <= m_position.z - ma_dimensions[2]))  // trop devant
          return false; 
   else
          return true; 
}

////////////////////////////////////////////////////////////////////////
///
/// @fn int rayCollisionSide(Vector& i_sourcePosition, Vector& i_targetPosition)
///
/// Fonction qui permet de savoir quel face du cube est touché en permier par un rayon
/// 
/// @param[in] i_sourcePosition : point source
/// @param[in] i_targetPosition : point cible
///
/// @return Position relative où le cube doit être créer
///
////////////////////////////////////////////////////////////////////////
int HitBox::rayCollisionSide(const Vector& i_sourcePosition, const Vector& i_targetPosition)
{
	std::vector<Vector> hitsPositions;
	rayCollisionPosition(i_sourcePosition, i_targetPosition, hitsPositions);

	//Si il y a au moins une collision
	if(hitsPositions.size() > 0)
	{

		//On retourne la position
		
		if(hitsPositions[0].x == m_position.x - ma_dimensions[0])
			return RIGHT;
		if(hitsPositions[0].x == m_position.x + ma_dimensions[0])
			return LEFT;
		if(hitsPositions[0].y == m_position.y - ma_dimensions[1])
			return BOTTOM;
		if(hitsPositions[0].y == m_position.y + ma_dimensions[1])
			return TOP;
		if(hitsPositions[0].z == m_position.z - ma_dimensions[2])
			return FRONT;
		if(hitsPositions[0].z == m_position.z + ma_dimensions[2])
			return BACK;
	}

	return false;
}

void HitBox::rayCollisionPosition(const Vector& i_sourcePosition, const Vector& i_targetPosition, std::vector<Vector>& i_hitsPositions)
{
	Vector hit;
	Vector box1 = Vector(m_position.x - ma_dimensions[0], m_position.y - ma_dimensions[1], m_position.z - ma_dimensions[2]);
	Vector box2 = Vector(m_position.x + ma_dimensions[0], m_position.y + ma_dimensions[1], m_position.z + ma_dimensions[2]);

	//On regarde les cas triviaux
	if (i_targetPosition.x < box1.x && i_sourcePosition.x < box1.x) 
		return;
	if (i_targetPosition.x > box2.x && i_sourcePosition.x > box2.x) 
		return;
	if (i_targetPosition.y < box1.y && i_sourcePosition.y < box1.y) 
		return;
	if (i_targetPosition.y > box2.y && i_sourcePosition.y > box2.y) 
		return;
	if (i_targetPosition.z < box1.z && i_sourcePosition.z < box1.z) 
		return;
	if (i_targetPosition.z > box2.z && i_sourcePosition.z > box2.z) 
		return;

	//On trouve le point de collision
	if (i_sourcePosition.x > box1.x && i_sourcePosition.x < box2.x &&
		i_sourcePosition.y > box1.y && i_sourcePosition.y < box2.y &&
		i_sourcePosition.z > box1.z && i_sourcePosition.z < box2.z) 
	{
		hit = i_sourcePosition; 
		i_hitsPositions.push_back(hit);
	}

	

	//On doit effectuer les 6 getIntersection et choisir le hit le plus pres
	if(getIntersection( i_sourcePosition.x - box1.x, i_targetPosition.x - box1.x, i_sourcePosition, i_targetPosition, hit))
	{
		if(inBox( hit, box1, box2, 1 ))
		{
			i_hitsPositions.push_back(hit);
		}
	}
	if(getIntersection( i_sourcePosition.y - box1.y, i_targetPosition.y - box1.y, i_sourcePosition, i_targetPosition, hit))
	{
		if(inBox( hit, box1, box2, 2 ))
		{
			i_hitsPositions.push_back(hit);
		}
	}
	if(getIntersection( i_sourcePosition.z - box1.z, i_targetPosition.z - box1.z, i_sourcePosition, i_targetPosition, hit))
	{
		if(inBox( hit, box1, box2, 3 ))
		{
			i_hitsPositions.push_back(hit);
		}
	}
	if(getIntersection( i_sourcePosition.x - box2.x, i_targetPosition.x - box2.x, i_sourcePosition, i_targetPosition, hit))
	{
		if(inBox( hit, box1, box2, 1 ))
		{
			i_hitsPositions.push_back(hit);
		}
	}
	if(getIntersection( i_sourcePosition.y - box2.y, i_targetPosition.y - box2.y, i_sourcePosition, i_targetPosition, hit))
	{
		if(inBox( hit, box1, box2, 2 ))
		{
			i_hitsPositions.push_back(hit);
		}
	}
	if(getIntersection( i_sourcePosition.z - box2.z, i_targetPosition.z - box2.z, i_sourcePosition, i_targetPosition, hit))
	{
		if(inBox( hit, box1, box2, 3 ))
		{
			i_hitsPositions.push_back(hit);
		}
	}

	//Si il y a au moins une collision
	std::vector<Vector> hitMin;
	if(i_hitsPositions.size() > 0)
	{
		float minDistance = (i_hitsPositions[0] - i_sourcePosition).length();
		
		hitMin.push_back(i_hitsPositions[0]);

		//On trouve le plus proche point de collision
		for(int i(1), hitsPositionsSize(i_hitsPositions.size()); i < hitsPositionsSize; ++i)
		{
			if(minDistance > (i_hitsPositions[i] - i_sourcePosition).length())
			{
				minDistance = (i_hitsPositions[i] - i_sourcePosition).length();
				hitMin[0] = i_hitsPositions[i];
			}
		}

	}
	i_hitsPositions.swap(hitMin);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool rayCollision(Vector& i_sourcePosition, Vector& i_targetPosition)
///
/// Fonction qui permet de savoir si un rayon entre en collision avec la hitbox
/// 
/// @param[in] i_sourcePosition : point source
/// @param[in] i_targetPosition : point cible
///
/// @return Vrai s'il y a une collision
///
////////////////////////////////////////////////////////////////////////
bool HitBox::rayCollision(const Vector& i_sourcePosition, const Vector& i_targetPosition)
{
	Vector hit;
	Vector box1 = Vector(m_position.x - ma_dimensions[0], m_position.y - ma_dimensions[1], m_position.z - ma_dimensions[2]);
	Vector box2 = Vector(m_position.x + ma_dimensions[0], m_position.y + ma_dimensions[1], m_position.z + ma_dimensions[2]);

	//On regarde les cas triviaux
	if (i_targetPosition.x < box1.x && i_sourcePosition.x < box1.x) 
		return false;
	if (i_targetPosition.x > box2.x && i_sourcePosition.x > box2.x) 
		return false;
	if (i_targetPosition.y < box1.y && i_sourcePosition.y < box1.y) 
		return false;
	if (i_targetPosition.y > box2.y && i_sourcePosition.y > box2.y) 
		return false;
	if (i_targetPosition.z < box1.z && i_sourcePosition.z < box1.z) 
		return false;
	if (i_targetPosition.z > box2.z && i_sourcePosition.z > box2.z) 
		return false;

	if (i_sourcePosition.x > box1.x && i_sourcePosition.x < box2.x &&
		i_sourcePosition.y > box1.y && i_sourcePosition.y < box2.y &&
		i_sourcePosition.z > box1.z && i_sourcePosition.z < box2.z) 
	{
		hit = i_sourcePosition; 
		return true;
	}


	//On doit effectuer les 6 getIntersection et choisir le hit le plus pres
	if((getIntersection( i_sourcePosition.x - box1.x, i_targetPosition.x - box1.x, i_sourcePosition, i_targetPosition, hit) && inBox( hit, box1, box2, 1 ))
	|| (getIntersection( i_sourcePosition.y - box1.y, i_targetPosition.y - box1.y, i_sourcePosition, i_targetPosition, hit) && inBox( hit, box1, box2, 2 )) 
	|| (getIntersection( i_sourcePosition.z - box1.z, i_targetPosition.z - box1.z, i_sourcePosition, i_targetPosition, hit) && inBox( hit, box1, box2, 3 )) 
	|| (getIntersection( i_sourcePosition.x - box2.x, i_targetPosition.x - box2.x, i_sourcePosition, i_targetPosition, hit) && inBox( hit, box1, box2, 1 )) 
	|| (getIntersection( i_sourcePosition.y - box2.y, i_targetPosition.y - box2.y, i_sourcePosition, i_targetPosition, hit) && inBox( hit, box1, box2, 2 )) 
	|| (getIntersection( i_sourcePosition.z - box2.z, i_targetPosition.z - box2.z, i_sourcePosition, i_targetPosition, hit) && inBox( hit, box1, box2, 3 )))
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool getIntersection(float i_distance1, float i_distance2, const Vector& i_point1, const Vector& i_point2, Vector& i_hit) 
///
/// Fonction qui permet de savoir si un rayon entre en collision avec la hitbox
/// 
/// @param[in] i_distance1 : distance du point 1 au point 2
/// @param[in] i_distance2 : distance du point 2 au point 1 
/// @param[in] i_point1 : premier point de la droite
/// @param[in] i_point2 : deuxième point de la droite
/// @param[in] i_hit : coordonnée de l'intersection trouvé
///
/// @return Vrai s'il y a une intersection
///
////////////////////////////////////////////////////////////////////////
bool inline HitBox::getIntersection(float i_distance1, float i_distance2, const Vector& i_point1, const Vector& i_point2, Vector& i_hit) 
{
	//Si les deux distance sont positive, il n'y a pas d'intersection, car il sont du meme côté
	if((i_distance1 * i_distance2) >= 0.0f)
		return false;

	if(i_distance1 == i_distance2) 
		return false; 

	//On calcule l'intesection
	//i_hit = i_point1 + (i_point2 - i_point1) * ( -i_distance1/(i_distance2 - i_distance1) );
	Vector direction(i_point2 - i_point1);
	i_hit = i_point1 + direction.normalize() * direction.length() * (abs(i_distance1) / (abs(i_distance2) + abs(i_distance1)));
	return true;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool inBox(const Vector& hit, const Vector& b1, const Vector& b2, const int Axis) 
///
/// Fonction qui permet de savoir si une droite est dans une boite
/// 
/// @param[in] i_hit : point de collision
/// @param[in] i_box1 : premier point de la boite
/// @param[in] i_box2 : deuxième point de la voite
/// @param[in] i_axis : axe
///
/// @return Vrai si la droite est dans une boite
///
////////////////////////////////////////////////////////////////////////
bool inline HitBox::inBox(const Vector& i_hit, const Vector& i_box1, const Vector& i_box2, const int i_axis) 
{
	//On détermine si les deux point sont a l'intérieur de la boite
	if(i_axis==1 && i_hit.z >= i_box1.z && i_hit.z <= i_box2.z && i_hit.y >= i_box1.y && i_hit.y <= i_box2.y) 
		return true;

	if(i_axis==2 && i_hit.z >= i_box1.z && i_hit.z <= i_box2.z && i_hit.x >= i_box1.x && i_hit.x <= i_box2.x) 
		return true;

	if(i_axis==3 && i_hit.x >= i_box1.x && i_hit.x <= i_box2.x && i_hit.y >= i_box1.y && i_hit.y <= i_box2.y) 
		return true;

	return false;
}