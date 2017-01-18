#include "Vector4f.h"

#include "Vector.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector4f()
///
/// Constructeur par défaut
/// 
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Vector4f::Vector4f()
{
    x = 0;
    y = 0;
    z = 0;
	w = 0;
}

Vector4f::Vector4f(const Vector& i_vector, float i_w)
{
	x = i_vector.x;
	y = i_vector.y;
	z = i_vector.z;
	w = i_w;
}

Vector4f& Vector4f::operator/=(const float ik_number)
{
    x /= ik_number;
    y /= ik_number;
    z /= ik_number;
	w /= ik_number;
    return *this;
}