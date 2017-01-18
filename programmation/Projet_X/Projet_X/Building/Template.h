#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "../Math/Vector3I.h"
#include "../Rendering/RenderInformation.h"
#include "../Entities/HitpointEntity.h"
#include <SDL/SDL.h>
#include <hash_map>

#include "../HitBox.h"

class Chunk;
class WhitePrismeInstanciable;
class HitpointBarMesh;
class Unit;

enum BuildingMode{PREPARING_REMOVE, PREPARING_ADD, BUILDING, UPGRADING, DONE};

class Template: public HitpointEntity
{
    public:
        Template(const Chunk* ikp_shape, bool i_useCustomShape, WhitePrismeInstanciable* ip_whitePrismeInstanciable, HitpointBarMesh* ip_hitpointBarMesh);
		
        ~Template();

		RenderInformation& getRenderInformation(){return m_renderInformation;}
		Vector getPosition();
		const Vector3I& getCubePosition();
		float getDimension(int i_index);
		int getCubeDimension(int i_index);

		Vector getBottomCenterPosition(); 

		bool build(Unit* ip_unit, Uint32 i_timestep);

		void calculateCurrentHitpoint();
		
		void setIsSelected(bool i_isSeleted);

		bool updateHitpointBar(const Vector& i_cameraForward);
		HitBox getHitBox(bool i_extended = false);

		bool isPositionOccupied(const Vector3I& i_position);
		void addWorkerPosition(const Vector3I& i_position, Unit* ip_unit);
		void removeWorkerPosition(Unit* ip_unit);

		void upgrade();
		void resetBuildingState();
    private:
		Template(const Template& i_template):m_renderInformation(this), m_hitpointBarRenderInformation(this){}
		void calculateMaxHitpoint();
		void calculateMaxArmor();
		void resetCurrentCube(Vector3I& i_currentCube);
		bool nextCurrentCube(Vector3I& i_currentCube);
		Vector3I getTerrainAbsolutPosition(int i_x, int i_y, int i_z);

		const Chunk* mkp_shape;
		Vector3I m_cubePosition;
		RenderInformation m_renderInformation;
		RenderInformation m_hitpointBarRenderInformation;
		BuildingMode m_currentBuildingMode;
		bool m_useCustomShape;

		WhitePrismeInstanciable* mp_whitePrismeInstanciable;
		HitpointBarMesh* mp_hitpointBarMesh;

		bool m_isSelected;

		int m_targetUpgradeLevel;

		std::hash_map<Vector3I, Unit*> m_workerUsedPosition;
};

#endif // TEMPLATE_H
