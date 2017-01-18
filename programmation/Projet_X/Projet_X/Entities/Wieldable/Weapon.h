#ifndef WEAPON_H
#define WEAPON_H

#include <SDL/SDL.h>
#include "../../Timer.h"
#include "../../Interpolator.h"
#include "../../Rendering/RenderInformation.h"
#include "../ItemInformation.h"
#include "../Item.h"

class Matrix;
class Camera;
class Mesh;
class Unit;
class Item;

class Weapon:public Item
{
	public:
		Weapon();
		Weapon(const Weapon& i_weapon);
		virtual ~Weapon() = 0;

		virtual void animate(Uint32 i_timestep) = 0;

		virtual void draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera) = 0;

		void setIsShooting(int i_index, bool i_isShooting, bool i_isChangedShooting);
		
		void setIsReloading(bool i_isReloading){m_isReloading |= i_isReloading;}

		float getViewAngle(){return m_viewAngle;}

		int getCurrentBackupAmmo(){return m_currentBackupAmmo;}
		int getCurrentAmmo(){return m_currentAmmo;}

		float getTerrainDamage();

		void reload(Uint32 i_timestep);

		virtual void addElement();
		virtual void removeElement();

		virtual void setUnit(Unit* ip_unit);

	protected:
		Mesh* mp_mesh;
		
		RenderInformation m_renderInformation;

		//Statistique de l'arme
		float m_rechargeSpeed;
		float m_damage[2];

		float m_rateOfFire[2];

		bool m_canDestroyTerrain[2];

		int m_verticalRecoil;
		int m_horizontalRecoil;

		float m_zoom;

		//Munition
		int m_maxBackupAmmo;
		int m_currentBackupAmmo;

		int m_maxAmmo;
		int m_currentAmmo;


		//Variable de gestion
		bool m_isShooting[3];
		bool m_isChangedShooting[3];
		bool m_isReloading;
		float m_viewAngle;
		int m_nbBulletShot;
	
		//timer
		Timer m_primaryFireTimer;
		Timer m_secondaryFireTimer;
		Timer m_rechargeTimer;

		//interpolateur
		Interpolator m_verticalInterpolator;

		Interpolator m_horizontalInterpolator;

		Interpolator m_viewAngleInterpolator;
		Interpolator m_rViewAngleInterpolator;
};

#endif //WEAPON_H
