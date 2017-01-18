#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include "../Definition.h"

class Vector
{
public:
    Vector();
    Vector(float i_x, float i_y, float i_z);
    Vector(const Vector& ik_vector);
    Vector(const Vector& ik_from, const Vector& ik_to);

	//Opérateur
    Vector& operator=(const Vector& ik_vector);

    Vector& operator+=(const Vector& ik_vector);
    Vector operator+(const Vector& ik_vector) const;

    Vector& operator-=(const Vector& ik_vector);
    Vector operator-(const Vector& ik_vector) const;
	Vector operator-() const;

    Vector& operator*=(const float ik_number);
    Vector operator*(const float ik_number)const;
	friend Vector operator*(const float ik_number, const Vector& ik_vector);

	//Produit scalaire
	float operator*(const Vector& ik_vector)const;
    

    Vector& operator/=(const float ik_number);
    Vector operator/(const float ik_number)const;

	//Produit vectorielle
    Vector crossProduct(const Vector& ik_vector)const;

	Vector& normalize();

	//Accesseur
    float getPhi()const
	{
		if(x == 0.f)
			if(z < 0.f)
				return -90.f;
			else
				return 90.f;
		return (float)atan(z/x) * 180.0f/MATH_PI + ((x > 0)?0.f:180.f);
	}
    float getTheta()const{return (float)atan(y/sqrt(x*x +z *z)) * 180.0f/MATH_PI;}
	float length()const{return (float)sqrt( x * x + y * y + z * z);}

	//Attributs
	float x;
    float y;
    float z;
};

#endif //VECTOR_H
