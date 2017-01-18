#include "Noise2D.h"

#include <assert.h>
#include <math.h>

#include "../Definition.h"

////////////////////////////////////////////////////////////////////////
///
/// @fn modulo(float i_leftNumber, float i_rightNumber)
///
/// modulo avec des nombres à virgule
/// 
/// @param[in] i_leftNumber : numérateur
/// @param[in] i_rightNumber : dénominateur
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
float modulo(float i_leftNumber, float i_rightNumber)
{
    assert(i_rightNumber >= 0);

	i_leftNumber-= i_rightNumber * abs((int)(i_leftNumber/i_rightNumber));
    if (i_leftNumber >= 0.) 
		return i_leftNumber;
    else 
		return i_leftNumber + i_rightNumber;
}
////////////////////////////////////////////////////////////////////////
///
/// @fn Noise2D(unsigned int i_seed)
///
/// Constructeur
/// 
/// @param[in] i_upperLimit : valeur maximale
/// @param[in] i_lowerLimit : valeur minimale
/// @param[in] i_seed : seed utilisé pour le random
///
/// @return Aucune
///
////////////////////////////////////////////////////////////////////////
Noise2D::Noise2D(float i_upperLimit, float i_lowerLimit, unsigned int i_seed)
:m_upperLimit(i_upperLimit), m_lowerLimit(i_lowerLimit)
{
    srand(i_seed);
    for (unsigned int i(0); i < MAX_VERTICES; ++i)
    {
        ma_randomValue[i] = modulo((float)rand(), 128);
        /// assign value to permutation array
        ma_permutation[i] = i;
    }
    /// randomly swap values in permutationa array

	//Permutation aléatoire dans le array de permutation
	int swapIndex;
	int temp;
    for (unsigned int i(0); i < MAX_VERTICES; ++i)
    {
        swapIndex = rand() & MAX_VERTICES_MASK;
        temp = ma_permutation[swapIndex];
        ma_permutation[swapIndex] = ma_permutation[i];
        ma_permutation[i] = temp;
        ma_permutation[i + MAX_VERTICES - 2] = ma_permutation[i];
    }
}

////////////////////////////////////////////////////////////////////////
///
/// @fn float evaluate(float i_x, float i_z)
///
/// Fonction evaluant un bruit de perlin
/// 
/// @param[in] i_x : position en x
/// @param[in] i_z : position en z
///
/// @return Valeur généré
///
////////////////////////////////////////////////////////////////////////
float Noise2D::evaluateSingle(float i_x, float i_z)
{
	//On espace les points connus
	i_x /= 16;
	i_z /= 16;

	//On extrait la partie entière 
    int xInteger = floor(i_x);
    int zInteger = floor(i_z);
    

    float xRadical = i_x - xInteger;
    float zRadical = i_z - zInteger;
     
    int rx0 = xInteger & MAX_VERTICES_MASK;
    int rx1 = (rx0 + 1) & MAX_VERTICES_MASK;
    int rz0 = zInteger & MAX_VERTICES_MASK;
    int rz1 = (rz0 + 1) & MAX_VERTICES_MASK;
     
    //Valeur aléatoire au coin de la cellule grâce à la table de permutation
    float & corner00 = ma_randomValue[ma_permutation[ma_permutation[rx0] + rz0]];
    float & corner10 = ma_randomValue[ma_permutation[ma_permutation[rx1] + rz0]];
    float & corner01 = ma_randomValue[ma_permutation[ma_permutation[rx0] + rz1]];
    float & corner11 = ma_randomValue[ma_permutation[ma_permutation[rx1] + rz1]];
     
    //Modification du x et z selon le smoothstep
    float xSmooth = smoothstep(xRadical);
    float zSmooth = smoothstep(zRadical);
     
    //Interpolation linéaire sur l'axe des x
    float xLinear0 = mix(corner00, corner10, xSmooth);
    float xLinear1 = mix(corner01, corner11, xSmooth);
     
    //Interpolation linéaire selon l'axe des y
    return mix(xLinear0, xLinear1, zSmooth);
}

////////////////////////////////////////////////////////////////////////
///
/// @fn float evaluateFractal(float i_x, float i_z)
///
/// Fonction evaluant la combinaison de plusieur bruit de perlin
/// 
/// @param[in] i_x : position en x
/// @param[in] i_z : position en z
///
/// @return Valeur généré
///
////////////////////////////////////////////////////////////////////////
float Noise2D::evaluateFractal(float i_x, float i_z)
{
	return (evaluateSingle((float)(i_x), (float)(i_z))/128.0f + 
			evaluateSingle((float)(i_x)*2.0f, (float)(i_z)*2.0f)/2.0f/128.0f + 
			evaluateSingle((float)(i_x)*4.0f, (float)(i_z)*4.0f)/4.0f/128.0f) * m_upperLimit + m_lowerLimit;
}