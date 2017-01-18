#include "Vector.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector()
///
/// Constructeur par défaut
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
/// Constructeur par paramètre
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
/// @param[in] ik_vector: vecteur à copier
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
/// @param[in] ik_from: point de départ
/// @param[in] ik_to: point d'arrivé départ
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
/// Opérateur d'assignation
/// 
/// @param[in] ik_vector: vecteur à assigner
///
/// @return Le vecteur courant qui vient d'être assigné
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
/// Opérateur d'assignation additif
/// 
/// @param[in] ik_vector: vecteur à additionner au vecteur courant
///
/// @return Le vecteur courant qui vient d'être modifié
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
/// Opérateur d'addition
/// 
/// @param[in] ik_vector: vecteur à additionner au vecteur courant
///
/// @return Le vecteur résultant de l'addition des deux vecteurs
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
/// Opérateur d'assignation soustractif
/// 
/// @param[in] ik_vector: vecteur à soustraire au vecteur courant
///
/// @return Le vecteur courant qui vient d'être modifié
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
/// Opérateur de soustraction
/// 
/// @param[in] ik_vector: vecteur à soustraire au vecteur courant
///
/// @return Le vecteur résultant de la soustraction des deux vecteurs
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
/// Opérateur d'assignation multiplicatif
/// 
/// @param[in] ik_number: nombre à multiplier
///
/// @return Le vecteur courant qui vient d'être modifié
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
/// Opérateur de multiplication avec un nombre scalaire
/// 
/// @param[in] ik_number: Nombre à multiplier
///
/// @return Le vecteur résultant de la multiplication du vecteur et du scalaire
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
/// Opérateur de multiplication avec un nombre scalaire et un vecteur (de l'autre sens)
/// 
/// @param[in] ikf_number: Nombre à multiplier
/// @param[in] ikx_vector: vecteur à multiplier
///
/// @return Le vecteur résultant de la multiplication du vecteur et du scalaire
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
/// Opérateur du produit scalaire
/// 
/// @param[in] ikx_vector: vecteur à multiplier
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
/// Opérateur d'assignation divisif
/// 
/// @param[in] ikf_number: diviseur scalaire
///
/// @return Vector courant modifié par l'opération
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
/// Opérateur de division
/// 
/// @param[in] ikf_number: diviseur scalaire
///
/// @return Le vecteur résultant de la division entre le vecteur et le scalaire
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
/// @param[in] ikx_vector: vecteur à multiplier
///
/// @return Le vecteur résultant de la multiplication vectorielle entre les deux vecteurs
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
/// @return Vecteur normalizé
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


