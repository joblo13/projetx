#ifndef TERRAIN_H
#define TERRAIN_H

#include <string>
#include <fstream>
#include <hash_set>
#include <vector>
#include <hash_map>

#include "../Definition.h"
#include "../Math/Vector.h"
#include "../Math/Vector3I.h"
#include "../Rendering/Drawable.h"
#include "Chunk.h"

#include "Bullet/btBulletDynamicsCommon.h"
#include "Bullet/LinearMath/btIDebugDraw.h"
#include "Bullet/BulletCollision/CollisionShapes/btBoxShape.h"

class Color;
class Cube;
class Matrix;
class Camera;
struct Position;
class Unit;

//Masques pour les collisions
#define BIT(x) (1<<(x))
enum collisiontypes {
	COL_NOTHING = 0, //<Collide with nothing
	COL_SHIP = BIT(1), //<Collide with ships
	COL_WALL = BIT(2), //<Collide with walls
	COL_BONUS = BIT(3), //<Collide with bonus
	COL_UNIT = BIT(4), //<Collide with units
	COL_TERRAIN_RAYCAST = BIT(5) // Collision avec le terrain raycast
};

class Terrain: public Drawable
{
    public:
		//Singleton
		static Terrain* getInstance()
		{
			if(mp_instance == 0)
			{
				mp_instance = new Terrain();
			}
			return mp_instance;
		}

		static void releaseInstance()
		{
			delete mp_instance;
			mp_instance = 0;
		}

		virtual void animate(Uint32 i_timestep);
		void clearTerrain();
        void loadMap(int i_seed);
		//Fichier
		bool saveMap(const std::string& ik_filePath);
		bool loadMap(const std::string& ik_filePath);

		bool saveChunk(const Chunk& i_chunk, const std::string& ik_filePath);
		bool saveChunk(const Vector3I& i_corner1, const Vector3I& i_corner2, const std::string& ik_filePath);
		Chunk* loadChunk(const Vector3I& i_position, const std::string& ik_filePath);

		void setDimension(int i_width, int i_height, int i_length);

