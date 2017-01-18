#ifndef DEFERRED_RENDERER_H
#define DEFERRED_RENDERER_H

#include <math.h>
#include <GL/glew.h>
#include <Vector>

#include "../Mesh/Mesh.h"

#include "GBuffer.h"
#include "LightsCommon.h"
#include "../RenderInformation.h"

class Camera;
class Drawable;

class DeferredRenderer
{
public:
	//Singleton
	static DeferredRenderer* getInstance()
	{
		if(mp_instance == 0)
		{
			mp_instance = new DeferredRenderer();
			if (!mp_instance->init()) 
				exit(-200); 
		}
		return mp_instance;
	}

	static void releaseInstance()
	{
		delete mp_instance;
		mp_instance = 0;
	}

    bool init();
    
	virtual void animate(Uint32 i_timestep);
    virtual void draw();
    void doGeometryPass(GBuffer& i_buffer);
	void doStencilPass(unsigned int i_pointLightIndex, GBuffer& i_buffer);
    void doPointLightPass(unsigned int i_pointLightIndex, GBuffer& i_buffer);
	void doDirectionalLightPass(GBuffer& i_buffer);
	void doSkyboxPass(GBuffer& i_buffer);
	void doWhiteLinePass(GBuffer& i_buffer);
	void doFinalPass(GBuffer& i_buffer);

	int addDrawable(Drawable* i_drawable);
	void removeDrawable(Drawable* i_drawable);

	int addWhiteLine(Drawable* i_whiteLine);
	void removeWhiteLine(Drawable* i_whiteLine);

private:
	//Instance du singleton
	static DeferredRenderer* mp_instance;
	DeferredRenderer() ;
	~DeferredRenderer(); 
    float calcPointLightBSphere(const PointLight& i_light);        
    void initLights();    

	void drawScene();
	void drawUnitPreview();

    SpotLight m_spotLight;
	DirectionalLight m_dirLight;
	std::vector<PointLight> m_pointLights;

	Drawable* mp_skybox;
    std::vector<Drawable*> mp_meshes;
	std::vector<Drawable*> mp_whiteLines;

	std::vector<Mesh*> mp_lightMeshes;
 
    GBuffer m_gBuffer;
	GBuffer m_gBufferUnitPreview;

	Matrix m_projection;
	Matrix m_view;

	std::vector<RenderInformation> m_lightRenderInformation;
};


#endif //DEFERRED_RENDERER_H