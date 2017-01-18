#ifndef CHUNK_H
#define CHUNK_H

#include <vector>
#include <iterator>
#include <SDL/SDL.h>

#include "../Rendering/Drawable.h"

#include "Bullet/btBulletDynamicsCommon.h"
#include "Bullet/LinearMath/btIDebugDraw.h"
#include "Bullet/BulletCollision/CollisionShapes/btBoxShape.h"
#include "../Math/Vector3I.h"
#include "../Math/Vector.h"
#include "../Rendering/RenderInformation.h"

#include "../Rendering/WhiteTerrainLine.h"

#include "../Definition.h"

class WhitePrismeInstanciable;
class Matrix;
class Camera;

enum FaceType{LEFT_FACE, RIGHT_FACE, FRONT_FACE, BACK_FACE, BOTTOM_FACE};

class Chunk: public Drawable
{
    public:
        Chunk(int i_x, int i_y, int i_z, int i_sizeX, int i_sizeY, int i_sizeZ, const bool ik_isTerrainChunk = true);
		Chunk(const Chunk& i_chunk);
        ~Chunk();

		virtual void animate(Uint32 i_timestep){}
		//Action sur un cube
		void createCube(int i_x, int i_y, int i_z, int i_type, float i_hitpoint, float i_maxHitpoint, float ia_density[8]);
        void removeCube(int i_x, int i_y, int i_z);
		float damageCube(int i_x, int i_y, int i_z, float i_hit);
		float repairCube(int i_x, int i_y, int i_z, float i_hit);
		bool upgradeCube(int i_x, int i_y, int i_z);
		int switchCubeType(int i_x, int i_y, int i_z, int i_newType);

        int getCube(int i_x, int i_y, int i_z)const{return m_map[i_x][i_y][i_z][CUBE_TYPE];}
		float getCubeMaxHitpoint(int i_x, int i_y, int i_z)const;
		const std::vector<int>* getFullCube(int i_x, int i_y, int i_z)const;

		//Action sur le chunk
		void updateAndSetBtObject(btDynamicsWorld* i_dynamicsWorld);
		void updateBuffer();
		void saveMap(std::ofstream& i_ofstreamFile);
		short loadMap(std::ifstream& i_ifstreamFile);
		void resizeChunk(int i_sizeX, int i_sizeY, int i_sizeZ);
		bool isHigherThen(int i_x, int i_y, int i_z, FaceType i_faceType)const;

		//accesseur
		bool isFull(int i_x, int i_y, int i_z)const;
		bool isBottomFilled(int i_x, int i_y, int i_z)const;
		void generateUsedVertex(int i_x, int i_y, int i_z, bool ia_usedVertex[16])const;

		void getMapColor(int i_x, int i_z, unsigned char i_colors[3])
		{
			for(int i(0); i < 3; ++i)
				i_colors[i] = m_chunkOverview[i_x][i_z][i];
		}



		void createLineMesh(int i_x, int i_y, int i_z, std::vector<Vector>& i_vertex, std::vector<GLuint>& i_indexList);

		bool isSmoothTopRenderable(int i_x, int i_y, int i_z)const;
		bool isFlatTopRenderable(int i_x, int i_y, int i_z)const;