		//Action sur les cubes
		bool createCube(Vector3I& i_cube, int i_type, float i_hitpoint, float i_maxHitpoint);
		bool createCube(int i_x, int i_y, int i_z, int i_type, float i_hitpoint, float i_maxHitpoint);
		bool createCube(int i_x, int i_y, int i_z, int i_type, float i_hitpoint, float i_maxHitpoint, float a_density[8]);
		int switchCubeType(int i_x, int i_y, int i_z, int i_newType);
		void removeCube(Vector3I& i_cube);
		bool removeCube(int i_x, int i_y, int i_z);		
		float damageCube(int i_x, int i_y, int i_z, float i_hit);
        virtual void draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)const;

		void setFullCube(int i_x, int i_y, int i_z);

		std::vector<Vector3I> getCollision(const Vector& i_sourceVect, const Vector& i_targetVect, bool i_onlyFirst = false);

		//Accesseur
        int  getWidth()const
		{
			return m_width;}
        int  getHeight()const
		{
			return m_height;}
        int  getLength()const
		{
			return m_length;}
		
		int getChunkLength(){return mk_chunkLength;}

		btDynamicsWorld* getDynamicWorld()const{return m_dynamicsWorld;}
		btBroadphaseInterface* getBroadphaseInterface()const{return m_broadphase;}

		//Wrappeur d'accesseur
		int getHighestCube(int i_x, int i_z)const;
		bool isFull(int i_x, int i_y, int i_z)const;

		bool isBottomFilled(int i_x, int i_y, int i_z)const;
		//État du cube
		bool isNatural(int i_x, int i_y, int i_z)const{return getCubeType(i_x, i_y, i_z) > 0;}
		bool isAir(int i_x, int i_y, int i_z)const{return getCubeType(i_x, i_y, i_z) == 0;}
		bool isArtificial(int i_x, int i_y, int i_z)const{return getCubeType(i_x, i_y, i_z) < 0;}
		bool isInLimit(int i_x, int i_y, int i_z)const{return i_x >= 0 && i_y >= 0 && i_z >= 0 && i_x < mk_chunkLength * m_width && i_y < mk_chunkLength * m_height && i_z < mk_chunkLength * m_length;}
		bool isWalkable(int i_x, int i_y, int i_z)const{return getCubeType(i_x, i_y - 1, i_z) != 0 && getCubeType(i_x, i_y, i_z) == 0 && getCubeType(i_x, i_y + 1, i_z) == 0;}
		int  getTopWalkable(int i_x, int i_z);

		bool isNaturalOrOutside(int i_x, int i_y, int i_z)const;
		int getCubeType(int i_x, int i_y, int i_z)const;
		bool isHigherThen(int i_x, int i_y, int i_z, FaceType i_faceType)const;
		bool isSmoothTopRenderable(int i_x, int i_y, int i_z)const;
		bool isFlatTopRenderable(int i_x, int i_y, int i_z)const;

		const std::vector<int>* getFullCube(int i_x, int i_y, int i_z)const;

		//Conversion d'unité 
		static int cubeToRelativeCube(const int ik_cubePos)
		{
			return (int)(ik_cubePos % Terrain::getInstance()->getChunkLength());
			
		}
		static int cubeToChunk(const int ik_cubePos)
		{
			return (int)(ik_cubePos / Terrain::getInstance()->getChunkLength());
			
		}
		static Vector3I positionToCube(const Vector& ik_position)
		{
			Vector3I x_cubePos;
			x_cubePos.x = (int)((ik_position.x + HALFSIDE)/SIDE);
			x_cubePos.y = (int)((ik_position.y + HALFSIDE)/SIDE);
			x_cubePos.z = (int)((ik_position.z + HALFSIDE)/SIDE);
			return x_cubePos;
		}
		static int positionToCube(const float ik_position)
		{
			return (int)((ik_position + HALFSIDE)/SIDE);
			
		}
		static float cubeToPosition(const int ik_cubePos)
		{
			return SIDE * ik_cubePos /*- HALFSIDE*/;
		}
		static Vector cubeToPosition(const Vector3I& ik_cubePos)
		{
			Vector position;
			position.x = SIDE * ik_cubePos.x /*- HALFSIDE*/;
			position.y = SIDE * ik_cubePos.y /*- HALFSIDE*/;
			position.z = SIDE * ik_cubePos.z /*- HALFSIDE*/;
			return position;
		}
		//Permet d'obtenir la position du point en négligeant la position du cube
		static Vector normalizePosition(const Vector& ik_position)
		{
			Vector3I cubePos = positionToCube(ik_position);
			Vector result;
			result.x = (ik_position.x - ((float)(cubePos.x*SIDE) - HALFSIDE))/SIDE;
			result.y = (ik_position.y - ((float)(cubePos.y*SIDE) - HALFSIDE))/SIDE;
			result.z = (ik_position.z - ((float)(cubePos.z*SIDE) - HALFSIDE))/SIDE;
			return result;
		}

		std::hash_multimap<Position, std::pair<Unit*, Color>>::iterator addUnitToPosition(const Position& i_position, Unit* ip_unit);
		void removeUnitFromPosition(std::hash_multimap<Position, std::pair<Unit*, Color>>::iterator i_it);

		void getMapColor(std::vector<unsigned char>& i_mapOverview);
		void getMapOverviewSingleColor(int i_x, int i_z, unsigned char i_color[3]);

		void getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result);
		void getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result, std::vector<VertexPosition>& i_oldVertexList);

		float getIsoPosition(int i_x, int i_y, int i_z, unsigned int i_index);
		void setIsoPosition(int i_x, int i_y, int i_z, unsigned int i_index, float i_isoPosition);
		void setRenforcement(int i_x, int i_y, int i_z, int i_renforcement);

		void addToUpdateList(int i_x, int i_y, int i_z);
		void updateAllChunk();
		
		
		void setChangedFlag(int i_x, int i_y, int i_z, bool i_value);

		float repairCube(int i_x, int i_y, int i_z, float i_hitpoint);
		bool upgradeCube(int i_x, int i_y, int i_z);
		float getCubeHitpoint(int i_x, int i_y, int i_z);
		float getCubeMaxHitpoint(int i_x, int i_y, int i_z);

		void getDensity(int i_x, int i_y, int i_z, float i_density[8]);

		Vector3I lastDestroyedCube;

		void updateMapOverview(int i_chunkX, int i_chunkZ, int i_x, int i_z);
		
		void setMapOverviewSingleColor(int i_x, int i_z, unsigned char i_color[3]);

		void reloadMapOverviewSingleColor(int i_realX, int i_realZ);

    protected:
    private:
		//On cache les constructeurs et les destructeurs puisque c'est un singleton
		Terrain();
		~Terrain();

		//Action sur les cubes spéciales
		bool createCubePrivate(int i_x, int i_y, int i_z, int i_type, float i_hitpoint, float i_maxHitpoint, float ia_density[8]);
		void removeUnconnectedCube(int i_x, int i_y, int i_z, std::hash_set<Position>& i_cubeToRemoveList);
		void addNeighbor(std::hash_set<Position>& i_cubeToRemoveList);
		bool addNeighbor(std::hash_set<Position>& i_cubeToRemoveList, int i_x, int i_y, int i_z);

		bool damageCubePrivate(int i_x, int i_y, int i_z, float i_hit);

		

		//Physique
		void initPhysics();
		void exitPhysics();		
		void resetPhysics();

		//Information sur la map
		Chunk**** m4p_chunkMap;
		int m_width;
		int m_height;
		int m_length;
		const int mk_chunkLength;
		//Instance du singleton
		static Terrain* mp_instance;
		//Permet de différencier un terrain construit à un terrain chargé
		bool m_initialised;

		std::hash_set<Vector3I> m_chunksToUpdate;

		bool m_showWire;

		//Physique
		btBroadphaseInterface*	m_broadphase;
		btCollisionDispatcher*	m_dispatcher;
		btConstraintSolver*	m_solver;
		btDefaultCollisionConfiguration* m_collisionConfiguration;
		btDynamicsWorld* m_dynamicsWorld;

		std::vector<unsigned char> m_mapOverview;
};

#endif // TERRAIN_H
