#ifndef UNIT_H
#define UNIT_H

#include <SDL/SDL.h>
#include <deque>
#include <tuple>

#include "../HitBox.h"
#include "../AI/AStar.h"
#include "../Math/Vector.h"
#include "../Rendering/RenderInformation.h"
#include "ItemInformation.h"
#include "HitpointEntity.h"

#include "Bullet/btBulletDynamicsCommon.h"
#include "Bullet/LinearMath/btIDebugDraw.h"
#include "Bullet/BulletCollision/CollisionShapes/btBoxShape.h"

#include <Bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

#include "../GUI/MapGUI.h"

class Color;
class Mesh;
class HitpointBarMesh;
class WhiteCircle;
class Action;
class Weapon;
class Matrix;
class Camera;
class Armor;
class BoidGroup;


class btKinematicCharacterController;


class Unit: public HitpointEntity
{
public:
		Unit(Mesh* ip_mesh, HitpointBarMesh* ip_hitpointBarMesh, WhiteCircle* ip_whiteCircles);
		~Unit();

		//Affichage
		Vector* const getLeft();
		Vector m_lastPosition;

		float m_theta;
		float m_phi;

		HitBox m_hitBox;

		RenderInformation m_renderInformation;
		RenderInformation m_selectorRenderInformation;
		RenderInformation m_hitpointBarRenderInformation;

		//Description des buts
		std::deque<Action*> mp_targetList;
		AStar m_aStar;

		//Description général de l'unité
		int m_teamID;
		int m_unitID;

		float m_speed;
		
		float m_fallingSpeed;
		bool m_isFalling;

		void stopMoving();
		void move(const Vector& ik_deplacement, bool i_isJumping, Uint32 i_timestep);
		void moveFPS(const Vector& ik_walkingDirection, const Vector& ik_left, int i_sideDir, int i_frontDir, bool i_speedBoost, bool i_isJumping, Uint32 i_timestep);
	
		void setPosition(const Vector& i_position);
		void setTarget(const Vector& i_target, bool i_skipMapUpgrade = false);
		const Vector& getTarget(){return m_target;}

		const Vector& getPosition();
		btVector3 btGetPosition();


		Weapon* const getCurrentWeapon()const
		{
			return mpa_wieldable[m_currentWeapon];
		}
		void switchWeapon(int i_index);
		void setIsShooting(int i_index, bool i_isShooting, bool i_isChangedShooting);
		void setIsReloading(bool i_isReloading);

		
		void drawWeapon(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera);

		int getCurrentAmmo();
		int getCurrentBackupAmmo();

		float getViewAngle();

		bool damage(float i_damage); 

		void animate(Uint32 i_timestep);

		Vector* const setLeft();

		void addElement();
		void removeElement();

		void setItem(ItemType i_itemType, ItemInformation* i_itemInformation);
		const ItemInformation* getItem(ItemType i_itemType);

		//physique
		void initPhysicsAndAddToWorld();
		void reloadPhysic();

		//BOID
		void setBoidGroup(int i_boidGroupID){m_boidGroupID = i_boidGroupID;}
		int getBoidGroup(){return m_boidGroupID;}
		void boidBehaviors(Uint32 i_timestep);
		void radarScan();
		void updateHitpointBar(const Vector& i_cameraForward);

		void setIsSelected(bool i_isSeleted);

		void addAnimation(const std::string& i_animationPath);
		bool getHasMoved(){return m_hasMoved;}

		void getMapShape(std::hash_set<PixelPosition>& i_mapShape);
		void getMapFullShape(std::vector<std::vector<PixelPosition>>& i_mapShape);
		void getMapDirection(std::vector<PixelPosition>& o_result);

	private:
		Mesh* mp_mesh;
		HitpointBarMesh* mp_hitpointBarMesh;
		WhiteCircle* mp_whiteCircles;

		Vector m_target;
		Vector m_position;
		void updateCubeIn();
		void updatePositionWithPhysics();
		void animateWeapon(Uint32 i_timestep);

		void updateMapTarget();

		//physique		
		void exitPhysics();	
		int m_timeSinceLastJump;
		
		//Vitesse
		bool m_isBoosting; //true si shift enfoncé en fps
		float m_speedBonus; //exemple : 1 = vitesse normale, 0.8 = on ralentit, 1.5 = 50% de bonus

		Armor* mpa_wearable[NUM_ARMOR];

		int m_currentWeapon;
		Weapon* mpa_wieldable[10];
		
		Vector m_left;

		//Physique
		btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;

		///keep track of the contact manifolds
		btKinematicCharacterController* m_character;
		btPairCachingGhostObject* m_ghostObject;


		//boid
		int m_boidGroupID;
		std::vector<Unit*> m_neighbors;
		btVector3 m_walkingDirection;

		bool m_isSelected;

		bool m_hasMoved;
};

#endif //UNIT_H