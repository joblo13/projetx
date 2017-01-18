#ifndef VECTOR_3I_H
#define VECTOR_3I_H

#include <cmath>
#include "../Definition.h"

class Vector3I
{
public:
    Vector3I();
    Vector3I(int i_x, int i_y, int i_z);
    Vector3I(const Vector3I& ik_vector);
    Vector3I(const Vector3I& ik_from, const Vector3I& ik_to);

	//Opérateur
    Vector3I& operator=(const Vector3I& ik_vector);

    Vector3I& operator+=(const Vector3I& ik_vector);
    Vector3I operator+(const Vector3I& ik_vector) const;

    Vector3I& operator-=(const Vector3I& ik_vector);
    Vector3I operator-(const Vector3I& ik_vector) const;

    Vector3I& operator*=(const int ik_number);
    Vector3I operator*(const int ik_number)const;
	friend Vector3I operator*(const int ik_number, const Vector3I& ik_vector);

	//Produit scalaire
	int operator*(const Vector3I& ik_vector)const;
    

    Vector3I& operator/=(const float ik_number);
    Vector3I operator/(const float ik_number)const;

	bool operator<(const Vector3I& i_position)const;
	operator size_t()const;

	//Produit vectorielle
    Vector3I crossProduct(const Vector3I& ik_vector)const;

	Vector3I& normalize();

	//Accesseur
    float getPhi()const{return atan((float)(z/x)) * 180.0f/MATH_PI;}
    float getTheta()const{return atan(y/sqrt((float)(x*x +z *z))) * 180.0f/MATH_PI;}
	float length()const{return sqrt((float)(x * x + y * y + z * z));}

	//Attributs
	int x;
    int y;
    int z;

	void setDirection(	bool i_xDirection, bool i_yDirection, bool i_zDirection);

private:
	bool m_xDirection;
	bool m_yDirection;
	bool m_zDirection;
};

#endif //VECTOR_3I_H
