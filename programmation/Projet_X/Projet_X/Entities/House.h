#ifndef HOUSE_H
#define HOUSE_H

#include "../Math/Vector3I.h"

class House
{
public:
	House(const Vector3I& ik_position);
	~House();

	//Modifier
	void House::setOccupied(bool ib_isOccupied){m_isOccupied = ib_isOccupied;}
	//Accesseur
	bool House::getOccupied() const{return m_isOccupied;}
	int House::getSize() const{return 8;}
	Vector3I House::getPosition()const{return m_position;}

private:
	//Cube
	Vector3I m_position;
	bool m_isOccupied;
};

#endif //HOUSE_H