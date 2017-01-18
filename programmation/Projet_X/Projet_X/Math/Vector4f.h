#ifndef VECTOR_4F_H
#define VECTOR_4F_H

#include <cmath>
#include "../Definition.h"

class Vector;

class Vector4f
{
public:
    Vector4f();
	Vector4f(const Vector& i_vector, float i_w);

	Vector4f& Vector4f::operator/=(const float ik_number);

	//Attributs
	float x;
    float y;
    float z;
	float w;
};

#endif //VECTOR_4F_H
