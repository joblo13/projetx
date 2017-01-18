#include "Matrix.h"

#include <assert.h>

#include "Vector.h"
#include "Vector4f.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix()
///
/// Constructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix::Matrix() 
:mp_previousSave(0)
{
    // Initialisation de toutes les valeurs à 0
    for(int i(0); i < 16; ++i)
        ma_values[i] = 0.0;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix(float* ip_values)
///
/// Constructeur par valeur
/// 
/// @param[in] ip_values : pointeur vers les valeurs 
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix::Matrix(float* ip_values) 
:mp_previousSave(0)
{
    // Copie des valeurs
    for(int i = 0; i < 16; ++i)
        ma_values[i] = ip_values[i];
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix(Matrix const& ik_matrix)
///
/// Constructeur par copie
/// 
/// @param[in] &ik_matrix : matrice à copier 
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix::Matrix(Matrix const& ik_matrix) 
:mp_previousSave(0)
{
    // Copie des valeurs de la matrice à copier
    for(int i(0); i < 16; ++i)
        ma_values[i] = ik_matrix.ma_values[i];
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix(const aiMatrix4x4& ik_assimpMatrix)
///
/// Convertisseur de matrice assimp
/// 
/// @param[in] &ik_assimpMatrix : matrice à convertir
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix::Matrix(const aiMatrix4x4& ik_assimpMatrix)
:mp_previousSave(0)
{
	ma_values[0] = ik_assimpMatrix.a1; ma_values[1] = ik_assimpMatrix.a2; ma_values[2] = ik_assimpMatrix.a3; ma_values[3] = ik_assimpMatrix.a4;
	ma_values[4] = ik_assimpMatrix.b1; ma_values[5] = ik_assimpMatrix.b2; ma_values[6] = ik_assimpMatrix.b3; ma_values[7] = ik_assimpMatrix.b4;
	ma_values[8] = ik_assimpMatrix.c1; ma_values[9] = ik_assimpMatrix.c2; ma_values[10] = ik_assimpMatrix.c3; ma_values[11] = ik_assimpMatrix.c4;
	ma_values[12] = ik_assimpMatrix.d1; ma_values[13] = ik_assimpMatrix.d2; ma_values[14] = ik_assimpMatrix.d3; ma_values[15] = ik_assimpMatrix.d4;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix(const aiMatrix3x3& ik_assimpMatrix)
///
/// Convertisseur de matrice assimp
/// 
/// @param[in] &ik_assimpMatrix : matrice à convertir
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix::Matrix(const aiMatrix3x3& ik_assimpMatrix)
:mp_previousSave(0)
{
	ma_values[0] = ik_assimpMatrix.a1; ma_values[1] = ik_assimpMatrix.a2; ma_values[2] = ik_assimpMatrix.a3; ma_values[3] = 0;
	ma_values[4] = ik_assimpMatrix.b1; ma_values[5] = ik_assimpMatrix.b2; ma_values[6] = ik_assimpMatrix.b3; ma_values[7] = 0;
	ma_values[8] = ik_assimpMatrix.c1; ma_values[9] = ik_assimpMatrix.c2; ma_values[10] = ik_assimpMatrix.c3; ma_values[11] = 0;
	ma_values[12] = 0; ma_values[13] = 0; ma_values[14] = 0; ma_values[15] = 1;
} 

////////////////////////////////////////////////////////////////////////
///
/// @fn ~Matrix()
///
/// Destructeur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix::~Matrix()
{
    // Libération de la mémoire
    clear();
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix(Matrix const &ik_matrix)
///
/// Constructeur par copie
/// 
/// @param[in] &ik_matrix : matrice à copier 
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix Matrix::operator*(Matrix const& ik_matrix)const
{
    // Matrix résultat
    Matrix result;

    // Addition des multiplications
    for(int k(0); k < 4; ++k)
    {
        for(int j(0); j < 4; ++j)
        {
            for(int i(0); i < 4; ++i)
                result.ma_values[4 * j + k] += ma_values[4 * j + i] * ik_matrix.ma_values[4 * i + k];
        }
    }

    // Envoi de la matrice resultat
    return result;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix& operator=(Matrix const& ik_matrix)
///
/// Operateur d'assignation
/// 
/// @param[in] &ik_matrix : matrice à copier 
///
/// @return Pointeur vers la matrice courante
///
////////////////////////////////////////////////////////////////////////
Matrix& Matrix::operator=(Matrix const& ik_matrix)
{
    // Copie des valeurs
    for(int i(0); i < 16; i++)
        ma_values[i] = ik_matrix.ma_values[i];

    // Renvoi de l'objet
    return *this;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix& operator*=(Matrix const& ik_matrix)
///
/// Operateur d'assignation multiplicative
/// 
/// @param[in] &ik_matrix : matrice à multiplier
///
/// @return Pointeur vers la matrice courante
///
////////////////////////////////////////////////////////////////////////
Matrix Matrix::operator*=(Matrix const& ik_matrix)
{

	// Addition des multiplications
	for(int k(0); k < 4; ++k)
	{
		for(int j(0); j < 4; ++j)
		{
			for(int i(0); i < 4; ++i)
				this->ma_values[4 * j + k] += ma_values[4 * j + i] * ik_matrix.ma_values[4 * i + k];
		}
	}

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
Vector Matrix::operator*(Vector const& ik_vector)
{
	//assert(false);
	Vector result;
	float wComponant(1.f);
	result.x = ma_values[0] * ik_vector.x + ma_values[1] * ik_vector.y + ma_values[2] * ik_vector.z + ma_values[3] * wComponant;
	result.y = ma_values[4] * ik_vector.x + ma_values[5] * ik_vector.y + ma_values[6] * ik_vector.z + ma_values[7] * wComponant;
	result.z = ma_values[8] * ik_vector.x + ma_values[9] * ik_vector.y + ma_values[10] * ik_vector.z + ma_values[11] * wComponant;

	return result;
}

Vector4f Matrix::operator*(Vector4f const& ik_vector)
{
	//assert(false);
	Vector4f result;
	result.x = ma_values[0] * ik_vector.x + ma_values[1] * ik_vector.y + ma_values[2] * ik_vector.z + ma_values[3] * ik_vector.w;
	result.y = ma_values[4] * ik_vector.x + ma_values[5] * ik_vector.y + ma_values[6] * ik_vector.z + ma_values[7] * ik_vector.w;
	result.z = ma_values[8] * ik_vector.x + ma_values[9] * ik_vector.y + ma_values[10] * ik_vector.z + ma_values[11] * ik_vector.w;
	result.w = ma_values[12] * ik_vector.x + ma_values[13] * ik_vector.y + ma_values[14] * ik_vector.z + ma_values[15] * ik_vector.w;

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
float* const Matrix::getValues() const
{
	// On renvoi les valeurs sous forme de pointeur de float
    return (float* const) ma_values;
}
void Matrix::getValues(std::vector<float>& i_value) const
{
	i_value.insert(i_value.end(), ma_values, ma_values + 16);
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
void Matrix::loadIdentity()
{
    // On charge une matrice vide
    for(int i(0); i < 16; ++i)
        ma_values[i] = 0.0;

    // Puis nous mettons à 1 les valeurs de la diagonale
    ma_values[0] = 1.0;
    ma_values[5] = 1.0;
    ma_values[10] = 1.0;
    ma_values[15] = 1.0;
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
void Matrix::loadZero()
{
    // Libération de la mémoire
    clear();

    // On charge une matrice vide
    for(int i(0); i < 16; ++i)
        ma_values[i] = 0.0;
}


////////////////////////////////////////////////////////////////////////
///
/// @fn void translate(float i_x, float i_y, float i_z)
///
/// Fonction appliquant une translation à la matrice courante
/// 
/// @param[in] i_x: modification de la position en x
/// @param[in] i_y: modification de la position en y
/// @param[in] i_z: modification de la position en z
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Matrix::translate(float i_x, float i_y, float i_z)
{
	//Ce calcule est l'équivalent à la multiplication de la matrice actuel avec la matrice identité combinée au vecteur de translation
	for(int i(0); i < 16; i+=4)
		this->ma_values[i+3] = this->ma_values[i+3] + this->ma_values[i] * i_x + this->ma_values[i+1] * i_y + this->ma_values[i+2] * i_z;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void scale(float i_x, float i_y, float i_z)
///
/// Fonction appliquant une homothétie à la matrice courante
/// 
/// @param[in] i_x: modification de l'échelle en x
/// @param[in] i_y: modification de l'échelle en y
/// @param[in] i_z: modification de l'échelle en z
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Matrix::scale(float i_x, float i_y, float i_z)
{
    // Création de la matrice scale
    Matrix scaleMatrix;

    // Ajout des paramètres
    scaleMatrix.ma_values[0] = i_x;
    scaleMatrix.ma_values[5] = i_y;
    scaleMatrix.ma_values[10] = i_z;
    scaleMatrix.ma_values[15] = 1.0;

    // Multiplication de la matrice (*this) par la matrice scale
    *this = *this * scaleMatrix;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void rotate(float i_angle, float i_x, float i_y, float i_z)
///
/// Fonction appliquant une rotation à la matrice courante
/// 
/// @param[in] i_angle: angle de la rotation
/// @param[in] i_x: composante en x de l'axe de rotation
/// @param[in] i_y: composante en y de l'axe de rotation
/// @param[in] i_z: composante en z de l'axe de rotation
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Matrix::rotate(float i_angle, float i_x, float i_y, float i_z)
{
    // Création de la matrice de rotation
    Matrix rotationMatrix;

    // Conversion de l'angle de degrés vers radians
    i_angle = i_angle * MATH_PI / 180;

    // Normalisation du vecteur axe
    Vector axe = Vector(i_x, i_y, i_z);
	axe.normalize();

    // Ajout des paramètres
	rotationMatrix.ma_values[0] = axe.x*axe.x * (1 - cos(i_angle)) + cos(i_angle);
	rotationMatrix.ma_values[1] = axe.x*axe.y * (1 - cos(i_angle)) - axe.z*sin(i_angle);
	rotationMatrix.ma_values[2] = axe.x*axe.z * (1 - cos(i_angle)) + axe.y*sin(i_angle);

	rotationMatrix.ma_values[4] = axe.x*axe.y * (1 - cos(i_angle)) + axe.z*sin(i_angle);
	rotationMatrix.ma_values[5] = axe.y*axe.y * (1 - cos(i_angle)) + cos(i_angle);
	rotationMatrix.ma_values[6] = axe.y*axe.z * (1 - cos(i_angle)) - axe.x*sin(i_angle);

	rotationMatrix.ma_values[8] = axe.x*axe.z * (1 - cos(i_angle)) - axe.y*sin(i_angle);
	rotationMatrix.ma_values[9] = axe.y*axe.z * (1 - cos(i_angle)) + axe.x*sin(i_angle);
	rotationMatrix.ma_values[10] = axe.z*axe.z * (1 - cos(i_angle)) + cos(i_angle);

    rotationMatrix.ma_values[15] = 1.0;

    // Multiplication des deux matrices
    *this = *this * rotationMatrix;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void perspective(float if_angle, float if_ratio, float if_near, float if_far)
///
/// Fonction permettant de créer la matrice de perspective grâce au paramètre
/// 
/// @param[in] if_angle: angle du cône de vision
/// @param[in] if_ratio: proportion de la fenêtre (largeur/hauteur)
/// @param[in] if_near: distance minimum à afficher
/// @param[in] if_far: distance maximum à afficher
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Matrix::perspective(float i_angle, float i_ratio, float i_near, float i_far)
{
    //Matrix de projection
    Matrix scalingMatrix;

	//Valeur dérivé du l'angle de vue
    float radAngle (1 / tan((i_angle / 2) * MATH_PI / 180));


    //Remplissage des valeurs de la matrice
    scalingMatrix.ma_values[0] = radAngle / i_ratio;
    scalingMatrix.ma_values[5] = radAngle;

    scalingMatrix.ma_values[10] = (i_near + i_far) / (i_near - i_far);
    scalingMatrix.ma_values[11] = (2 * i_near * i_far) / (i_near - i_far);

    scalingMatrix.ma_values[14] = -1;


    //Multiplication des deux matrices
    *this = *this * scalingMatrix;
}

////////////////////////////////////////////////////////////////////////
///
/// @fn void lookAt(float i_eyeX, float i_eyeY, float i_eyeZ, float i_centerX, float i_centerY, float i_centerZ, float i_upX, float i_upY, float i_upZ)
///
/// Fonction deplacement la position de la scène observé pour la mettre à l'oeil de l'observateur
/// 
/// @param[in] i_eyeX: Position de l'observateur en x
/// @param[in] i_eyeY: Position de l'observateur en y
/// @param[in] i_eyeZ: Position de l'observateur en z
/// @param[in] i_centerX: position du point observé en x
/// @param[in] i_centerY: position du point observé en y
/// @param[in] i_centerZ: position du point observé en z
/// @param[in] i_upX: composante x du vecteur définissant le haut de la caméra
/// @param[in] i_upY: composante y du vecteur définissant le haut de la caméra
/// @param[in] i_upZ: composante z du vecteur définissant le haut de la caméra
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
void Matrix::lookAt(float i_eyeX, float i_eyeY, float if_eyeZ, float i_centerX, float i_centerY, float i_centerZ, float i_upX, float i_upY, float i_upZ)
{
    // Création des vecteurs
    Vector axe = Vector(i_upX, i_upY, i_upZ);
    Vector view = Vector(i_centerX - i_eyeX, i_centerY - i_eyeY, i_centerZ - if_eyeZ);

    Vector normal = Vector(0, 0, 0);
    Vector newAxe = Vector(0, 0, 0);


    // Création de la matrice à multiplier
    Matrix matrix;


    // Multiplications des vecteurs
	normal = view.crossProduct(axe);
	newAxe = normal.crossProduct(view);

    // Normalisation des vecteurs
	normal.normalize();
	newAxe.normalize();
	view.normalize();

    // Introduction des valeurs dans la matrice
	matrix.ma_values[0] = normal.x;
	matrix.ma_values[1] = normal.y;
	matrix.ma_values[2] = normal.z;

	matrix.ma_values[4] = newAxe.x;
	matrix.ma_values[5] = newAxe.y;
	matrix.ma_values[6] = newAxe.z;

	matrix.ma_values[8] = -view.x;
	matrix.ma_values[9] = -view.y;
	matrix.ma_values[10] = -view.z;

    matrix.ma_values[15] = 1.0;


    // Multiplication des matrices, puis translation de la matrice modelview
    *this = *this * matrix;
    translate(-i_eyeX, -i_eyeY, -if_eyeZ);
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
bool Matrix::push()
{
    // Nouvelle case dans la pile
    Matrix* p_newSlot = new Matrix;


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
bool Matrix::pop()
{
    // Création d'un pointeur temporaire
    Matrix* p_matrixTmp = mp_previousSave;


    // Si la pile existe toujours
    if(p_matrixTmp != 0)
    {
        // Copie des valeurs depuis la sauvegarde
        *this = *p_matrixTmp;

        // Redéfinition du sommet de la pile
        mp_previousSave = p_matrixTmp->mp_previousSave;

        // On libère la sauvegarde à supprimer
        p_matrixTmp->mp_previousSave = 0;
        delete p_matrixTmp;

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
void Matrix::clear()
{
    // Tant qu'il y a des sauvegardes, on les détruit
    while(pop() != false);
}
////////////////////////////////////////////////////////////////////////
///
/// @fn Matrix transpose()
///
/// Fonction permettant de transposer la matrice
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Matrix Matrix::transpose()
{
	float oldValues[16];

	for(int i(0); i < 16; ++i)
		oldValues[i] = ma_values[i];
	
	int row(0), column(0), tidx(0);
	for(int j(0); j < 16; ++j)
	{
		row = j/4;
		column = j%4;
		tidx = column*4 + row;
		ma_values[tidx] = oldValues[j];
	}

	return *this;
}