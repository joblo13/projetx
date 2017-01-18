#ifndef CONSTRUCTOR_WEAPON_H
#define CONSTRUCTOR_WEAPON_H

#include <SDL/SDL.h>
#include "Weapon.h"

class Matrix;
class Camera;
class Unit;

class ConstructorWeapon: public Weapon
{
	public:
		ConstructorWeapon();
		ConstructorWeapon(const ConstructorWeapon& i_constructorWeapon);
		~ConstructorWeapon();

		virtual void animate(Uint32 i_timestep);

		virtual void draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera);

		Item* getCopy();

	private:
		void createCube(int i_cubeType, float i_hitpoint, float i_maxHitpoint);
		void damageCube(float i_damage);
		void repairCube(float i_damage);
};

#endif //CONSTRUCTOR_WEAPON_H
