#include "Matrix3x3.h"

#include "Vector.h"
#include "../Math/Matrix.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix3x3()
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix3x3::Matrix3x3() 
:mp_previousSave(0)
{
    // Initialisation de toutes les valeurs à 0
    for(int i(0); i < 9; ++i)
        ma_values[i] = 0.0;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix3x3(const Matrix& i_matrix)
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix3x3::Matrix3x3(const Matrix& i_matrix)
:mp_previousSave(0)
{
	float* values = i_matrix.getValues();

	ma_values[0] = values[0]; ma_values[1] = values[1]; ma_values[2] = values[2];
	ma_values[3] = values[4]; ma_values[4] = values[5]; ma_values[5] = values[6];
	ma_values[6] = values[8]; ma_values[7] = values[9]; ma_values[8] = values[10];
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix3x3(float* ip_values)
///
/// Constructeur par valeur
/// 
/// @param[in] ip_values : pointeur vers les valeurs 
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix3x3::Matrix3x3(float* ip_values) 
:mp_previousSave(0)
{
    // Copie des valeurs
    for(int i = 0; i < 9; ++i)
        ma_values[i] = ip_values[i];
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix3x3(Matrix3x3 const& ik_matrix3x3)
///
/// Constructeur par copie
/// 
/// @param[in] &ik_matrix : matrice à copier 
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix3x3::Matrix3x3(Matrix3x3 const& ik_matrix3x3) 
:mp_previousSave(0)
{
    // Copie des valeurs de la matrice à copier
    for(int i(0); i < 9; ++i)
        ma_values[i] = ik_matrix3x3.ma_values[i];
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix3x3(const aiMatrix4x4& ik_assimpMatrix)
///
/// Convertisseur de matrice assimp
/// 
/// @param[in] &ik_assimpMatrix : matrice à convertir
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix3x3::Matrix3x3(const aiMatrix4x4& ik_assimpMatrix)
:mp_previousSave(0)
{
	ma_values[0] = ik_assimpMatrix.a1; ma_values[1] = ik_assimpMatrix.a2; ma_values[2] = ik_assimpMatrix.a3; 
	ma_values[3] = ik_assimpMatrix.b1; ma_values[4] = ik_assimpMatrix.b2; ma_values[5] = ik_assimpMatrix.b3;
	ma_values[6] = ik_assimpMatrix.c1; ma_values[7] = ik_assimpMatrix.c2; ma_values[8] = ik_assimpMatrix.c3;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix3x3(const aiMatrix3x3& ik_assimpMatrix)
///
/// Convertisseur de matrice assimp
/// 
/// @param[in] &ik_assimpMatrix : matrice à convertir
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix3x3::Matrix3x3(const aiMatrix3x3& ik_assimpMatrix)
:mp_previousSave(0)
{
	ma_values[0] = ik_assimpMatrix.a1; ma_values[1] = ik_assimpMatrix.a2; ma_values[2] = ik_assimpMatrix.a3; 
	ma_values[3] = ik_assimpMatrix.b1; ma_values[4] = ik_assimpMatrix.b2; ma_values[5] = ik_assimpMatrix.b3;
	ma_values[6] = ik_assimpMatrix.c1; ma_values[7] = ik_assimpMatrix.c2; ma_values[8] = ik_assimpMatrix.c3;
} 

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Matrix3x3()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix3x3::~Matrix3x3()
{
    // Libération de la mémoire
    clear();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix3x3(Matrix3x3 const &ik_matrix)
///
/// Constructeur par copie
/// 
/// @param[in] &ik_matrix : matrice à copier 
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix3x3 Matrix3x3::operator*(Matrix3x3 const& ik_matrix)
{
    // Matrix résultat
    Matrix3x3 result;

    // Addition des multiplications
    for(int k(0); k < 3; ++k)
    {
        for(int j(0); j < 3; ++j)
        {
            for(int i(0); i < 3; ++i)
                result.ma_values[3 * j + k] += ma_values[3 * j + i] * ik_matrix.ma_values[3 * i + k];
        }
    }

    // Envoi de la matrice resultat
    return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix3x3& operator=(Matrix3x3 const& ik_matrix)
///
/// Operateur d'assignation
/// 
/// @param[in] &ik_matrix3x3 : matrice à copier 
///
/// @return Pointeur vers la matrice courante
///
////////////////////////////////////////////////////////////////////////
Matrix3x3& Matrix3x3::operator=(Matrix3x3 const& ik_matrix3x3)
{
    // Copie des valeurs
    for(int i(0); i < 9; i++)
        ma_values[i] = ik_matrix3x3.ma_values[i];

    // Renvoi de l'objet
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Vector operator*(Vector const& ik_vector)
///
/// Operateur de multiplication avec un vecteur
/// 
/// @param[in] &ik_vector : vecteur à multiplier
///
/// @return Resultat de la multiplication
///
////////////////////////////////////////////////////////////////////////
Vector Matrix3x3::operator*(Vector const& ik_vector)
{
	Vector result;
	result.x = ma_values[0] * result.x + ma_values[1] * result.y +  ma_values[2] * result.z + ma_values[3];
	result.y = ma_values[3] * result.x + ma_values[4] * result.y +  ma_values[5] * result.z + ma_values[7];
	result.z = ma_values[6] * result.x + ma_values[7] * result.y +  ma_values[8] * result.z + ma_values[11];

	return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn float* getValues()
///
/// Accesseur de valeurs
///
/// @return Pointeur vers les valeurs
///
////////////////////////////////////////////////////////////////////////
float* const Matrix3x3::getValues() const
{
	// On renvoi les valeurs sous forme de pointeur de float
    return (float* const) ma_values;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void loadIdentity()
///
/// Remplace la matrice courante par la matrice identité
/// 
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Matrix3x3::loadIdentity()
{
    // Libération de la mémoire
    clear();

    // On charge une matrice vide
    for(int i(0); i < 9; ++i)
        ma_values[i] = 0.0;

    // Puis nous mettons à 1 les valeurs de la diagonale
    ma_values[0] = 1.0;
    ma_values[4] = 1.0;
    ma_values[8] = 1.0;
}
////////////////////////////////////////////////////////////////////////
///
/// @fn void loadZero()
///
/// Remplace la matrice courante par la matrice zéro
/// 
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Matrix3x3::loadZero()
{
    // Libération de la mémoire
    clear();

    // On charge une matrice vide
    for(int i(0); i < 9; ++i)
        ma_values[i] = 0.0;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool push()
///
/// Fonction permettant d'enregistrer la matrice courrante
///
/// @return bool confirmant que la matrice à bien été enregistrée
///
////////////////////////////////////////////////////////////////////////
bool Matrix3x3::push()
{
    // Nouvelle case dans la pile
    Matrix3x3* p_newSlot = new Matrix3x3;


    // Si l'allocation réussit
    if(p_newSlot != 0)
    {
        // Copie des valeurs dans la nouvelle case
        *p_newSlot = *this;

        // On pointe sur la sauvegarde précédente
        p_newSlot->mp_previousSave = mp_previousSave;


        // Redéfinition du sommet de la pile
        mp_previousSave = p_newSlot;

        return true;
    }
    // Sinon l'allocation a échoué
    else
        return false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn bool pop()
///
/// Fonction permettant de charger une matrice enregistrée et de remplacer la matrice courante
///
/// @return bool permettant de savoir si le chargement de la matrice à pu être effectué
///
////////////////////////////////////////////////////////////////////////
bool Matrix3x3::pop()
{
    // Création d'un pointeur temporaire
    Matrix3x3* p_matrix3x3Tmp = mp_previousSave;


    // Si la pile existe toujours
    if(p_matrix3x3Tmp != 0)
    {
        // Copie des valeurs depuis la sauvegarde
        *this = *p_matrix3x3Tmp;

        // Redéfinition du sommet de la pile
        mp_previousSave = p_matrix3x3Tmp->mp_previousSave;

        // On libère la sauvegarde à supprimer
        p_matrix3x3Tmp->mp_previousSave = 0;
        delete p_matrix3x3Tmp;

        // La restauration s'est bien passée, on retourne true
        return true;
    }
    // Sinon la pile n'existe pas
    else
        return false;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void clear()
///
/// Fonction permettant de vider la pile de matrice
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Matrix3x3::clear()
{
    // Tant qu'il y a des sauvegardes, on les détruit
    while(pop() != false);
}

void Matrix3x3::invert()
{
	float oldValues[9];
	for(int i(0); i < 9; ++i)
		oldValues[i] = ma_values[i];

	float factor = oldValues[0] * oldValues[4] * oldValues[8] + oldValues[1] * oldValues[5] * oldValues[6] + oldValues[2] * oldValues[3] * oldValues[7]
					- oldValues[6] * oldValues[4] * oldValues[2] - oldValues[7] * oldValues[5] * oldValues[0] - oldValues[8] * oldValues[3] * oldValues[1];

	ma_values[0] = ((oldValues[4] * oldValues[8]) - (oldValues[5] * oldValues[7]))/factor;
	ma_values[1] = -((oldValues[3] * oldValues[8]) - (oldValues[5] * oldValues[6]))/factor;
	ma_values[2] = ((oldValues[3] * oldValues[7]) - (oldValues[4] * oldValues[6]))/factor;
	ma_values[3] = -((oldValues[1] * oldValues[8]) - (oldValues[2] * oldValues[7]))/factor;
	ma_values[4] = ((oldValues[0] * oldValues[8]) - (oldValues[2] * oldValues[6]))/factor;
	ma_values[5] = -((oldValues[0] * oldValues[7]) - (oldValues[1] * oldValues[6]))/factor;
	ma_values[6] = ((oldValues[1] * oldValues[5]) - (oldValues[2] * oldValues[4]))/factor;
	ma_values[7] = -((oldValues[0] * oldValues[5]) - (oldValues[2] * oldValues[3]))/factor;
	ma_values[8] = ((oldValues[0] * oldValues[4]) - (oldValues[1] * oldValues[3]))/factor;

	transpose();
}

void Matrix3x3::transpose()
{
	float oldValues[9];
	for(int i(0); i < 9; ++i)
		oldValues[i] = ma_values[i];

	ma_values[1] = oldValues[3];
	ma_values[2] = oldValues[6];
	ma_values[3] = oldValues[1];
	ma_values[5] = oldValues[7];
	ma_values[6] = oldValues[2];
	ma_values[7] = oldValues[5];
}
