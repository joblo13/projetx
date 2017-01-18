#ifndef NOISE_2D_H
#define NOISE_2D_H

//Implémentation basée sur http://scratchapixel.com/lessons/3d-advanced-lessons/noise-part-1/creating-a-simple-2d-noise/
#include <stdlib.h>

#define MAX_VERTICES 256
#define MAX_VERTICES_MASK (MAX_VERTICES - 1)

class Noise2D
{
	public:
		Noise2D(float i_range, float i_lowerLimit, unsigned int i_seed = 10);

		float evaluateFractal(float i_x, float i_z);
		float evaluateSingle(float i_x, float i_z);

	private:
		//Fonction mathématique custom
		inline float mix(float& i_a, float& i_b, float& i_t){return i_a * ( 1 - i_t ) + i_b * i_t;}
		inline int floor(float i_x){return (int)i_x - ( i_x < 0 && (int)i_x != i_x );}
		inline float smoothstep(float & i_t){return i_t * i_t * ( 3 - 2 * i_t );}

		//Tableau de données aléatoire
		float ma_randomValue[MAX_VERTICES];
		//Table de permutation
		unsigned int ma_permutation[MAX_VERTICES_MASK * 2];

		float m_upperLimit;
		float m_lowerLimit;
};

#endif //NOISE_2D_H