		void getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result);
		void getVertex(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result, std::vector<VertexPosition>& i_oldVertexList);

		void updateDamage(int i_x, int i_y, int i_z);

		void setChangedFlag(int i_x, int i_y, int i_z, bool i_value);

		void setFullCube(int i_x, int i_y, int i_z);
		void setRenforcement(int i_x, int i_y, int i_z, int i_renforcement);

		float getIsoSurfaceRelativeHeight(int i_x, int i_y, int i_z, int i_vertexIndex)const;
		void setIsoSurfaceRelativeHeight(int i_x, int i_y, int i_z, int i_vertexIndex, float i_isoPosition);

		void getDensity(int i_x, int i_y, int i_z, float i_density[8])const;

		WhiteTerrainLine* getWhiteLine(){return &m_whiteTerrainLine;}
		int getX()const{return m_chunkPosition.x;}
		int getY()const{return m_chunkPosition.y;}
		int getZ()const{return m_chunkPosition.z;}

		const Vector3I& getCubePosition()const{return m_cubePosition;}

		int getSizeX()const{return m_sizeX;}
		int getSizeY()const{return m_sizeY;}
		int getSizeZ()const{return m_sizeZ;}

		void copyTerrain();

		void moveChunkBottomCenter(Vector3I i_position);
		void moveChunk(const Vector3I& i_position);
		void moveChunk(int i_x, int i_y, int i_z);

		void addWhitePrismeDrawable();
		void removeWhitePrismeDrawable();

		void fill(int i_cubeType, float i_hitpoint);
		float getCubeHitpoint(int i_x, int i_y, int i_z);
		

    protected:
		virtual void preTreatementDraw()const;
		
		virtual void transformModelview(Matrix& i_modelview, const Camera* ipk_camera)const;
    private:
		void setSingleDensity(int i_x, int i_y, int i_z, unsigned int i_index, float i_density);
		void generateExtraTopIndex(int i_x, int i_y, int i_z);
		void generateExtraTop(int i_x, int i_y, int i_z, std::vector<GLint>& i_textureNbList, std::vector<GLint>& i_renforcementList);
		float getDamage(int i_x, int i_y, int i_z);
		void updateOutline();
		void init();
		int generateCubeColor(int i_x, int i_y, int i_z);
		void getTransformedVertexList(std::vector<Vector>& o_transformedVertexList);

		bool isAirIntern(int i_x, int i_y, int i_z)const;
		bool isArtificialIntern(int i_x, int i_y, int i_z)const;
		bool isNaturalIntern(int i_x, int i_y, int i_z)const;
		bool isHigherThenIntern(int i_x, int i_y, int i_z, FaceType i_faceType)const;
		bool isTopRenderableIntern(int i_x, int i_y, int i_z)const;
		bool isFullIntern(int i_x, int i_y, int i_z)const;
		bool isBottomFilledIntern(int i_x, int i_y, int i_z)const;

		void getVertexIntern(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result);
		void getVertexIntern(int i_x, int i_y, int i_z, std::vector<VertexPosition*>& i_result, std::vector<VertexPosition>& i_oldVertexList);

		void initPhysics();
		void exitPhysics();
		btRigidBody* localCreateRigidBody(float mass, const btTransform& startTransform,btCollisionShape* shape);

		void addCompleteFace(int i_x, int i_y, int i_z, std::vector<VertexPosition>& i_vertex, FaceType i_faceType);
		void addVerticalPolygon(int i_x, int i_y, int i_z, int i_vertexOldStart, int i_vertexOldEnd, FaceType i_faceType);
		void addBottomPolygon(int i_x, int i_y, int i_z, int i_vertexOldStart, int i_vertexOldEnd);
		void createFlatTop(int i_x, int i_y, int i_z);
		
		//Mutateur privé
		void createNaturalCubeMesh(int i_x, int i_y, int i_z, std::vector<GLint>& i_textureNbList, std::vector<GLint>& i_renforcementList, std::vector<GLfloat>& i_damageList);
		void createArtificialCubeMesh(int i_x, int i_y, int i_z, std::vector<GLint>& i_textureNbList, std::vector<GLint>& i_renforcementList, std::vector<GLfloat>& i_damageList);

		void updateChunkOverview();
		void updateChunkOverview(int i_x, int i_z);

		//Accesseur interne
		void setFloatToMap(int i_x, int i_y, int i_z, int i_cubeInfo, float i_value);
		float getFloatFromMap(int i_x, int i_y, int i_z, int i_cubeInfo)const;

		int faceGotIsosurface(int i_x, int i_y, int i_z, FaceType i_faceType);
		void addTopCorners(int i_x, int i_y, int i_z, std::vector<VertexPosition>& i_vertexPositionList, FaceType i_faceType);

		void smoothNormal(int i_x, int i_y, int i_z, std::vector<VertexPosition>& i_oldVertexList);

		void resizeChunk();

		//Liste des cubes
        std::vector<std::vector<std::vector<std::vector<int>>>> m_map;

		int m_sizeX;
		int m_sizeY;
		int m_sizeZ;

		std::vector<std::vector<std::vector<unsigned char>>> m_chunkOverview;

		std::vector<VertexPosition> m_vertexList;
		std::vector<GLuint> m_indexList;
		std::vector<GLfloat> m_damageList;
		//Position du chunk
		Vector3I m_chunkPosition;
		Vector3I m_cubePosition;

		GLuint m_textureNbID;
		GLuint m_damageID;
		GLuint m_renforcementID;

		GLuint m_lineIBOID;
		GLuint m_lineVBOID;
		int m_lineIndexSize;

		//Physique
		btVector3* gVertices;
		btBvhTriangleMeshShape* trimeshShape;
		btRigidBody* m_staticBody;

		btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
		btTriangleIndexVertexArray* m_indexVertexArrays;
		btScalar m_defaultContactProcessingThreshold;

		WhiteTerrainLine m_whiteTerrainLine;
		RenderInformation m_outlineRenderInformation;

		const bool mk_isTerrainChunk;
};

#endif // CHUNK_H
