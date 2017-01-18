#include "Vector3I.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I()
///
/// Constructeur par d�faut
/// 
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Vector3I::Vector3I()
:m_xDirection(true),
m_yDirection(true),
m_zDirection(true),
x(0),
y(0),
z(0)
{

}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I(int i_x, int i_y, int i_z)
///
/// Constructeur par param�tre
/// 
/// @param[in] i_x: composante x
/// @param[in] i_y: composante y
/// @param[in] i_z: composante z
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Vector3I::Vector3I(int i_x, int i_y, int i_z)
	:m_xDirection(true),
	m_yDirection(true),
	m_zDirection(true),
	x(i_x),
	y(i_y),
	z(i_z)
{

}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I(const Vector3I & ik_vector)
///
/// Constructeur par copie
/// 
/// @param[in] ik_vector: vecteur � copier
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Vector3I::Vector3I(const Vector3I & ik_vector)
	:m_xDirection(ik_vector.m_xDirection),
	m_yDirection(ik_vector.m_yDirection),
	m_zDirection(ik_vector.m_zDirection),
	x(ik_vector.x),
	y(ik_vector.y),
	z(ik_vector.z)
{

}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I(const Vector3I& ik_from, const Vector3I& ik_to)
///
/// Constructeur par deux points
/// 
/// @param[in] ik_from: point de d�part
/// @param[in] ik_to: point d'arriv� d�part
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Vector3I::Vector3I(const Vector3I& ik_from, const Vector3I& ik_to)
	:m_xDirection(true),
	m_yDirection(true),
	m_zDirection(true),
	x(ik_to.x - ik_from.x),
	y(ik_to.y - ik_from.y),
	z(ik_to.z - ik_from.z)
{

}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I& operator=(const Vector3I& ik_vector)
///
/// Op�rateur d'assignation
/// 
/// @param[in] ik_vector: vecteur � assigner
///
/// @return Le vecteur courant qui vient d'�tre assign�
///
////////////////////////////////////////////////////////////////////////
Vector3I& Vector3I::operator=(const Vector3I& ik_vector)
{
    x = ik_vector.x;
    y = ik_vector.y;
    z = ik_vector.z;
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I& operator+=(const Vector3I& ik_vector)
///
/// Op�rateur d'assignation additif
/// 
/// @param[in] ik_vector: vecteur � additionner au vecteur courant
///
/// @return Le vecteur courant qui vient d'�tre modifi�
///
////////////////////////////////////////////////////////////////////////
Vector3I& Vector3I::operator+=(const Vector3I& ik_vector)
{
    x += ik_vector.x;
    y += ik_vector.y;
    z += ik_vector.z;
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I operator+(const Vector3I& ik_vector)
///
/// Op�rateur d'addition
/// 
/// @param[in] ik_vector: vecteur � additionner au vecteur courant
///
/// @return Le vecteur r�sultant de l'addition des deux vecteurs
///
////////////////////////////////////////////////////////////////////////
Vector3I Vector3I::operator+(const Vector3I & ik_vector) const
{
    Vector3I result = *this;
    result += ik_vector;
    return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I& operator-=(const Vector3I& ik_vector)
///
/// Op�rateur d'assignation soustractif
/// 
/// @param[in] ik_vector: vecteur � soustraire au vecteur courant
///
/// @return Le vecteur courant qui vient d'�tre modifi�
///
////////////////////////////////////////////////////////////////////////
Vector3I& Vector3I::operator-=(const Vector3I& ik_vector)
{
    x -= ik_vector.x;
    y -= ik_vector.y;
    z -= ik_vector.z;
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I operator-(const Vector3I& ik_vector) const
///
/// Op�rateur de soustraction
/// 
/// @param[in] ik_vector: vecteur � soustraire au vecteur courant
///
/// @return Le vecteur r�sultant de la soustraction des deux vecteurs
///
////////////////////////////////////////////////////////////////////////
Vector3I Vector3I::operator-(const Vector3I& ik_vector) const
{
    Vector3I result = *this;
    result -= ik_vector;
    return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I& operator*=(const int ik_number)
///
/// Op�rateur d'assignation multiplicatif
/// 
/// @param[in] ik_number: nombre � multiplier
///
/// @return Le vecteur courant qui vient d'�tre modifi�
///
////////////////////////////////////////////////////////////////////////
Vector3I& Vector3I::operator*=(const int ik_number)
{
    x *= ik_number;
    y *= ik_number;
    z *= ik_number;
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I operator*(const int ik_number)const
///
/// Op�rateur de multiplication avec un nombre scalaire
/// 
/// @param[in] ik_number: Nombre � multiplier
///
/// @return Le vecteur r�sultant de la multiplication du vecteur et du scalaire
///
////////////////////////////////////////////////////////////////////////
Vector3I Vector3I::operator*(const int ik_number)const
{
    Vector3I result = *this;
    result *= ik_number;
    return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I operator*(const int ikf_number, const Vector3I& ikx_vector)
///
/// Op�rateur de multiplication avec un nombre scalaire et un vecteur (de l'autre sens)
/// 
/// @param[in] ikf_number: Nombre � multiplier
/// @param[in] ikx_vector: vecteur � multiplier
///
/// @return Le vecteur r�sultant de la multiplication du vecteur et du scalaire
///
////////////////////////////////////////////////////////////////////////
Vector3I operator*(const int ik_number, const Vector3I& ik_vector)
{
    return Vector3I(ik_vector.x * ik_number, ik_vector.y * ik_number, ik_vector.z * ik_number);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn int operator*(const Vector3I& ikx_vector)const
///
/// Op�rateur du produit scalaire
/// 
/// @param[in] ikx_vector: vecteur � multiplier
///
/// @return Scalaire produit par le produit scalaire
///
////////////////////////////////////////////////////////////////////////
int Vector3I::operator*(const Vector3I& ik_vector)const
{
    return x * ik_vector.x + y * ik_vector.y + z * ik_vector.z;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I& Vector3I::operator/= (const float ikf_number)
///
/// Op�rateur d'assignation divisif
/// 
/// @param[in] ikf_number: diviseur scalaire
///
/// @return Vector3I courant modifi� par l'op�ration
///
////////////////////////////////////////////////////////////////////////
Vector3I& Vector3I::operator/= (const float ik_number)
{
    x = (int)(x/ik_number);
    y = (int)(y/ik_number);
    z = (int)(z/ik_number);
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I operator/(const float ikf_number)const
///
/// Op�rateur de division
/// 
/// @param[in] ikf_number: diviseur scalaire
///
/// @return Le vecteur r�sultant de la division entre le vecteur et le scalaire
///
////////////////////////////////////////////////////////////////////////
Vector3I Vector3I::operator/(const float ik_number)const
{
    Vector3I result = *this;
    result /= ik_number;
    return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I crossProduct(const Vector3I& ikx_vector)const
///
/// Fonction permettant d'effectuer la multiplication vectorielle
/// 
/// @param[in] ikx_vector: vecteur � multiplier
///
/// @return Le vecteur r�sultant de la multiplication vectorielle entre les deux vecteurs
///
////////////////////////////////////////////////////////////////////////
Vector3I Vector3I::crossProduct(const Vector3I& ik_vector)const
{
    Vector3I result;
    result.x = y * ik_vector.z - z * ik_vector.y;
    result.y = z * ik_vector.x - x * ik_vector.z;
    result.z = x * ik_vector.y - y * ik_vector.x;
    return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector3I& normalize()
///
/// Fonction normalizant le vecteur
///
/// @return Vecteur normaliz�
///
////////////////////////////////////////////////////////////////////////
Vector3I& Vector3I::normalize()
{
    (*this) /= length();
    return (*this);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool operator<(const Vector3I& i_position)const
///
/// Operateur de comparaison pour class� dans une hash_map
///
/// @return resultat de la comparaison
///
////////////////////////////////////////////////////////////////////////
bool Vector3I::operator<(const Vector3I& i_position)const
{

	if(x == i_position.x)
	{
		if(y == i_position.y)
		{
			if(m_zDirection)
				return z < i_position.z;
			else
				return z > i_position.z;
		}
		else
		{
			if(m_yDirection)
				return y < i_position.y;
			else
				return y > i_position.y;
		}
	}
	else
	{
		if(m_xDirection)
			return x < i_position.x;
		else
			return x > i_position.x;
	}
}

////////////////////////////////////////////////////////////////////////
///
/// @fn operator size_t()const 
///
/// Fonction retournant la grandeur pour class� dans un hash_map
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Vector3I::operator size_t()const 
{ 
	return (size_t)(x * 100) ^ (size_t)((short)(y * 100)<<2) ^ (size_t)((short)(z * 100)<<4); 
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void setDirection(bool i_xDirection, bool i_yDirection, bool i_zDirection)
///
/// Fonction permettant de detrminer le sens du vecteur pour optimiser l'ordre dans la hash_map
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Vector3I::setDirection(bool i_xDirection, bool i_yDirection, bool i_zDirection)
{
	m_xDirection = i_xDirection; 
	m_yDirection = i_yDirection; 
	m_zDirection = i_zDirection;
}