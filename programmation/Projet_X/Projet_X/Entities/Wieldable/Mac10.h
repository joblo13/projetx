#ifndef MAC10_H
#define MAC10_H

#include <SDL/SDL.h>
#include "Weapon.h"

class Matrix;
class Camera;
class Mesh;
class Unit;

class Mac10: public Weapon
{
	public:
		Mac10();
		Mac10(const Mac10& i_mac10);
		~Mac10();

		virtual void animate(Uint32 i_timestep);

		virtual void draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera);

		virtual Item* getCopy();

	private:
		void primaryFire(float i_damage);
		void secondaryFire(float i_damage);
		void damageUnit(float i_damage);
};

#endif //MAC10_H
