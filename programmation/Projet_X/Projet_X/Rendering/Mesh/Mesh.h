#ifndef MESH_H
#define	MESH_H

#include <string>
#include <vector>
#include <map>
#include <SDL/SDL.h>

#include <assert.h>
#include <GL/glew.h>
#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <postprocess.h> // Post processing flags

#include "../../Definition.h"
#include "../../Math/Matrix.h"

#include "Animation.h"
#include "../Instanciable.h"
#include "../../Shaders/Shader.h"

#include "../../Math/Vector.h"

enum {RED_ALPHA = 1, GREEN_ALPHA = 1 << 1, BLUE_ALPHA = 1 << 2, SKIP_LIGHT = 1 << 3};

class Unit;
struct AnimationInfo;
class Vector;
class Camera;
struct RenderInformation;

class Mesh:public Instanciable
{
	public:
		static Mesh* getLoadedMesh(const std::string& i_path);
		static Mesh* getMesh(const std::string& i_path, ShaderType i_shaderType, int i_alphaColor = 0);

		void boneTransform(Unit* ip_unit);
		
		void boneTransformBaked(Unit* ip_unit);

		//Animation
		void addAnimation(const std::string& i_path);
		bool loadMesh(const std::string& ik_filename);

		unsigned int getBonesLocation(int i_index){return m_bonesLocation[i_index];}

		int getBonesNum(){return m_boneList.size();}
		float* const getBakedAnimations(int i_animationIndex, int i_frameIndex, int i_bonesIndex){return m_bakedAnimations[i_animationIndex][i_frameIndex].second[i_bonesIndex].getValues();}
		float* const getBakedAnimationsTranspose(int i_animationIndex, int i_frameIndex, int i_bonesIndex);

		void getBakedAnimationsTranspose(int i_animationIndex, int i_frameIndex, int i_bonesIndex, std::vector<float>& i_value)const;

		virtual void animate(Uint32 i_timestep);

		void saveInfoToFile();
		void setCustom(int i_id, int i_value){m_customisation[i_id] = i_value;}
		
		static void getMeshesList(std::vector<std::string>& i_list);
	protected:
		void generateBaseCorrection(std::vector<VertexPosition>& i_vertexInformations);
		bool loadInfoFromFile();

		Mesh(const std::string& i_path, ShaderType i_shaderType, int i_alphaColor);
		virtual ~Mesh();

		virtual void preTreatementDraw()const;
		virtual void postTreatementDraw()const;

		void updateAdditionnalBuffers()const;

		void bakeAnimation(int i_animationIndex);

		void clear();
		void loadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones);
		bool initFromScene(const aiScene* pScene, const std::string& ik_filename);
		bool initMaterials(const aiScene* pScene, const std::string& ik_filename);

		void initMesh(unsigned int MeshIndex, const aiMesh* paiMesh, std::vector<VertexPosition>& i_vertices, 
					 std::vector<VertexBoneData>& Bones, std::vector<unsigned int>& Indices);

		void evaluateLocalTransformation(float i_animationTime, AnimationInfo* ip_animationInfo);

		void calcInterpolatedPosition(aiVector3D& i_out, float i_animationTime, int i_channelIndex, AnimationInfo* ip_animationInfo);
		void calcInterpolatedRotation(aiQuaternion& i_out, float i_animationTime, int i_channelIndex, AnimationInfo* ip_animationInfo);
		void calcInterpolatedScaling(aiVector3D& i_out, float i_animationTime, int i_channelIndex, AnimationInfo* ip_animationInfo);

		unsigned int findKeyFrameTranslation(float i_animationTime, const std::vector<aiVectorKey>* i_vectorKey, int i_channelIndex, AnimationInfo* ip_animationInfo);
		unsigned int findKeyFrameScaling(float i_animationTime, const std::vector<aiVectorKey>* i_vectorKey, int i_channelIndex, AnimationInfo* ip_animationInfo);
		unsigned int findKeyFrameRotation(float i_animationTime, const std::vector<aiQuatKey>* i_quatKey, int i_channelIndex, AnimationInfo* ip_animationInfo);

		void createBoneTree(const aiScene* ip_scene);
		Bone* createBoneTree( aiNode* ip_node, Bone* ip_parent, const aiScene* ip_scene);
		void calculateBoneToWorldTransform(Bone* ip_child);

		void calculateBoneToWorldTransformBaked(Bone* ip_child, Animation* ip_animation);
		void updateTransformsBaked(Bone* ip_node, Animation* ip_animation);

		void updateTransforms(Bone* ip_node);

		Assimp::Importer m_importer;

		static std::hash_map<std::string, Mesh*> mp_meshes;
		std::string m_path;

		//Animation Attributs

		Bone* mp_skeleton;
		std::hash_set<Bone*> m_boneSet;
		std::vector<Bone*> m_boneList;
		std::hash_set<std::string> m_boneNameSet;

		std::map<std::string, unsigned int> m_boneMapping;
		unsigned int m_numBones;

		unsigned int m_bonesLocation[MAX_BONES];
		
		std::hash_map<std::string, int> m_animationNameToID;
		std::vector<Animation> m_animations;
		bool m_animated;
		std::vector<std::vector<std::pair<float, std::vector<Matrix>>>> m_bakedAnimations;

		GLuint m_animMatrixTextureID;
		GLuint m_animMatrixBufferID;

		int m_animTextureID;

		int m_customisation[NB_CUSTOMISATION];
		int m_alphaColor;
};


#endif	/* MESH_H */

