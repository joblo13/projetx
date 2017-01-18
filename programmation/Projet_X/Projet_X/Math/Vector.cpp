#include "Vector.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector()
///
/// Constructeur par d�faut
/// 
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Vector::Vector()
{
    x = 0;
    y = 0;
    z = 0;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector(float i_x, float i_y, float i_z)
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
Vector::Vector(float i_x, float i_y, float i_z)
{
    x = i_x;
    y = i_y;
    z = i_z;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector(const Vector & ik_vector)
///
/// Constructeur par copie
/// 
/// @param[in] ik_vector: vecteur � copier
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Vector::Vector(const Vector & ik_vector)
{
    x = ik_vector.x;
    y = ik_vector.y;
    z = ik_vector.z;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector(const Vector& ik_from, const Vector& ik_to)
///
/// Constructeur par deux points
/// 
/// @param[in] ik_from: point de d�part
/// @param[in] ik_to: point d'arriv� d�part
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Vector::Vector(const Vector& ik_from, const Vector& ik_to)
{
    x = ik_to.x - ik_from.x;
    y = ik_to.y - ik_from.y;
    z = ik_to.z - ik_from.z;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector& operator=(const Vector& ik_vector)
///
/// Op�rateur d'assignation
/// 
/// @param[in] ik_vector: vecteur � assigner
///
/// @return Le vecteur courant qui vient d'�tre assign�
///
////////////////////////////////////////////////////////////////////////
Vector& Vector::operator=(const Vector& ik_vector)
{
    x = ik_vector.x;
    y = ik_vector.y;
    z = ik_vector.z;
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector& operator+=(const Vector& ik_vector)
///
/// Op�rateur d'assignation additif
/// 
/// @param[in] ik_vector: vecteur � additionner au vecteur courant
///
/// @return Le vecteur courant qui vient d'�tre modifi�
///
////////////////////////////////////////////////////////////////////////
Vector& Vector::operator+=(const Vector& ik_vector)
{
    x += ik_vector.x;
    y += ik_vector.y;
    z += ik_vector.z;
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector operator+(const Vector& ik_vector)
///
/// Op�rateur d'addition
/// 
/// @param[in] ik_vector: vecteur � additionner au vecteur courant
///
/// @return Le vecteur r�sultant de l'addition des deux vecteurs
///
////////////////////////////////////////////////////////////////////////
Vector Vector::operator+(const Vector & ik_vector) const
{
    Vector result = *this;
    result += ik_vector;
    return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector& operator-=(const Vector& ik_vector)
///
/// Op�rateur d'assignation soustractif
/// 
/// @param[in] ik_vector: vecteur � soustraire au vecteur courant
///
/// @return Le vecteur courant qui vient d'�tre modifi�
///
////////////////////////////////////////////////////////////////////////
Vector& Vector::operator-=(const Vector& ik_vector)
{
    x -= ik_vector.x;
    y -= ik_vector.y;
    z -= ik_vector.z;
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector operator-(const Vector& ik_vector) const
///
/// Op�rateur de soustraction
/// 
/// @param[in] ik_vector: vecteur � soustraire au vecteur courant
///
/// @return Le vecteur r�sultant de la soustraction des deux vecteurs
///
////////////////////////////////////////////////////////////////////////
Vector Vector::operator-(const Vector& ik_vector) const
{
    Vector result = *this;
    result -= ik_vector;
    return result;
}

Vector Vector::operator-() const
{
	Vector result = *this;
	result.x = -result.x;
	result.y = -result.y;
	result.z = -result.z;
	return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector& operator*=(const float ik_number)
///
/// Op�rateur d'assignation multiplicatif
/// 
/// @param[in] ik_number: nombre � multiplier
///
/// @return Le vecteur courant qui vient d'�tre modifi�
///
////////////////////////////////////////////////////////////////////////
Vector& Vector::operator*=(const float ik_number)
{
    x *= ik_number;
    y *= ik_number;
    z *= ik_number;
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector operator*(const float ik_number)const
///
/// Op�rateur de multiplication avec un nombre scalaire
/// 
/// @param[in] ik_number: Nombre � multiplier
///
/// @return Le vecteur r�sultant de la multiplication du vecteur et du scalaire
///
////////////////////////////////////////////////////////////////////////
Vector Vector::operator*(const float ik_number)const
{
    Vector result = *this;
    result *= ik_number;
    return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector operator*(const float ikf_number, const Vector& ikx_vector)
///
/// Op�rateur de multiplication avec un nombre scalaire et un vecteur (de l'autre sens)
/// 
/// @param[in] ikf_number: Nombre � multiplier
/// @param[in] ikx_vector: vecteur � multiplier
///
/// @return Le vecteur r�sultant de la multiplication du vecteur et du scalaire
///
////////////////////////////////////////////////////////////////////////
Vector operator*(const float ik_number, const Vector& ik_vector)
{
    return Vector(ik_vector.x * ik_number, ik_vector.y * ik_number, ik_vector.z * ik_number);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn float operator*(const Vector& ikx_vector)const
///
/// Op�rateur du produit scalaire
/// 
/// @param[in] ikx_vector: vecteur � multiplier
///
/// @return Scalaire produit par le produit scalaire
///
////////////////////////////////////////////////////////////////////////
float Vector::operator*(const Vector& ik_vector)const
{
    return x * ik_vector.x + y * ik_vector.y + z * ik_vector.z;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector& Vector::operator/= (const float ikf_number)
///
/// Op�rateur d'assignation divisif
/// 
/// @param[in] ikf_number: diviseur scalaire
///
/// @return Vector courant modifi� par l'op�ration
///
////////////////////////////////////////////////////////////////////////
Vector& Vector::operator/= (const float ik_number)
{
    x /= ik_number;
    y /= ik_number;
    z /= ik_number;
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector operator/(const float ikf_number)const
///
/// Op�rateur de division
/// 
/// @param[in] ikf_number: diviseur scalaire
///
/// @return Le vecteur r�sultant de la division entre le vecteur et le scalaire
///
////////////////////////////////////////////////////////////////////////
Vector Vector::operator/(const float ik_number)const
{
    Vector result = *this;
    result /= ik_number;
    return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector crossProduct(const Vector& ikx_vector)const
///
/// Fonction permettant d'effectuer la multiplication vectorielle
/// 
/// @param[in] ikx_vector: vecteur � multiplier
///
/// @return Le vecteur r�sultant de la multiplication vectorielle entre les deux vecteurs
///
////////////////////////////////////////////////////////////////////////
Vector Vector::crossProduct(const Vector& ik_vector)const
{
    Vector result;
    result.x = y * ik_vector.z - z * ik_vector.y;
    result.y = z * ik_vector.x - x * ik_vector.z;
    result.z = x * ik_vector.y - y * ik_vector.x;
    return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector& normalize()
///
/// Fonction normalizant le vecteur
///
/// @return Vecteur normaliz�
///
////////////////////////////////////////////////////////////////////////
Vector& Vector::normalize()
{
	float length = this->length();
	if(length == 0)
		return (*this);
    (*this) /= length;
    return (*this);
}


