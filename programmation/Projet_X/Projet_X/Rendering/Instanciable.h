#ifndef INSTANCIABLE_H
#define INSTANCIABLE_H

#include <vector>
#include "Drawable.h"

#include "../Math/Matrix.h"

struct VertexBoneData
{        
    unsigned int IDs[NUM_BONES_PER_VEREX];
    float weights[NUM_BONES_PER_VEREX];

    VertexBoneData()
    {
        Reset();
    };
        
    void Reset()
    {
        ZERO_MEM(IDs);
        ZERO_MEM(weights);        
    }
        
    void addBoneData(unsigned int BoneID, float Weight);
};

struct MeshEntry {
    MeshEntry()
    {
        numIndices    = 0;
        baseVertex    = 0;
        baseIndex     = 0;
        materialIndex = INVALID_MATERIAL;
    }
        
    unsigned int numIndices;
    unsigned int baseVertex;
    unsigned int baseIndex;
    unsigned int materialIndex;
};

class MeshEntry;
struct RenderInformation;

class Instanciable: public Drawable
{
	public:
		Instanciable(ShaderType i_shaderType);
		~Instanciable();

		virtual void draw(const Matrix& ik_projection, Matrix& i_modelview, const Camera* ipk_camera)const;

		void addToRenderInfoList(RenderInformation& i_renderInformation);
		void removeFromRenderInfoList(RenderInformation& i_renderInformation);
		void clearRenderInfoList();
	protected:
		virtual void updateAdditionnalBuffers()const{}
		virtual Matrix getModelview(int i_index)const;

		void initDrawingState()const;
		void clearDrawingState()const;
		void updateInstances()const;

		std::vector<MeshEntry> m_entries;
		std::vector<RenderInformation*> mp_renderInfoList;

		Matrix m_normalisationMatrix;

};

#endif // INSTANCIABLE_H
